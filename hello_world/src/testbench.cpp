#include <iostream>
#include <hls_stream.h>
#include <cstdint>

#include "process_request.hpp"

#define PACKET_SIZE 1500

int main() {
    // 호스트 메모리 공간에 매핑된 배열을 생성하여 커널에 전달
    struct rx_packet_host rxq_addresses[QUEUE_SIZE] = {0};
    struct rx_packet_host txq_addresses[QUEUE_SIZE] = {0};

    char rx_buffer[QUEUE_SIZE * PACKET_SIZE] = {0};
    char tx_buffer[QUEUE_SIZE * PACKET_SIZE] = {0};

    for (int i = 0; i < 10; i++) {
        struct rx_packet_host *packet = &rxq_addresses[i];
    }

    // 테스트용 레지스터 초기화
    uint32_t rx_head = 0;
    uint32_t rx_tail = 0;
    uint32_t tx_head = 0;
    uint32_t tx_tail = 0;

    // `process_request` 함수 호출
    process_request((char*)rxq_addresses, rx_buffer, (char*)txq_addresses, tx_buffer, &rx_head, &rx_tail, &tx_head, &tx_tail);

    // 결과 확인
    std::cout << "rx_head: " << rx_head << std::endl;
    std::cout << "rx_tail: " << rx_tail << std::endl;
    std::cout << "tx_head: " << tx_head << std::endl;
    std::cout << "tx_tail: " << tx_tail << std::endl;

    bool passed = true;
    for (int i = 0; i < 10; i++) {
        if (rxq_addresses[i].session_id != i) {
            std::cerr << "Test failed at index " << i << ": Expected " << i
                      << " but got " << rxq_addresses[i].session_id << std::endl;
            passed = false;
        }
        else {
            std::cout << "Test passed at index " << i << ": " << rxq_addresses[i].session_id << std::endl;
            std::cout << "length: " << rxq_addresses[i].length << std::endl;

            for (int j = 0; j < 5; j++) {
                std::cout << rx_buffer[PACKET_SIZE*i + j];
            }
            std::cout << std::endl;
        }
    }
        
    if (passed) {
        std::cout << "Test passed!" << std::endl;
    } else {
        std::cout << "Test failed!" << std::endl;
    }
    
    for (int i = 0; i <10; i++) {
        txq_addresses[i].session_id = i;
        txq_addresses[i].length = 1;
        tx_buffer[1500*i] = 65;
        tx_tail = (tx_tail + 1) % QUEUE_SIZE;
    }

    process_request((char*)rxq_addresses, rx_buffer, (char*)txq_addresses, tx_buffer, &rx_head, &rx_tail, &tx_head, &tx_tail);

    return 0;
}