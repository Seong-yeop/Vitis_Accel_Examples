#include <stdint.h>
#include <hls_stream.h>

// Example constants
#define BURST_SIZE 256

extern "C" {
void process_request(
    // S_AXILite control registers
    uint64_t base_addr,   // 64-bit pointer to pinned host memory
    uint32_t size_bytes,  // number of bytes to copy
    // M_AXI to do the actual read/write from pinned host memory
    volatile uint32_t* mem
    )
{
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE s_axilite      port=base_addr
#pragma HLS INTERFACE s_axilite      port=size_bytes
// #pragma HLS INTERFACE s_axilite      port=mem
// or #pragma HLS INTERFACE s_axilite port=mem bundle=control // if needed

// M_AXI interface for the actual DMA
#pragma HLS INTERFACE m_axi     depth=65536  max_read_burst_length=256 max_write_burst_length=256 \
                     offset=slave port=mem

    // For demonstration, we'll do a single pass, but you can also do while(true).
    // 1) Read data from pinned host memory
    // 2) Modify it
    // 3) Write it back

    uint32_t total = size_bytes;
    uint64_t read_addr = base_addr;  // byte address
    uint64_t write_addr = base_addr; // we'll write back at the same location

    // A small local buffer
    uint32_t local_buf[BURST_SIZE];
#pragma HLS ARRAY_PARTITION variable=local_buf complete dim=1

    while (total > 0) {
        // Step A: decide chunk size
        uint32_t chunk = ((total < BURST_SIZE) ? total : BURST_SIZE) / 4; // in 32-bit words

        // Step B: Read from host memory 
        for (int i=0; i < chunk; i++) {
#pragma HLS PIPELINE II=1
            // Convert read_addr from bytes to "index" if necessary
            // For an 8-bit wide (uint8_t *) M_AXI, it's direct:
            local_buf[i] = mem[(read_addr >> 2) + i];
        }

        // Step C: Modify data (e.g., invert bits)
        for (int i=0; i < chunk; i++) {
#pragma HLS PIPELINE II=1
            local_buf[i] = local_buf[i] + 1;
        }

        // Step D: Write back
        for (int i=0; i < chunk; i++) {
#pragma HLS PIPELINE II=1
            mem[(write_addr >> 2) + i] = local_buf[i];
        }

        read_addr  += (chunk * 4);
        write_addr += (chunk * 4);
        total      -= (chunk * 4);
    }
}

}