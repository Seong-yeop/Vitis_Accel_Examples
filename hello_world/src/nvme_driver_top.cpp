#include <stdint.h>
#include <hls_stream.h>
#include <hls_print.h>

#include "nvme_driver_top.hpp"

extern "C" {
    void nvme_driver_top(
        uint32_t &start,
        uint32_t &done,
        uint32_t &done_ack,

        nvme_cqe_t *io_cq_base_addr, // CQ base address
        nvme_io_command_t *io_sq_base_addr, // SQ base address
        uint32_t *dbl_base_address, // Doorbell base address
        uint64_t *buffer_base_address,

        hls::stream<struct request_packet> &request_packet_stream,
        /* NVMe Controller Completion Interface */
        uint32_t &completed_request_number,
        uint32_t &completed_request_bytes,
        uint64_t prp2_physical_address,
        uint64_t* prp2_virtual_address
    )
    {
        #pragma HLS INTERFACE ap_ctrl_none port=return

        #pragma HLS INTERFACE m_axi port=io_cq_base_addr offset=slave bundle=gmem0
        #pragma HLS INTERFACE m_axi port=io_sq_base_addr offset=slave bundle=gmem0
        #pragma HLS INTERFACE m_axi port=dbl_base_address offset=slave bundle=gmem0
        #pragma HLS INTERFACE m_axi port=buffer_base_address offset=slave bundle=gmem1

        #pragma HLS INTERFACE s_axilite port=start 
        #pragma HLS INTERFACE s_axilite port=done 
        #pragma HLS INTERFACE s_axilite port=done_ack 

        #pragma HLS INTERFACE m_axi port=request_packet_stream

        // #pragma HLS INTERFACE s_axilite port=opcode 
        // #pragma HLS INTERFACE s_axilite port=lba 
        // #pragma HLS INTERFACE s_axilite port=nlba 
        // #pragma HLS INTERFACE s_axilite port=prp1 
        // #pragma HLS INTERFACE s_axilite port=request_count 

        // static hls::stream<struct request_packet> request_packet_stream("request_packet_stream");
        // #pragma HLS STREAM variable=request_packet_stream depth=512

        static uint32_t sq_tail = 0;
        static uint32_t sq_head = 0;
        static uint32_t cq_tail = 0;
        static uint32_t cq_head = 0;
    
        #pragma HLS DATAFLOW

        nvme_io_submit(
            io_sq_base_addr, 
            io_cq_base_addr, 
            dbl_base_address, 
            buffer_base_address,
            sq_tail, 
            prp2_physical_address,  
            prp2_virtual_address,           
            request_packet_stream
        );
    }
}