#include "nvme_tcp.hpp"
#include "nvme_tcp_admin.hpp"

#ifndef NVME_TCP_DRIVER_TOP_HPP
#define NVME_TCP_DRIVER_TOP_HPP
//#define DISCOVERY_NQN "nqn.2014-08.org.nvmexpress.discovery"
//#define SUBSYS_NQN "nqn.2025-04.org.snucsl.subsys"


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


typedef struct _nvme_sqe_t
{
    // common
    uint8_t  opc;         ///< opcode (0x01=Write, 0x02=Read, etc.)
    uint8_t  fuse : 2;    ///< fuse
    uint8_t  rsvd : 6;    ///< reserved
    uint16_t cid;         ///< Command ID
    uint32_t nsid;        ///< Namespace ID
    uint32_t cdw2;        ///< reserved or optional
    uint32_t cdw3;        ///< reserved or optional
    uint64_t mptr;        ///< metadata pointer
    uint64_t prp1;        ///< PRP entry 1
    uint64_t prp2;        ///< PRP entry 2
    uint32_t cdw10;

    // rsvd
    uint32_t cdw11;
    uint32_t cdw12;
    uint32_t cdw13;
    uint32_t cdw14;
    uint32_t cdw15;

} nvme_sqe_t;






/*
 * Initiator Status
 */
#define MAX_SESSION_ID 16

extern "C"{
void nvme_tcp_driver_top(
    // from network stream
    hls::stream<ap_uint<DATA_WIDTH>> &nvme_tcp_rxdata, // receive
    hls::stream<ap_uint<DATA_WIDTH>> &nvme_tcp_txdata, // transmit

    // for nvme driver
    hls::stream<struct request_packet> &nvme_request_stream,
    hls::stream<struct completion_packet> &nvme_response_stream
);
}

#endif



