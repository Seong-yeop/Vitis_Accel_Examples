#include <hls_stream.h>
#include <ap_int.h>
#include <stdint.h>
#include <string.h>

#ifndef NVME_TCP_HPP
#define NVME_TCP_HPP

#define DATA_WIDTH 512




/*
 * PDU types
 */
enum {
	PDU_TYPE_ICREQ	 = 0, // IcReq
	PDU_TYPE_ICRESP  = 1, // ICResp
	PDU_TYPE_CMD	 = 4, // CapsuleCmd
	PDU_TYPE_RESP	 = 5, // CapsuleResp
    PDU_TYPE_H2CDATA = 6, // H2CData
	PDU_TYPE_C2HDATA = 7, // C2HData
};

/*
 * PDU common header
 */
struct pdu_header {
	uint8_t  type;
	uint8_t  flags;
	uint8_t  hlen;
	uint8_t  pdo;
	uint32_t plen;
};
#define PDU_CH_LEN 8

static inline ap_uint<64> pdu_header_to_ap_uint(const struct pdu_header& hdr) {
    ap_uint<64> raw;
    raw.range(7, 0)   = hdr.type;
    raw.range(15, 8)  = hdr.flags;
    raw.range(23, 16) = hdr.hlen;
    raw.range(31, 24) = hdr.pdo;
    raw.range(63, 32) = hdr.plen;
    return raw;
}

#define PDU_HDR_LEN 8 // sizeof(struct pdu_header)

struct nvme_status {
	uint32_t dw0;
	uint32_t dw1;
	uint16_t sqhd;
	uint16_t sqid;
	uint16_t cid;
	uint16_t sf;

};
#define NVME_STATUS_LEN 16

static inline ap_uint<128> nvme_status_to_ap_uint(const struct nvme_status& status) {
    ap_uint<128> raw;
    raw.range(7, 0)   = status.dw0;
    raw.range(15, 8)  = status.dw1;
    raw.range(23, 16) = status.sqhd;
    raw.range(31, 24) = status.sqid;
    raw.range(63, 32) = status.cid;
    raw.range(127, 64) = status.sf;
    return raw;
}


enum nvme_sc_type {
	SCT_GENERIC  = 0,
	SCT_CMD_SPEC = 1,
};

enum nvme_sc {
	SC_SUCCESS         = 0x0,
	SC_INVALID_OPCODE  = 0x1,
	SC_INVALID_FIELD   = 0x2,
	SC_COMMAND_SEQ     = 0xC,
	SC_CONNECT_INVALID = 0x82,
};

enum {
    NO_CONNECTION = 0,
    PDU_LEVEL_CONNECTION = 1,
    DISCOVERY_CONNECTION = 2,
    ADMIN_CONNECTION = 3,
    IO_CONNECTION = 4


};

struct icresp_t {
    struct pdu_header pdu_hdr;
    uint32_t psh[30]; // PSH, 120B, reserved

};
#define ICRESP_LEN 128

static inline ap_uint<128> icresp_to_ap_uint(const struct icresp_t& resp) {
    ap_uint<128> raw;
    raw.range(7, 0)   = resp.pdu_hdr.type;
    raw.range(15, 8)  = resp.pdu_hdr.flags;
    raw.range(23, 16) = resp.pdu_hdr.hlen;
    raw.range(31, 24) = resp.pdu_hdr.pdo;
    raw.range(63, 32) = resp.pdu_hdr.plen;
    raw.range(127, 64) = 0;
    return raw;
}

const struct icresp_t icresp_format = {
    .pdu_hdr = {
        .type = PDU_TYPE_ICRESP,
        .flags = 0,
        .hlen = 128,
        .pdo = 0,
        .plen = 128,
    },
    .psh = {0}
};

struct cmd_resp_t{
    struct pdu_header pdu_hdr; // 8
    struct nvme_status psh;  // 16
    
};
#define CMD_RESP_LEN 24

static inline ap_uint<192> cmd_resp_to_ap_uint(const struct cmd_resp_t& resp) {
    ap_uint<192> raw;
    raw = 0;
    raw.range(7, 0)   = resp.pdu_hdr.type;
    raw.range(15, 8)  = resp.pdu_hdr.flags;
    raw.range(23, 16) = resp.pdu_hdr.hlen;
    raw.range(31, 24) = resp.pdu_hdr.pdo;
    raw.range(63, 32) = resp.pdu_hdr.plen;
    raw.range(95, 64) = resp.psh.dw0;
    raw.range(127, 96) = resp.psh.dw1;
    raw.range(143, 128) = resp.psh.sqhd;
    raw.range(159, 144) = resp.psh.sqid;
    raw.range(175, 160) = resp.psh.cid;
    raw.range(191, 176) = resp.psh.sf;
    return raw;
}

const struct cmd_resp_t cmd_resp_format = {
    .pdu_hdr = {
        .type = PDU_TYPE_RESP,
        .flags = 0,
        .hlen = CMD_RESP_LEN,
        .pdo = 0,
        .plen = CMD_RESP_LEN,
    },
    .psh = {
        .dw0  = 1, 
        .dw1  = 0,
        .sqhd = 1,
        .sqid = 0,
        .cid  = 0, // TODO: INITIALIZE CID in function
        .sf   = 0,
    },
};

struct nvme_sgl_desc {
	uint64_t address;
	uint32_t length;
	uint8_t  resvd[3];
	uint8_t  sglid;

  
};

