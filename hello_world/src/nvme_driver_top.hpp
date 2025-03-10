#ifndef NVME_PROCESS_CPL_H
#define NVME_PROCESS_CPL_H

#include <hls_stream.h>
#include <ap_int.h>
#include <stdint.h>


// NVMe I/O Opcode
#define NVME_OPC_WRITE  0x01
#define NVME_OPC_READ   0x02

// 큐 엔트리 하나의 크기(64B = 16DW)
#define NVME_CMD_DWORDS 16

// 큐의 최대 깊이(예시)
#define IO_QUEUE_MAX_DEPTH  128
#define IO_QUEUE_MAX_NUM 4

/// NVMe I/O command opcode
#define NVME_IO_WRITE 0x01
#define NVME_IO_READ  0x02

#define MAX_CMD_INFO_TBL_SIZE 256

// NVMe I/O command
typedef struct _nvme_io_command
{
    // -- DW0 --
    uint8_t  opc;         ///< opcode (0x01=Write, 0x02=Read, etc.)
    uint8_t  fuse : 2;    ///< fuse
    uint8_t  rsvd : 6;    ///< reserved
    uint16_t cid;         ///< Command ID

    // -- DW1 --
    uint32_t nsid;        ///< Namespace ID

    // -- DW2-DW3 --
    uint32_t cdw2;        ///< reserved or optional
    uint32_t cdw3;        ///< reserved or optional

    // -- DW4-DW5 (MPTR) --
    uint64_t mptr;        ///< metadata pointer

    // -- DW6-DW7 (PRP1) --
    uint64_t prp1;        ///< PRP entry 1

    // -- DW8-DW9 (PRP2) --
    uint64_t prp2;        ///< PRP entry 2

    // -- DW10 --
    //  [31:16] = NLB (Number of Logical Blocks) - 1
    //  [15:0]  = starting LBA's upper bits or other fields
    uint32_t cdw10;

    // -- DW11 --
    //  보통 I/O 명령에서 LBA 상위비트, 예약 필드, force unit access 등
    uint32_t cdw11;

    // -- DW12 --
    //  [31:0] = Lower 32 bits of SLBA or additional parameters
    uint32_t cdw12;

    // -- DW13 --
    //  [31:0] = Upper 32 bits of SLBA (if 64-bit LBA)
    uint32_t cdw13;

    // -- DW14 --
    uint32_t cdw14;

    // -- DW15 --
    uint32_t cdw15;

} nvme_io_command_t;


typedef struct _nvme_cqe_t
{
    // -- DW0 --
    uint32_t dw0; // Command Specific

    // -- DW1 --
    uint32_t dw1; // Reserved 

    // -- DW2 --
    uint16_t sq_head;                 
    uint16_t sq_identifier;               

    // -- DW3 --
    uint16_t cid;       // Command Identifier (CID)
    uint32_t phase_tag  :1;     // Phase Tag (P)
    uint32_t status_code:8;     // Status Code
    uint32_t sct        :3;     // Status Code Type (SCT)
    uint32_t reserved   :4;     // Reserved

} nvme_cqe_t;

struct request_packet {
    union {
        struct {
            uint16_t id;         ///< command ID
            uint32_t dev : 4;    ///< device
            uint32_t sqid : 4;   ///< SQ ID
            uint32_t flags : 4;  ///< flags
            uint32_t opc : 4;     ///< opcode (0x01=Write, 0x02=Read, etc.)
        };
        uint32_t raw;
    } dw0;

    uint32_t dw1; /// slba[31:0]
    
    union {
        struct {
            uint32_t nlb : 16;    ///< Number of Logical Blocks
            uint32_t slba : 16;   /// slba[47:32]
        };
        uint32_t raw;
    } dw2;

    uint64_t dw3; /// buffer address
};

struct completion_packet {
    union {
        struct {
            uint16_t id;         ///< command ID
            uint32_t reserved1 : 4;  
            uint32_t sct : 4;  ///< Status Code Type
            uint32_t sc : 8;     ///< Status Code
        };
        uint32_t raw;
    } dw0;
};

struct cmd_info {
    ap_uint<1> valid;
    ap_uint<16> qid;
    ap_uint<16> cid;
    ap_uint<8> opc;
    ap_uint<64> slba;
    ap_uint<32> nlb;
    ap_uint<64> prp1;
};

struct mgmt_table_req {
    ap_uint<4>   op;    // op: 0=read, 1=write, 2=clear
    ap_uint<16> cid;  // CID 
    struct cmd_info   wdata; // write data  
};

struct mgmt_table_resp {
    struct cmd_info rdata;   
    ap_uint<1> status;
};

void mgmt_table(
    hls::stream<mgmt_table_req>  &submit_in_req
    // hls::stream<mgmt_table_resp> &submit_out_resp
    // hls::stream<mgmt_table_req>  &cpl_in_req,
    // hls::stream<mgmt_table_resp> &cpl_out_resp
);



extern "C" {
void packet_generator(
    uint32_t opcode,
    uint64_t slba,
    uint32_t nlb,
    uint64_t prp1,
    uint32_t request_count,
    hls::stream<struct request_packet> &request_packet_stream
);
}

void nvme_io_submit(
    nvme_io_command_t* io_sq_base_address,
    nvme_cqe_t *io_cq_base_address,
    uint32_t *dbl_base_address,
    uint64_t *buffer_base_address,
    uint32_t &sq_tail,
    uint64_t prp2_physical_address,
    uint64_t* prp2_virtual_address,
    hls::stream<struct request_packet> &request_packet_stream,
    hls::stream<struct mgmt_table_req> &submit_in_req,
    hls::stream<struct mgmt_table_resp> &submit_out_resp,
    hls::stream<uint32_t> &nvme_cq_polling_stream,
    hls::stream<uint32_t> &sq_head_stream_read
);


void nvme_process_cpl(
    nvme_cqe_t *io_cq_base_addr,
    uint32_t *dbl_base_address,
    uint32_t &completed_requests_count,  
    uint32_t &completed_requests_bytes,
    hls::stream<struct mgmt_table_req> &cpl_in_req,
    hls::stream<struct mgmt_table_resp> &cpl_out_resp,
    hls::stream<uint32_t> &nvme_cq_polling_stream,
    hls::stream<uint32_t> &sq_head_stream_write
); 

void mgmt_table(
    hls::stream<struct mgmt_table_req>  &submit_in_req,
    hls::stream<struct mgmt_table_resp> &submit_out_resp,
    hls::stream<struct mgmt_table_req>  &cpl_in_req,
    hls::stream<struct mgmt_table_resp> &cpl_out_resp,
    hls::stream<uint32_t> &sq_head_stream_write,
    hls::stream<uint32_t> &sq_head_stream_read
);

extern "C" {
    void nvme_driver_top(
        uint32_t &start,
        uint32_t &done,
        uint32_t &done_ack,

        nvme_cqe_t *io_cq_base_addr, // CQ base address
        nvme_io_command_t *io_sq_base_addr, // SQ base address
        uint32_t *dbl_base_address, // Doorbell base address
        uint32_t *dbl_base_address2, // Doorbell base address
        uint64_t *buffer_base_address,

        hls::stream<struct request_packet> &request_packet_stream,
        /* NVMe Controller Completion Interface */
        uint32_t &completed_request_number,
        uint32_t &completed_request_bytes,
        uint64_t prp2_physical_address,
        uint64_t* prp2_virtual_address
    );
}

#endif
