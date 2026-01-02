#include <stdio.h>
#include <string.h>
#include <hls_stream.h>
#include "nvme_driver_top.hpp"

#define TEST_REQUEST_COUNT 10
#define TEST_NLB 8
#define SIMULATION_CYCLES 50000

// NVMe Controller Mock: Read commands from SQ and generate completions in CQ
class NvmeControllerMock {
public:
    uint32_t sq_head;
    uint32_t cq_tail;
    uint8_t cq_phase;

    NvmeControllerMock() : sq_head(0), cq_tail(0), cq_phase(1) {}

    // Generate completion when SQ doorbell change is detected
    void process(
        nvme_io_command_t* io_sq_base,
        nvme_cqe_t* io_cq_base,
        uint32_t* dbl_base
    ) {
        uint32_t sq_tail = dbl_base[2]; // SQ tail doorbell

        // Process new commands in SQ
        while (sq_head != sq_tail) {
            nvme_io_command_t cmd = io_sq_base[sq_head];

            // Create completion entry
            nvme_cqe_t cqe;
            cqe.dw0 = 0;
            cqe.dw1 = 0;
            cqe.sq_head = (sq_head + 1) % IO_QUEUE_MAX_DEPTH;
            cqe.sq_identifier = 0;
            cqe.cid = cmd.cid;
            cqe.phase_tag = cq_phase;
            cqe.status_code = 0; // Success
            cqe.sct = 0;
            cqe.reserved = 0;

            io_cq_base[cq_tail] = cqe;

            // Update CQ tail
            cq_tail = (cq_tail + 1) % IO_QUEUE_MAX_DEPTH;
            if (cq_tail == 0) {
                cq_phase = !cq_phase;
            }

            sq_head = (sq_head + 1) % IO_QUEUE_MAX_DEPTH;

            printf("[Controller] Processed cmd CID=%u, OPC=0x%02x, SLBA=0x%08x%08x, NLB=%u\n",
                   cmd.cid, cmd.opc, cmd.cdw11, cmd.cdw10, cmd.cdw12 + 1);
        }
    }
};

int main() {
    printf("=== NVMe Driver Top Testbench ===\n\n");

    // Memory allocation and initialization
    nvme_cqe_t io_cq_base_addr[IO_QUEUE_MAX_DEPTH];
    nvme_io_command_t io_sq_base_addr[IO_QUEUE_MAX_DEPTH];
    uint32_t dbl_base_address[16] = {0};
    uint32_t dbl_base_address2[16] = {0};
    uint64_t buffer_base_address[4096] = {0};
    uint64_t prp2_virtual_address[IO_QUEUE_MAX_DEPTH * 512] = {0};

    memset(io_cq_base_addr, 0, sizeof(io_cq_base_addr));
    memset(io_sq_base_addr, 0, sizeof(io_sq_base_addr));

    // Control signals
    uint32_t start = 1;
    uint32_t done = 0;
    uint32_t done_ack = 0;

    // Completion counters
    uint64_t completed_request_number = 0;
    uint64_t completed_request_bytes = 0;

    // PRP2 physical address (dummy)
    uint64_t prp2_physical_address = 0x100000000ULL;

    // Delay cycles for CQ polling
    uint32_t delay_cycles = 100;

    // Request packet stream
    hls::stream<struct request_packet> request_packet_stream("request_packet_stream");

    // NVMe Controller Mock
    NvmeControllerMock controller;

    // Generate test request packets
    printf("[Test] Generating %d request packets (NLB=%d each)\n", TEST_REQUEST_COUNT, TEST_NLB);
    packet_generator(
        NVME_IO_READ,      // opcode
        0x0,               // slba
        TEST_NLB,          // nlb
        0x200000000ULL,    // prp1 base
        TEST_REQUEST_COUNT,// request count
        request_packet_stream
    );
    printf("[Test] Generated %d requests in stream\n\n", (int)request_packet_stream.size());

    // Simulation loop
    printf("[Test] Starting simulation for %d cycles...\n\n", SIMULATION_CYCLES);

    for (int cycle = 0; cycle < SIMULATION_CYCLES; cycle++) {
        // Call NVMe Driver Top
        nvme_driver_top(
            start,
            done,
            done_ack,
            io_cq_base_addr,
            io_sq_base_addr,
            dbl_base_address,
            dbl_base_address2,
            buffer_base_address,
            request_packet_stream,
            completed_request_number,
            completed_request_bytes,
            prp2_physical_address,
            prp2_virtual_address,
            delay_cycles
        );

        // NVMe Controller Mock: Process SQ and generate CQ completions
        controller.process(io_sq_base_addr, io_cq_base_addr, dbl_base_address);

        // Sync dbl_base_address2 with dbl_base_address (shared doorbell)
        dbl_base_address2[3] = dbl_base_address[3];

        // Print progress every 5000 cycles
        if (cycle > 0 && cycle % 5000 == 0) {
            printf("[Cycle %d] Completed: %lu requests, %lu bytes\n",
                   cycle, (unsigned long)completed_request_number,
                   (unsigned long)completed_request_bytes);
        }
    }

    // Print results
    printf("\n=== Test Results ===\n");
    printf("Completed Requests: %lu / %d\n",
           (unsigned long)completed_request_number, TEST_REQUEST_COUNT);
    printf("Completed Bytes: %lu (expected: %lu)\n",
           (unsigned long)completed_request_bytes,
           (unsigned long)(TEST_REQUEST_COUNT * TEST_NLB * 4096));
    printf("SQ Tail Doorbell: %u\n", dbl_base_address[2]);
    printf("CQ Head Doorbell: %u\n", dbl_base_address[3]);

    // Verification
    int result = 0;
    if (completed_request_number != TEST_REQUEST_COUNT) {
        printf("\n[FAIL] Request count mismatch!\n");
        result = 1;
    }
    if (completed_request_bytes != (uint64_t)TEST_REQUEST_COUNT * TEST_NLB * 4096) {
        printf("\n[FAIL] Completed bytes mismatch!\n");
        result = 1;
    }

    if (result == 0) {
        printf("\n[PASS] All tests passed!\n");
    }

    return result;
}