struct nvme_fabrics_cmd {
	uint8_t  opcode; // 0
	uint8_t  flags; // 1
	uint16_t cid; // 2-3
    uint8_t fctype; // 4
	uint8_t rsvd0[19]; //5-23
	struct nvme_sgl_desc sgl; // 24 - 39
	uint32_t cdw10; // 40 - 43
	uint32_t cdw11; // 44 - 47
	uint32_t cdw12; // 48 - 51
	uint32_t cdw13; // 52 - 55
	uint32_t cdw14; // 56 - 59
	uint32_t cdw15; // 60 - 63
};
#define NVME_CMD_LEN 64

static inline struct nvme_fabrics_cmd ap_uint_to_nvme_cmd(const ap_uint<512>& raw){
    struct nvme_fabrics_cmd cmd;
    cmd.opcode = raw.range(7,0);
    cmd.flags  = raw.range(15,8);
    cmd.cid    = raw.range(31,16);
    cmd.fctype   = raw.range(39,32);
    for(int i = 0; i < 19; ++i) {
        #pragma HLS UNROLL
        cmd.rsvd0[i] = raw.range(47+i*8, 40+i*8);
    }
    cmd.sgl.address = raw.range(255,192);
    cmd.sgl.length  = raw.range(287,256);
    cmd.sgl.sglid   = raw.range(295,288);
    cmd.cdw10       = raw.range(351,320); 
    cmd.cdw11       = raw.range(383,352);  
    cmd.cdw12       = raw.range(415,384);
    cmd.cdw13       = raw.range(447,416);
    cmd.cdw14       = raw.range(479,448);
    cmd.cdw15       = raw.range(511,480);
    return cmd;
}





struct packet_header {
    pdu_header pdu_hdr;     //  0.. 63  (5×8‑bit fields)
    uint32_t session_id; // 64.. 95
};

static inline struct packet_header ap_uint_to_packet_header(const ap_uint<512>& raw)
{
    struct packet_header hdr;
    hdr.pdu_hdr.type  =  raw.range(7,0);
    hdr.pdu_hdr.flags =  raw.range(15,8);
    hdr.pdu_hdr.hlen  =  raw.range(23,16);
    hdr.pdu_hdr.pdo   =  raw.range(31,24);
    hdr.pdu_hdr.plen  =  raw.range(63,32);
    hdr.session_id    =  raw.range(95,64);
    return hdr;
}

enum nvme_opcodes {
	OPC_GET_LOG  = 0x2,
	OPC_IDENTIFY = 0x6,
	OPC_SET_FEATURES = 0x9,
	OPC_FABRICS  = 0x7f,
	OPC_KEEP_ALIVE = 0x18,
};


enum fabrics_commands {
	FCTYPE_SET_PROP = 0x0,
	FCTYPE_CONNECT	= 0x1,
	FCTYPE_GET_PROP = 0x4,
};
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

struct c2hdata_psh{
    uint16_t cid; // 9:8
    uint16_t rsvd0; // 11:10
    uint32_t DATAO; // Data Offset
    uint32_t DATAL; // Data Length
    uint32_t rsvd1; // 23:20
};
#define C2HDATA_PSH_LEN 16

struct c2hdata_hdr {
    struct pdu_header pdu_hdr;
    struct c2hdata_psh psh;
};
#define C2HDATA_HDR_LEN 24

const struct c2hdata_hdr c2hdata_hdr_1024B_const{
    .pdu_hdr = {
        .type = PDU_TYPE_C2HDATA,
        .flags = 0,
        .hlen = 0x18, // 24Byte
        .pdo = 0x18,
        .plen = 0x18 + 1024, // 1048Byte
    },
    .psh = {
        .cid = 0,
        .rsvd0 = 0,
        .DATAO = 0,
        .DATAL = 1024,
        .rsvd1 = 0
    }
};

const struct c2hdata_hdr c2hdata_hdr_4096B_const{
    .pdu_hdr = {
        .type = PDU_TYPE_C2HDATA,
        .flags = 0,
        .hlen = 0x18, // 24Byte
        .pdo = 0x18,
        .plen = 0x18 + 4096, // 4120Byte
    },
    .psh = {
        .cid = 0,
        .rsvd0 = 0,
        .DATAO = 0,
        .DATAL = 4096,
        .rsvd1 = 0
    }
}; 

const struct c2hdata_hdr c2hdata_hdr_20B_const{
    .pdu_hdr = {
        .type = PDU_TYPE_C2HDATA,
        .flags = 0,
        .hlen = 0x18, // 24Byte
        .pdo = 0x18,
        .plen = 0x18 + 20, // 48Byte
    },
    .psh = {
        .cid = 0,
        .rsvd0 = 0,
        .DATAO = 0,
        .DATAL = 20,
        .rsvd1 = 0
    }
}; 

static inline ap_uint<224> c2hdata_hdr_to_ap_uint(const struct c2hdata_hdr& hdr) {
    ap_uint<224> raw;
    raw.range(7, 0)   = hdr.pdu_hdr.type;
    raw.range(15, 8)  = hdr.pdu_hdr.flags;
    raw.range(23, 16) = hdr.pdu_hdr.hlen;
    raw.range(31, 24) = hdr.pdu_hdr.pdo;
    raw.range(63, 32) = hdr.pdu_hdr.plen;
    raw.range(95, 64) = hdr.psh.cid;
    raw.range(127, 96) = hdr.psh.rsvd0;
    raw.range(159, 128) = hdr.psh.DATAO;
    raw.range(191, 160) = hdr.psh.DATAL;
    raw.range(223, 192) = hdr.psh.rsvd1;
    return raw;
}

#endif
