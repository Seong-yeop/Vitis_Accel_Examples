#include <ap_int.h>
#include <hls_stream.h>
#include <stdint.h>
#include <string.h>

#include "nvme_driver_top.hpp"


void nvme_io_cmd_gen(
    hls::stream<struct request_packet> &request_packet_stream,
    hls::stream<nvme_io_command_t> &nvme_prp_req_stream
)
{
    #pragma HLS INLINE off
    #pragma HLS PIPELINE II=1

    if (!request_packet_stream.empty()) 
    {
        struct request_packet req = request_packet_stream.read();
        nvme_io_command_t cmd;
        // DW0
        cmd.opc  = req.dw0.opc; // 0x01=Write, 0x02=Read

        cmd.fuse = 0;
        cmd.rsvd = 0;
        cmd.cid  = req.dw0.id;
    
        // DW1
        cmd.nsid = 1;
    
        // DW2-DW3
        cmd.cdw2 = 0;
        cmd.cdw3 = 0;
    
        // DW4-DW5
        cmd.mptr = 0; // metadata 불필요 시 0
    
        // DW6-DW7
        cmd.prp1 = req.dw3;
        // cmd.prp1 = (req.dw2.slba << 16 || req.dw1);  // prp1
        // cmd.prp2 = prp2; // nlb가 1블록일 때는 prp2 불필요
    
        cmd.cdw10 = (uint32_t)(req.dw1);        // SLBA 하위 32비트
        cmd.cdw11 = (uint32_t)(req.dw2.slba);   // SLBA 상위 32비트
    
    
        cmd.cdw12 = (uint32_t)((req.dw2.nlb - 1) & 0xFFFF);

        cmd.cdw13 = 0;
        cmd.cdw14 = 0;
        cmd.cdw15 = 0;
    
        nvme_prp_req_stream.write(cmd);
    }
}

void nvme_mgmt_prp(
    hls::stream<nvme_io_command_t> &nvme_prp_req_stream,
    uint64_t prp2_physical_address,
    uint64_t* prp2_virtual_address,
    hls::stream<nvme_io_command_t> &nvme_io_command_stream
){
    #pragma HLS INLINE off
    #pragma HLS PIPELINE II=1

    static int entry = 0;
    
    if(!nvme_prp_req_stream.empty()){
        nvme_io_command_t cmd = nvme_prp_req_stream.read();
        if(cmd.cdw12 == 0){
            cmd.prp2 = 0;
        }
        else if(cmd.cdw12 == 1){
            cmd.prp2 = cmd.prp1 + 0x1000;
        }
        else{
            cmd.prp2 = prp2_physical_address + (entry * 0x1000);
            // #pragma HLS PIPELINE II=1
            // for(int i = 0; i < cmd.cdw12; i++){
            for(int i = 0; i < 32; i++){
                #pragma HLS UNROLL factor=32
                *(&prp2_virtual_address[entry * 512 + i]) = cmd.prp1 + ((i+1) * 0x1000);
            }
            entry = (entry + 1) % IO_QUEUE_MAX_DEPTH;
        }
        nvme_io_command_stream.write(cmd);
    }
}

