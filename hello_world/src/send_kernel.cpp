#include <stdint.h>
#include <hls_stream.h>
#include <cstdio>  // printf를 사용하기 위한 표준 C 라이브러리 포함

#include "process_request.hpp"
#include "tcp.hpp"

void tx_handler(char *txq_addresses,
                char *data,
                uint32_t* tx_head,
                uint32_t* tx_tail) 
{
    char tmp[1500] = {0};
    // Read from txq_addresses
    for (uint32_t i = *tx_head; i < *tx_tail; i++) {
        // #pragma HLS PIPELINE
        uint16_t session_id = *(uint16_t*)&txq_addresses[i*4];
        uint16_t length = *(uint16_t*)&txq_addresses[i*4 + 2];
        printf("session_id: %d, length: %d\n", session_id, length);
        // Process or send the data
        for (int i = 0; i < length; i++) {
            printf("%d", data[i]);
            tmp[i] = data[i];
        }
        printf("\n");
        *tx_head = (*tx_head + 1) % QUEUE_SIZE;
    }
    // Update tx_tail
}

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

    tx_handler(txq_addresses, data, tx_head, tx_tail);
}
}