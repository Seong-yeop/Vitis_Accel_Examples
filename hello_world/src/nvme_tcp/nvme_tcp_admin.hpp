#include "nvme_tcp.hpp"
#ifndef NVME_TCP_ADMIN_HPP
#define NVME_TCP_ADMIN_HPP


#define print_log(...)  hls::print(__VA_ARGS__)

// subsys_nqn : "nqn.2025-04.org.snucsl.subsys"

static const ap_uint<8> subsys_nqn_const[64] = {
    /* ASCII “nqn.2025-04.org.snucsl.subsys” (29 bytes) -------------------- */
    0x6E, 0x71, 0x6E, 0x2E, 0x32, 0x30, 0x32, 0x35,
    0x2D, 0x30, 0x34, 0x2E, 0x6F, 0x72, 0x67, 0x2E,
    0x73, 0x6E, 0x75, 0x63, 0x73, 0x6C, 0x2E, 0x73,
    0x75, 0x62, 0x73, 0x79, 0x73,
    /* terminating NUL + 35 bytes of padding -------------------------------- */
    0x00  /* the remaining elements (indices 30–63) are implicitly 0 */
};

void process_admin_queue(
    hls::stream<ap_uint<DATA_WIDTH>> &admin_queue_rxdata, // receive
    hls::stream<ap_uint<DATA_WIDTH>> &out_stream
);


enum {
	CAP	 = 0x0, 
	VS  = 0x8, 
	CC	 = 0x14, 	
    CSTS = 0x1C,
};
enum{
    FEATURE_NUMBER_OF_QUEUES = 0x07,
    FEATURE_ASYNC_EVENT_CONFIG = 0x0b,
    FEATURE_CONTROLLER_RESET = 0x20
};

struct nvme_properties {
	uint64_t cap;
	uint32_t vs;
	uint32_t cc;
	uint32_t csts;
};


enum identify_cns {
	CNS_ID_NS   = 0x0,
	CNS_ID_CTRL = 0x1,
	CNS_ID_ACTIVE_NSID = 0x2,
	CNS_ID_NS_LIST = 0x3,
};



struct nvme_lbaf {
    uint16_t ms;
    uint8_t ds;
    uint8_t rp;
};

struct nvme_id_ns {
	uint64_t			nsze;
	uint64_t			ncap;
	uint64_t			nuse;
	uint8_t			nsfeat;
	uint8_t			nlbaf;
	uint8_t			flbas;
	uint8_t			mc;
	uint8_t			dpc;
	uint8_t			dps;
	uint8_t			nmic;
	uint8_t			rescap;
	uint8_t			fpi;
	uint8_t			dlfeat;
	uint16_t			nawun;
	uint16_t			nawupf;
	uint16_t			nacwu;
	uint16_t			nabsn;
	uint16_t			nabo;
	uint16_t			nabspf;
	uint16_t			noiob;
	uint8_t			nvmcap[16];
	uint16_t			npwg;
	uint16_t			npwa;
	uint16_t			npdg;
	uint16_t			npda;
	uint16_t			nows;
	uint8_t			rsvd74[18];
	uint32_t			anagrpid;
	uint8_t			rsvd96[3];
	uint8_t			nsattr;
	uint16_t			nvmsetid;
	uint16_t			endgid;
	uint8_t			nguid[16];
	uint8_t			eui64[8];
	struct nvme_lbaf	lbaf[64];
	uint8_t			vs[3712];
}; // 4096B
#define NVME_ID_NS_LEN 4096

static const nvme_id_ns nvme_id_ns_const = {
    .nsze = 1024 * 1024 * 1024 / 4096, // 총 논리 블록 수
    .ncap = 1024 * 1024 * 1024 / 4096, // 용량은 size와 동일
    .nuse = 0, 
    .nsfeat = 0, // 추가 기능 없음
    .nlbaf = 9, // 지원하는 LBA 포맷 수 (예: 1)
    .flbas = 0, // 기본 LBA 포맷 사용
    .mc = 0, // 메타데이터 없음
    .dpc = 0,
    .dps = 0,
    .nmic = 0,
    .rescap = 0,
    .fpi = 0,
    .dlfeat = 0,
    .nawun = 0,
    .nawupf = 0,
    .nacwu = 0,
    .nabsn = 0,
    .nabo = 0,
    .nabspf = 0,
    .noiob = 0,
    .npwg = 0,
    .npwa = 0,
    .npdg = 0,
    .npda = 0,
    .nows = 0,
    .rsvd74 = {0},
    .anagrpid = 0,
    .rsvd96 = {0},
    .nsattr = 0,
    .nvmsetid = 0,
    .endgid = 0,
    .nguid = {0},
    .eui64 = {0},
    .lbaf = {
        {0, 12, 0}, // LBA 포맷 0
        {0, 0, 0}, // LBA 포맷 1
        {0}
    },
    .vs = {0} 
};

