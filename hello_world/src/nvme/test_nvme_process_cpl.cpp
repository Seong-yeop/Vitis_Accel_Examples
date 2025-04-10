#include <stdio.h>
#include "nvme_driver_top.hpp"
#include <hls_stream.h>

// 테스트 벤치 메인 함수
int main() {
    nvme_cqe_t io_cq_base_addr[IO_QUEUE_MAX_DEPTH];
    uint32_t dbl_base_address[16] = {0};

    uint32_t completed_requests_count = 0;
    uint32_t completed_requests_bytes = 0;

    hls::stream<mgmt_table_req> cpl_in_req;
    hls::stream<mgmt_table_resp> cpl_out_resp;
    hls::stream<uint32_t> nvme_cq_polling_stream;
    hls::stream<uint32_t> sq_head_stream_write;

    // IO Completion Queue Entry 초기화 (테스트용)
    for (int i = 0; i < IO_QUEUE_MAX_DEPTH; i++) {
        io_cq_base_addr[i].phase_tag = 1;
        io_cq_base_addr[i].sq_head = i;
        io_cq_base_addr[i].cid = i;
    }

    // mgmt_table_resp 초기화 (응답 미리 준비)
    mgmt_table_resp test_resp;
    test_resp.rdata.nlb = 7; // 8개 블록 (8 * 4096 bytes = 32KB)

    // 테스트 벤치 실행 루프
    for (int cycle = 0; cycle < 1000; cycle++) {
        // IDLE 상태에서 NVMe Completion Queue polling 신호를 주입
        if (cycle % 10 == 0) {
            nvme_cq_polling_stream.write(0); 
        }

        // cpl_out_resp 스트림에 응답 미리 넣기
        if (cycle % 10 == 0) {
            cpl_out_resp.write(test_resp);
        }

        // nvme_process_cpl 함수 호출
        nvme_process_cpl(
            io_cq_base_addr,
            dbl_base_address,
            completed_requests_count,
            completed_requests_bytes,
            cpl_in_req,
            cpl_out_resp,
            nvme_cq_polling_stream,
            sq_head_stream_write
        );

        // mgmt_table_req 스트림에서 데이터 읽어서 확인하기
        if (!cpl_in_req.empty()) {
            mgmt_table_req req = cpl_in_req.read();
            printf("[Cycle %d] cpl_in_req: op=%u, cid=%u\n", cycle, req.op, req.cid);
        }

        // sq_head_stream_write 스트림에서 데이터 읽어서 확인하기
        if (!sq_head_stream_write.empty()) {
            uint32_t sq_head = sq_head_stream_write.read();
            printf("[Cycle %d] sq_head_stream_write: sq_head=%u\n", cycle, sq_head);
        }
    }

    // 테스트 결과 출력
    printf("\n==== 테스트 결과 ====\n");
    printf("Completed Requests Count: %u\n", completed_requests_count);
    printf("Completed Requests Bytes: %u\n", completed_requests_bytes);
    printf("Doorbell Address CQ head (dbl_base_address[3]): %u\n", dbl_base_address[3]);

    return 0;
}
