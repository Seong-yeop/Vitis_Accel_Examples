#include <stdint.h>
#include <hls_stream.h>
#include <hls_print.h>

#include "nvme_driver_top.hpp"

extern "C" {
    void nvme_driver_top(
        nvme_cqe_t *io_cq_base_addr, // CQ base address
        nvme_io_command_t *io_sq_base_addr, // SQ base address
        uint32_t *dbl_base_address, // Doorbell base address
        uint32_t *dbl_base_address2, // Doorbell base address

        hls::stream<struct request_packet> &request_packet_stream,
        hls::stream<struct completion_packet> &response_packet_stream,

        /* NVMe Controller Completion Interface */
        uint64_t &completed_request_number,
        uint64_t &completed_request_bytes,
        uint64_t prp2_physical_address,
        uint64_t* prp2_virtual_address,

        uint32_t delay_cycles
    )
    {
        #pragma HLS DATAFLOW disable_start_propagation
        #pragma HLS INTERFACE ap_ctrl_none port=return

        #pragma HLS INTERFACE m_axi port=io_cq_base_addr offset=slave bundle=gmem0
        #pragma HLS INTERFACE m_axi port=io_sq_base_addr offset=slave bundle=gmem1
        #pragma HLS INTERFACE m_axi port=dbl_base_address offset=slave bundle=gmem2
        #pragma HLS INTERFACE m_axi port=dbl_base_address2 offset=slave bundle=gmem3

        #pragma HLS INTERFACE m_axi port=request_packet_stream
        #pragma HLS INTERFACE m_axi port=response_packet_stream

        static hls::stream<struct mgmt_table_req> submit_in_req("submit_in_req");
        #pragma HLS STREAM variable=submit_in_req depth=512
        static hls::stream<struct mgmt_table_resp> submit_out_resp("submit_out_resp");
        #pragma HLS STREAM variable=submit_out_resp depth=512
        static hls::stream<struct mgmt_table_req> cpl_in_req("cpl_in_req");
        #pragma HLS STREAM variable=cpl_in_req depth=512
        static hls::stream<struct mgmt_table_resp> cpl_out_resp("cpl_out_resp");
        #pragma HLS STREAM variable=cpl_out_resp depth=512

        // static hls::stream<nvme_io_command_t> nvme_prp_req_stream;
        // #pragma HLS STREAM variable=nvme_prp_req_stream depth=512
        // static hls::stream<nvme_io_command_t> nvme_io_command_stream;
        // #pragma HLS STREAM variable=nvme_io_command_stream depth=512
            
        static hls::stream<uint32_t> nvme_cq_polling_stream;
        #pragma HLS STREAM variable=nvme_cq_polling_stream depth=512

        static hls::stream<uint32_t> sq_head_stream_write;
        #pragma HLS STREAM variable=sq_head_stream_write depth=512

        static hls::stream<uint32_t> sq_head_stream_read;
        #pragma HLS STREAM variable=sq_head_stream_read depth=512


        static uint32_t sq_tail = 0;
        static uint32_t sq_head = 0;
        static uint32_t cq_tail = 0;
        static uint32_t cq_head = 0; 


        nvme_io_submit(
            io_sq_base_addr,
            io_cq_base_addr,
            dbl_base_address,
            // sq_tail,
            prp2_physical_address,
            prp2_virtual_address,
            request_packet_stream,
            submit_in_req,
            submit_out_resp,
            nvme_cq_polling_stream,
            sq_head_stream_read
        );
        
        mgmt_table(
            submit_in_req,
            submit_out_resp,
            cpl_in_req,
            cpl_out_resp,
            sq_head_stream_write,
            sq_head_stream_read
        );

        //CQ polling 
        nvme_process_cpl(
            io_cq_base_addr,
            dbl_base_address2,
            completed_request_number,
            completed_request_bytes,
            cpl_in_req,
            cpl_out_resp,
            nvme_cq_polling_stream,
            response_packet_stream,
            sq_head_stream_write,
            delay_cycles
        );
    }
}