static const ap_uint<8> nvme_id_ns_const_packet[64][64] = {
    
    /* 0 : bytes 0-63 */
    {
        0x00,0x00,0x04,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x04,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x09,0x00,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00
    },

    /* 1 : bytes 64-127 (all zero) */   {0},

    /* 2 : bytes 128-191 */
    {   0x00,0x00,0x0C,0x00   },

    /* 3-63 : bytes 192-4095 (all zero) */
};

static inline ap_uint<512> ap_uint_8_to_512( const ap_uint<8> src [64]){
    ap_uint<512> word = 0;
#pragma HLS INLINE 
        for (int i = 0; i < 64; ++i) {
#pragma HLS UNROLL
            word(i * 8 + 7, i * 8) = src[i];
        }
        return word;
}

struct nvme_connect_params {
	char hostid[16];
	uint16_t  cntlid;
	uint8_t   resvd1[238];
	char subnqn[256];
	char hostnqn[256];
	uint8_t   resvd2[256];
}; // 1024B
#define NVME_CONNPARAMS_LEN 1024




// struct nvme_id_ctrl {

// 	uint16_t  vid; // 01:00
// 	uint16_t  ssvid; // 03:02
// 	char sn[20]; // 23:04
// 	char mn[40]; // 63:24
// 	char fr[8]; // 71:64
// 	uint8_t   rab; // 72:72
// 	uint8_t   ieee[3]; // 75:73
// 	uint8_t   cmic; // 76
// 	uint8_t   mdts; // 77
// 	uint16_t  cntlid; // 79:78
// 	uint32_t  ver; // 83:80
// 	uint32_t  rtd3r; // 87:84
// 	uint32_t  rtd3e; // 91:88
// 	uint32_t  oaes; //  95:92
// 	uint32_t  ctratt; // 99:96
// 	uint16_t  rrls; // 101:100
// 	uint8_t   revsd1[9]; // 110:102
// 	uint8_t   cntrltype;
// 	char fguid[16];
// 	uint16_t  crdt1;
// 	uint16_t  crdt2;
// 	uint16_t  crdt3;
// 	uint8_t   resvd2[122];
// 	uint16_t  oacs;
// 	uint8_t   acl;
// 	uint8_t   aerl;
// 	uint8_t   frmw;
// 	uint8_t   apl;
// 	uint8_t   elpe;
// 	uint8_t   npss;
// 	uint8_t   avscc;
// 	uint8_t   apsta;
// 	uint16_t  wctemp;
// 	uint16_t  cctemp;
// 	uint16_t  mtfa;
// 	uint32_t  hmpre;
// 	uint32_t  hmmin;
// 	uint64_t  tnvmcap;
// 	uint64_t  tnvmcap_hi;
// 	uint64_t  unvmcap;
// 	uint64_t  unvmcap_hi;
// 	uint32_t  rpmbs;
// 	uint16_t  edstt;
// 	uint8_t   dsto;
// 	uint8_t   fwug;
// 	uint16_t  kas;
// 	uint16_t  hctma;
// 	uint16_t  mntmt;
// 	uint16_t  mxtmt;
// 	uint32_t  sanicap;
// 	uint32_t  hmminds;
// 	uint16_t  hmmaxd;
// 	uint16_t  nsetidmax;
// 	uint16_t  endgidmax;
// 	uint8_t   anatt;
// 	uint8_t   anacap;
// 	uint32_t  anagrpmax;
// 	uint32_t  nanagrpid;
// 	uint32_t  pels;
// 	uint8_t   resvd3[156];
// 	uint8_t   sqes;
// 	uint8_t   cqes;
// 	uint16_t  maxcmd;
// 	uint32_t  nn;
// 	uint16_t  oncs;
// 	uint16_t  fuses;
// 	uint8_t   fna;
// 	uint8_t   vwc;
// 	uint16_t  awun;
// 	uint16_t  awupf;
// 	uint8_t   nvscc;
// 	uint8_t   nwpc;
// 	uint16_t  acwu;
// 	uint8_t   resvd4[2];
// 	uint32_t  sgls;
// 	uint32_t  mnan;
// 	uint8_t   resvd5[224];
// 	char subnqn[256]; //768:1023, SUBNQN
// 	uint8_t   resvd6[768];
// 	uint32_t  ioccsz;
// 	uint32_t  iorcsz;
// 	uint16_t  icdoff;
// 	uint8_t   ctrattr; //
// 	uint8_t   msdbd; // 1803
// 	uint8_t   resvd7[244]; // 2047:1804 ( Fabrics)
// 	uint8_t   psd[1024]; // 3071:2048 
// 	uint8_t   vs[1024]; // 4095:3072
// }; // 4096B
// #define NVME_ID_CTRL_LEN 4096
// static const struct nvme_id_ctrl nvme_id_ctrl_const = {
//     /* 00h */ .sn      = "SN1234567890",          /* Serial Number (20 B)  */
//     /* 18h */ .mn      = "CSL NVMe-TCP Model",    /* Model Number  (40 B)  */
//     /* 40h */ .fr      = "0.0.1",                 /* FW Revision   (8 B)   */
//     /* 4Dh */ .mdts    = 11,                      /* 1 << 11  = 2048 B     */
//     /* 4Eh */ .cntlid  = 1,
//     /* 50h */ .ver     = 0x010400,                /* NVMe 1.4 (major.minor)*/
//     /* 140h*/ .kas     = 0x1111,                  /* Keep-Alive 100 ms × 0x1111 */
//     /* 200h*/ .sqes    = 0x66,                    /* SQE min = 6, max = 6  */
//     /* 201h*/ .cqes    = 0x44,                    /* CQE min = 4, max = 4  */
//     /* 202h*/ .maxcmd  = 128,
//     /* 204h*/ .nn      = 1,                       /* 한 개의 네임스페이스 */
//     /* 218h*/ .sgls    = 1,                       /* SGL 0 : NVM Command Set */
//     /* 300h*/ .subnqn  = "nqn.2025-04.org.snucsl.subsys",    /* 256 B Subsys NQN      */
//     /* 나머지 필드는 사양 기본값(0) */
// };