void nvme_io_sqe_dbl_write(
    uint32_t *dbl_base_address,
    // uint32_t &sq_tail,
    hls::stream<nvme_io_command_t> &nvme_io_command_stream,
    hls::stream<struct mgmt_table_req> &submit_in_req,
    hls::stream<struct mgmt_table_resp> &submit_out_resp,
    hls::stream<uint32_t> &nvme_cq_polling_stream,
    hls::stream<uint32_t> &sq_head_stream_read,
    hls::stream<nvme_io_command_t>& sq_write_stream,
    hls::stream<uint32_t>& sq_idx_stream
) 
{
    #pragma HLS INLINE off
    #pragma HLS PIPELINE II=1


    enum fsm_state {IDLE, CHECK_QUEUE_FULL, READ_SQ_HEAD, SEND_REQ, WAIT_RESP, WRITE_MEM, SEND_CQ_POLL};

    static fsm_state state = IDLE;
    #pragma HLS RESET variable=state

    static nvme_io_command_t cmd;
    static struct mgmt_table_req req;
    static struct cmd_info wdata;
    static struct mgmt_table_resp resp;
    static uint32_t sq_head = 0;
    static uint32_t sq_tail = 0;

    switch (state)
    {
        case IDLE:
            if (!nvme_io_command_stream.empty()) {
                cmd = nvme_io_command_stream.read();

                wdata.valid = true;
                wdata.opc = cmd.opc;
                wdata.cid = cmd.cid;
                wdata.qid = 0;
                wdata.slba = ((uint64_t)cmd.cdw11 << 32) | cmd.cdw10;
                wdata.nlb = cmd.cdw12;
                wdata.prp1 = cmd.prp1;

                req.op = 1;
                req.cid = cmd.cid;
                req.wdata = wdata;

                state = CHECK_QUEUE_FULL;
            }
            break;

        case CHECK_QUEUE_FULL:
            if ((sq_tail + 1) % IO_QUEUE_MAX_DEPTH == sq_head) {
                state = READ_SQ_HEAD;
            } else {
                state = SEND_REQ;
            }
            break;

        case READ_SQ_HEAD:
            if (!sq_head_stream_read.empty()) {
                sq_head = sq_head_stream_read.read();
            }
            state = CHECK_QUEUE_FULL;
            break;

        case SEND_REQ:
            if (!submit_in_req.full()) {
                submit_in_req.write(req);
                state = WAIT_RESP;
            }
            break;

        case WAIT_RESP:
            if (!submit_out_resp.empty()) {
                resp = submit_out_resp.read();
                state = WRITE_MEM;
            }
            break;

        case WRITE_MEM:
            // *(&io_sq_base_address[sq_tail]) = cmd;
            sq_write_stream.write(cmd);
            sq_idx_stream.write(sq_tail);
            sq_tail = (sq_tail + 1) % IO_QUEUE_MAX_DEPTH;
            dbl_base_address[2] = sq_tail;

            state = SEND_CQ_POLL;
            break;


        
        case SEND_CQ_POLL:
            if (!nvme_cq_polling_stream.full()) {
                nvme_cq_polling_stream.write(0);
                state = IDLE;
            }
            break;
    }
}



void sq_writer(
    nvme_io_command_t *io_sq_base_address,
    hls::stream<nvme_io_command_t>& sq_write_stream,
    hls::stream<uint32_t>& sq_idx_stream
)
{
    #pragma HLS INLINE off
    #pragma HLS PIPELINE II=1
    #pragma HLS DEPENDENCE variable=io_sq_base_address type=inter dependent=false

    static ap_uint<32> index = 0;
    if (!sq_write_stream.empty() && !sq_idx_stream.empty()) {
        nvme_io_command_t cmd = sq_write_stream.read();
        uint32_t idx = sq_idx_stream.read();
        io_sq_base_address[idx] = cmd;
    }   
}

void nvme_io_submit(
    nvme_io_command_t* io_sq_base_address,
    nvme_cqe_t *io_cq_base_address,
    uint32_t *dbl_base_address,
    uint64_t *buffer_base_address,
    // uint32_t &sq_tail,
    uint64_t prp2_physical_address,
    uint64_t* prp2_virtual_address,
    hls::stream<struct request_packet> &request_packet_stream,
    hls::stream<struct mgmt_table_req> &submit_in_req,
    hls::stream<struct mgmt_table_resp> &submit_out_resp,
    hls::stream<uint32_t> &nvme_cq_polling_stream,
    hls::stream<uint32_t> &sq_head_stream_read
)
{
    #pragma HLS INLINE
    #pragma HLS DATAFLOW disable_start_propagation

    static hls::stream<nvme_io_command_t> nvme_prp_req_stream;
    #pragma HLS STREAM variable=nvme_prp_req_stream depth=512
    static hls::stream<nvme_io_command_t> nvme_io_command_stream;
    #pragma HLS STREAM variable=nvme_io_command_stream depth=512

    static hls::stream<nvme_io_command_t> sq_write_stream;
    #pragma HLS STREAM variable=sq_write_stream depth=512
    static hls::stream<uint32_t> sq_idx_stream;
    #pragma HLS STREAM variable=sq_idx_stream depth=512

    nvme_io_cmd_gen(
        request_packet_stream,
        nvme_prp_req_stream  
    );

    nvme_mgmt_prp(
        nvme_prp_req_stream,
        prp2_physical_address,
        prp2_virtual_address,
        nvme_io_command_stream
    );

    nvme_io_sqe_dbl_write(
        dbl_base_address,
        // sq_tail,
        nvme_io_command_stream,
        submit_in_req,
        submit_out_resp,
        nvme_cq_polling_stream,
        sq_head_stream_read,
        sq_write_stream,
        sq_idx_stream
    );

    sq_writer(
        io_sq_base_address,
        sq_write_stream,
        sq_idx_stream
    );
}