#include <stdint.h>
#include <hls_stream.h>
#include <cstdio>  // printf를 사용하기 위한 표준 C 라이브러리 포함

#include "process_request.hpp"
#include "tcp.hpp"


extern "C" {
void send_kernel(char* txq_addresses, 
                 char *data,
                 uint32_t* tx_head, 
                 uint32_t* tx_tail) {
    #pragma HLS INTERFACE m_axi port=txq_addresses bundle=gmem1
    #pragma HLS INTERFACE m_axi port=data bundle=gmem1
    #pragma HLS INTERFACE s_axilite port=tx_head 
    #pragma HLS INTERFACE s_axilite port=tx_tail 
    #pragma HLS INTERFACE s_axilite port=return

    // tx_handler(txq_addresses, data, tx_head, tx_tail);
}
}