static const ap_uint<8> nvme_id_ctrl_const_packet[64][64] = {
    /* [0] bytes 0x000-0x03F :   SN + MN (model-number) section ------------- */
    {
        0x00, 0x00, 0x00, 0x00, 0x53, 0x4E, 0x31, 0x32,
        0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x43, 0x53, 0x4C, 0x20, 0x4E, 0x56, 0x4D, 0x65,
        0x2D, 0x54, 0x43, 0x50, 0x20, 0x4D, 0x6F, 0x64,
        0x65, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },

    /* [1] bytes 0x040-0x07F :   FR, MDTS, CNTLID, VER … -------------------- */
    {
        0x30, 0x2E, 0x30, 0x2E, 0x31, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x01, 0x00,
        0x00, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },

    /* [2]-[4] : all-zero rows --------------------------------------------- */
    {0}, {0}, {0},

    /* [5] bytes 0x180-0x1BF :   KAS = 0x1111 -------------------------------- */
    {
        0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },

    /* [6]-[7] : all-zero rows --------------------------------------------- */
    {0}, {0},

    /* [8] bytes 0x240-0x27F :   SQES, CQES, MAXCMD, NN, SGLS --------------- */
    {
        0x66, 0x44, 0x80, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },

    /* [9]-[11] : all-zero rows -------------------------------------------- */
    {0}, {0}, {0},

    /* [12] bytes 0x300-0x33F :   subnqn = "nqn.2025-04.org.snucsl.subsys" --- */
    {
        0x6E, 0x71, 0x6E, 0x2E, 0x32, 0x30, 0x32, 0x35,
        0x2D, 0x30, 0x34, 0x2E, 0x6F, 0x72, 0x67, 0x2E,
        0x73, 0x6E, 0x75, 0x63, 0x73, 0x6C, 0x2E, 0x73,
        0x75, 0x62, 0x73, 0x79, 0x73, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    },

    /* [13]-[63] : (omitted) – automatically zero-initialised --------------- */
};

// struct id_active_ns_list {
//     unsigned int cns[1024];
// }; // 4096B
// #define NVME_ID_ACTIVE_NS_LEN 4096

// static const struct id_active_ns_list identify_active_ns_list_const = {
//     .cns = {0x1} // NSID=1
// };

static const ap_uint<8> nvme_id_active_nsid_const_packet[64][64] = {
    /* [0] bytes 0x000-0x03F : cns[0] = 1 */
    /* [1]-[63] 모두 0 */
    0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    /// ... 0
};


static const ap_uint<8> id_ns_desc_const_packet[20] = {
    0x03, 0x10, 0x00, 0x00,
    0x6E, 0x71, 0x6E, 0x2E,
    0x32, 0x30, 0x32, 0x35,
    0x2D, 0x30, 0x34, 0x2E,
    0x6F, 0x72, 0x67, 0x2E
};

static inline ap_uint<160> ap_uint_8_to_160( const ap_uint<8> src [20]){
    ap_uint<512> word = 0;
#pragma HLS INLINE 
        for (int i = 0; i < 20; ++i) {
#pragma HLS UNROLL
            word(i * 8 + 7, i * 8) = src[i];
        }
        return word;
}
    
#endif