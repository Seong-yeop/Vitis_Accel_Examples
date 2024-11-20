#ifndef PROCESS_REQUEST_HPP
#define PROCESS_REQUEST_HPP

#include <stdint.h>
#include <hls_stream.h>

#include "tcp.hpp"

#define QUEUE_SIZE 1024


// Function prototypes
template <int D>
void fill_rx_packet_stream(hls::stream<rx_packet<D>>& rx_packet_stream);

template <int D>
void rx_handler(uint64_t* rxq_addresses, hls::stream<rx_packet<D>>& rx_packet_stream, hls::stream<uint32_t>& rx_head_stream, hls::stream<uint32_t>& rx_tail_stream);   

void update_rx_tail(hls::stream<uint32_t>& rx_tail_stream, uint32_t* rx_tail, uint32_t* tx_head);
void update_rx_head_stream(uint32_t* rx_head, hls::stream<uint32_t>& rx_head_stream);


extern "C" {
    void process_request(char* rxq_addresses, 
                         char* rx_buffer,
                         char* txq_addresses,
                         uint32_t* rx_head, 
                         uint32_t* rx_tail, 
                         uint32_t* tx_head, 
                         uint32_t* tx_tail);
}

extern "C" {
void send_kernel(char* txq_addresses, 
                 char *data,
                 uint32_t* tx_head, 
                 uint32_t* tx_tail); 
}
#endif // PROCESS_REQUEST_HPP
