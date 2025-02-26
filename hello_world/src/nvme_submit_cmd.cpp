#include <ap_int.h>
#include <hls_stream.h>
#include <stdint.h>
#include <string.h>

#include "nvme_driver_top.hpp"


void nvme_io_cmd_gen(
    hls::stream<struct request_packet> &request_packet_stream,
    hls::stream<nvme_io_command_t> &nvme_io_command_stream
)
{
    #pragma HLS INLINE off
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
    
        nvme_io_command_stream.write(cmd);
    }
}

void nvme_io_sqe_dbl_write(
    nvme_io_command_t *io_sq_base_address,
    uint32_t *dbl_base_address,
    uint32_t &sq_tail,
    hls::stream<nvme_io_command_t> &nvme_io_command_stream
) 
{
    #pragma HLS INLINE off
    if (!nvme_io_command_stream.empty()) {
        nvme_io_command_t cmd = nvme_io_command_stream.read();
        // io_sq_base_address[sq_tail] = cmd;
        *(&io_sq_base_address[sq_tail])  = cmd;
        sq_tail = (sq_tail + 1) % IO_QUEUE_MAX_DEPTH;
        dbl_base_address[2] = sq_tail;
    }

}

void nvme_io_submit(
    nvme_io_command_t* io_sq_base_address,
    nvme_cqe_t *io_cq_base_address,
    uint32_t *dbl_base_address,
    uint64_t *buffer_base_address,
    uint32_t &sq_tail,
    hls::stream<struct request_packet> &request_packet_stream
)
{
    #pragma HLS DATAFLOW
    
    static hls::stream<nvme_io_command_t> nvme_io_command_stream;

    nvme_io_cmd_gen(
        request_packet_stream,
        nvme_io_command_stream  
    );

    nvme_io_sqe_dbl_write(
        io_sq_base_address,
        dbl_base_address,
        sq_tail,
        nvme_io_command_stream
    );
}