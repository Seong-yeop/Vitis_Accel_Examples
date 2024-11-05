#include <stdint.h>
#include <hls_stream.h>

#include <cstdio>  // printf를 사용하기 위한 표준 C 라이브러리 포함


#define QUEUE_SIZE 1024

// FPGA에서 SQ와 CQ를 지속적으로 처리하는 커널
static void process_submission_queue(unsigned int* sq_buffer, 
                                    unsigned int* cq_buffer, 
                                    volatile unsigned int* sq_head, 
                                    volatile unsigned int* sq_tail, 
                                    volatile unsigned int* cq_head, 
                                    volatile unsigned int* cq_tail) 
{
sq_processing:

    uint32_t max_iters = 1000;
    uint32_t iters = 0;
    #pragma HLS pipeline off
    while (iters < max_iters) {  // 커널이 종료되지 않고 계속 실행
        iters++;
        // #pragma HLS PIPELINE II=1
        // SQ에서 새로운 명령이 있는지 확인
        #pragma HLS RESOURCE variable=sq_head core=AXI4LiteS metadata="-bus_bundle CONTROL"
        #pragma HLS RESOURCE variable=cq_tail core=AXI4LiteS metadata="-bus_bundle CONTROL"
        printf("Checking for new commands in SQ: sq_head = %u, sq_tail = %u\n", *sq_head, *sq_tail);
        if (*sq_head != *sq_tail && (*cq_tail + 1) % QUEUE_SIZE != *cq_head) {  // SQ가 비어 있지 않거나 CQ가 가득 차지 않은 경우
            unsigned int cmd = sq_buffer[*sq_head];  // SQ에서 명령 읽기
            *sq_head = (*sq_head + 1) % QUEUE_SIZE;  // SQ head 포인터 업데이트
            
            printf("Processing command from SQ: cmd = %u, sq_head = %u, sq_tail = %u\n", cmd, *sq_head, *sq_tail);

            cq_buffer[*cq_tail] = cmd + 1;  // 결과 기록
            *cq_tail = (*cq_tail + 1) % QUEUE_SIZE;  // CQ tail 포인터 업데이트
        } else {
            printf("No new commands in SQ or CQ is full, Waiting...\n");  // SQ가 비어 있는 경우
        }
    }
}

extern "C" {
void process_request(unsigned int* sq_buffer, 
        unsigned int* cq_buffer, 
        volatile unsigned int* sq_head, 
        volatile unsigned int* sq_tail, 
        volatile unsigned int* cq_head, 
        volatile unsigned int* cq_tail) 
{
    #pragma HLS INTERFACE m_axi port = sq_buffer offset = slave bundle = gmem0
    #pragma HLS INTERFACE m_axi port = cq_buffer offset = slave bundle = gmem1
    #pragma HLS INTERFACE m_axi port = sq_head offset = slave bundle = gmem0
    #pragma HLS INTERFACE m_axi port = sq_tail offset = slave bundle = gmem0
    #pragma HLS INTERFACE m_axi port = cq_head offset = slave bundle = gmem1
    #pragma HLS INTERFACE m_axi port = cq_tail offset = slave bundle = gmem1

    //process_submission_queue(sq_buffer, cq_buffer, sq_head, sq_tail, cq_head, cq_tail);
}
}