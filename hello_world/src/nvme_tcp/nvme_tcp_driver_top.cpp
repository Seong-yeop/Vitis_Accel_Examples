#include <stdint.h>
#include <ap_int.h>
#include <hls_stream.h>
#include <stdint.h>
#include <string.h>
#include "nvme_tcp_driver_top.hpp"


void rw_from_net_to_nvme_drv( // Just for dummy build
    hls::stream<ap_uint<DATA_WIDTH>> &nvme_tcp_rxdata, // receive
    hls::stream<ap_uint<DATA_WIDTH>> &nvme_tcp_txdata, // transmit
    hls::stream<struct request_packet> &nvme_request_stream,
    hls::stream<struct completion_packet> &nvme_response_stream
){
    if(!nvme_tcp_rxdata.empty()) {
        ap_uint<DATA_WIDTH> data = nvme_tcp_rxdata.read();
        struct request_packet req;
        nvme_request_stream.write(req);

    }
    else if(!nvme_response_stream.empty()) {
        struct completion_packet read_packet;
        read_packet = nvme_response_stream.read();
        nvme_tcp_txdata.write(0x0);

    }
}


extern "C" {
    void nvme_tcp_driver_top(
        // from network stream
        hls::stream<ap_uint<DATA_WIDTH>> &nvme_tcp_rxdata, // receive
        hls::stream<ap_uint<DATA_WIDTH>> &nvme_tcp_txdata, // transmit

        // admin queue address
        nvme_cqe_t *admin_cq_base_addr, // CQ base address
        nvme_sqe_t *admin_sq_base_addr, // SQ base address
         
        // NVMe SSD Bar addr
        uint32_t *bar_base_address, // Doorbell base address

        // admin queue doorbell base addr
        uint32_t *dbl_base_address, // Doorbell base address

        // for identify command
        uint64_t buffer_physical_address,
        uint64_t* buffer_virtual_address,

        // for nvme driver
        hls::stream<struct request_packet> &nvme_request_stream,
        hls::stream<struct completion_packet> &nvme_response_stream
    )
    {
        #pragma HLS DATAFLOW disable_start_propagation
        #pragma HLS INTERFACE ap_ctrl_none port=return

        #pragma HLS INTERFACE m_axi port=nvme_tcp_rxdata
        #pragma HLS INTERFACE m_axi port=nvme_tcp_txdata

        #pragma HLS INTERFACE m_axi port=admin_cq_base_addr offset=slave bundle=gmem0
        #pragma HLS INTERFACE m_axi port=admin_sq_base_addr offset=slave bundle=gmem1
        #pragma HLS INTERFACE m_axi port=bar_base_address offset=slave bundle=gmem2
        #pragma HLS INTERFACE m_axi port=dbl_base_address offset=slave bundle=gmem3
        #pragma HLS INTERFACE m_axi port=buffer_virtual_address offset=slave bundle=gmem4

        #pragma HLS INTERFACE m_axi port=nvme_response_stream
        
        rw_from_net_to_nvme_drv(
            nvme_tcp_rxdata,
            nvme_tcp_txdata,
            nvme_request_stream,
            nvme_response_stream
        );

        
        
    }
}