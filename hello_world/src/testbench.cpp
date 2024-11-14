#include <iostream>
#include <hls_stream.h>
#include <cstdint>

#include "process_request.hpp"

#define DATA_SIZE 6

int main() {
    // 호스트 메모리 공간에 매핑된 배열을 생성하여 커널에 전달


    char rxq_addresses[QUEUE_SIZE*DATA_SIZE] = {0};
    char txq_addresses[DATA_SIZE] = {0};

    // 테스트용 레지스터 초기화
    uint32_t rx_head = 0;
    uint32_t rx_tail = 0;
    uint32_t tx_head = 0;
    uint32_t tx_tail = 0;

    // 초기 값을 설정하여 테스트
    rx_head = 0;
    tx_tail = 0;

    // `process_request` 함수 호출
    process_request(rxq_addresses, txq_addresses, &rx_head, &rx_tail, &tx_head, &tx_tail);

    // 결과 확인
    std::cout << "rx_tail: " << rx_tail << std::endl;
    std::cout << "tx_head: " << tx_head << std::endl;
    std::cout << "tx_tail: " << tx_tail << std::endl;

    bool passed = true;
    for (int i = 0; i < 10; i++) {
        if (rxq_addresses[i * sizeof(rx_packet_host)] != i) {
            std::cerr << "Test failed at index " << i << ": Expected " << i
                      << " but got " << rxq_addresses[i] << std::endl;
            passed = false;
        }
    }

    if (passed) {
        std::cout << "Test passed!" << std::endl;
    } else {
        std::cout << "Test failed!" << std::endl;
    }

    return 0;
}
