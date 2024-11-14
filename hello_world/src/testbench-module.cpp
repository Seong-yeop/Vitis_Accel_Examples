#include <iostream>
#include <hls_stream.h>
#include <cassert>

#include "process_request.hpp"

#define QUEUE_SIZE 1024


int main() {
    // Initialize input stream and buffer for rx_packet
    hls::stream<rx_packet<512>> rx_packet_stream;
    #pragma HLS STREAM variable=rx_packet_stream depth=16

    hls::stream<uint32_t> rx_head_stream;
    hls::stream<uint32_t> rx_tail_stream;
    
    struct rx_packet_host rxq[QUEUE_SIZE] = {0};
    uint64_t rxq_addresses[QUEUE_SIZE] = {0};

    // for (int i = 0; i < QUEUE_SIZE; i++) {
    //     rxq_addresses[i] = reinterpret_cast<uint64_t>(&rxq[i]);
    //     printf("rxq_addresses[%d]: %p\n", i, rxq_addresses[i]);
    // }

    // Initialize rx_head and rx_tail
    uint32_t *rx_head = (uint32_t*)malloc(sizeof(uint32_t));
    uint32_t *rx_tail = (uint32_t*)malloc(sizeof(uint32_t));
    uint32_t *tx_head = (uint32_t*)malloc(sizeof(uint32_t));

    *rx_head = 0;
    *rx_tail = 0;
    *tx_head = 0;

    // for (int i = 0; i < 10; i++) {
    //     rx_packet<512> packet;
    //     packet.session_id = i;
    //     packet.length = 64;
    //     rx_packet_stream.write(packet);
    // }

    int count = 0;
    while (count++ < 100) {
        fill_rx_packet_stream(rx_packet_stream);
        update_rx_head_stream(rx_head, rx_head_stream);
        rx_handler(rxq_addresses, rx_packet_stream, rx_head_stream, rx_tail_stream);
        update_rx_tail(rx_tail_stream, rx_tail, tx_head);
    }
    
    printf("tx_head: %d\n", *tx_head);

    // Check results directly using rxq
    bool passed = true;
    for (uint32_t i = 0; i <= *rx_tail; i++) {
        if (rxq[i].session_id != i || rxq[i].length != 64) {
            std::cerr << "Test failed at index " << i << ": Expected session_id="
                      << i << ", length=64 but got session_id="
                      << rxq[i].session_id << ", length=" << rxq[i].length << std::endl;
            passed = false;
        }
        else {
            std::cout << "Test passed at index " << i << ": Expected session_id="
                      << i << ", length=64 and got session_id="
                      << rxq[i].session_id << ", length=" << rxq[i].length << std::endl;
        }
    }

    if (passed) {
        std::cout << "CSIM Test Passed: All packets in rxq_addresses match expected values." << std::endl;
    } else {
        std::cerr << "CSIM Test Failed: Some packets in rxq_addresses did not match expected values." << std::endl;
    }

    std::cout << "Rx_tail: " << *rx_tail << std::endl;

    return 0;
}
