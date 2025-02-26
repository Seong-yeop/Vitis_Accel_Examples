#ifndef NVME_PROCESS_CPL_H
#define NVME_PROCESS_CPL_H

#include <stdint.h>
#include <hls_stream.h>

#define DELAY_CYCLES 1000

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


typedef struct _nvme_cqe
{
    // -- DW0 --
    uint16_t command_specific_0;  ///< [15:0]   명령별/추가정보
    uint16_t reserved0;           ///< [31:16]  Reserved

    // -- DW1 --
    uint16_t command_specific_1;  ///< [15:0]   명령별/추가정보
    uint16_t sq_head_ptr;         ///< [31:16]  Submission Queue Head Pointer (SQHD)

    // -- DW2 --
    uint16_t cid;                 ///< [15:0]   Command Identifier
    uint16_t sq_id;               ///< [31:16]  Submission Queue Identifier

    // -- DW3 --
    union {
        uint32_t raw;             ///< DW3 전체를 32비트로 접근할 때
        struct {
            uint32_t dnr       : 1;  ///< bit[0]   = Do Not Retry
            uint32_t sc        : 7;  ///< bit[7:1] = Status Code
            uint32_t sct       : 3;  ///< bit[10:8]= Status Code Type
            uint32_t reserved1 : 5;  ///< bit[15:11]
            uint32_t p         : 1;  ///< bit[16]  = Phase Tag
            uint32_t reserved2 : 15; ///< bit[31:17]
        };
    } dw3;                         ///< [31:0]   DW3

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
    uint8_t used;
    uint32_t cid;
    uint16_t opc;
    uint64_t slba;
    uint32_t nlb;
    uint64_t buffer_addr;
};

// void nvme_process_cpl(
//     uint32_t *cq_base_addr, // CQ Base Address
//     uint32_t &cq_id, // CQ ID
//     uint32_t &cq_size, // CQ Depth
//     volatile uint32_t *nvme_cqh_dbl
// );  


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
    hls::stream<struct request_packet> &request_packet_stream
);

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
        uint32_t &completed_request_bytes
    );
}

#endif