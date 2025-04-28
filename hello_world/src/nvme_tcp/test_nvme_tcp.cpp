#include <stdio.h>
#include "nvme_tcp_driver_top.hpp"
#include <hls_stream.h>
#include <ap_int.h>

#define DATA_WIDTH 512
#define BUF_SIZE   4096


static inline ap_uint<DATA_WIDTH>
make_capsule_header(ap_uint<8>  TYPE,
                    ap_uint<8>  FLAGS,
                    ap_uint<8>  HLEN,
                    ap_uint<8>  PDO,
                    ap_uint<32> PLEN)
{
    ap_uint<DATA_WIDTH> w = 0;
    w.range(  7,  0) = TYPE;
    w.range( 15,  8) = FLAGS;
    w.range( 23, 16) = HLEN;
    w.range( 31, 24) = PDO;
    w.range( 63, 32) = PLEN;
    return w;                 // 나머지는 0
}

// ---------------------------------------------------------------------------
// Make Connection -----------------------------------------------------------
// ---------------------------------------------------------------------------

void sendICReq(hls::stream<ap_uint<DATA_WIDTH>>& rx, uint16_t cid)
{
    // CH
    rx.write(make_capsule_header(/*TYPE*/ 0x00, /*FLAGS*/ 0x00,
                                 /*HLEN*/ 128,  /*PDO*/   0,
                                 /*PLEN*/ 128));
    // PSH (내용 없음)
    rx.write(0);
}

void sendConnect(hls::stream<ap_uint<DATA_WIDTH>>& rx, uint16_t cid)
{
    // CH – NVMe‑oF Connect
    rx.write(make_capsule_header(/*TYPE*/ 0x04, /*FLAGS*/ 0x40,
                                 /*HLEN*/ 72,   /*PDO*/  72,
                                 /*PLEN*/ 1096));

    // PSH – Connect SQE ------------------------------------------------------
    {
        ap_uint<DATA_WIDTH> w = 0;
        const ap_uint<8>  OPCODE   = 0x7F;
        const ap_uint<8>  SQ_FLAGS = 0x40;  // rsvd bit 6 = 1
        const ap_uint<16> CID      = cid;
        const ap_uint<8>  FCTYPE   = 0x01;
        
        ap_uint<64> SGL_ADDR    = 0x0;
        ap_uint<32> SGL_LENGTH  = 1024;
        ap_uint<8>  SGL_SUBTYPE = 0x00;
        ap_uint<8>  SGL_TYPE    = 0x00;
        ap_uint<16> SGL_RSVD    = 0x0000;

        const ap_uint<16> RECFMT = 0;
        const ap_uint<16> QID    = 0;
        const ap_uint<16> SQSIZE = 31;
        const ap_uint<8>  CATTR  = 0;
        const ap_uint<32> KATO   = 15000;

        w.range(   7,   0) = OPCODE;
        w.range(  15,   8) = SQ_FLAGS;
        w.range(  31,  16) = CID;
        w.range(  39,  32) = FCTYPE;
        w.range( 255, 192) = SGL_ADDR;
        w.range( 287, 256) = SGL_LENGTH;
        w.range( 295, 288) = SGL_SUBTYPE;
        w.range( 303, 296) = SGL_TYPE;
        w.range( 319, 304) = SGL_RSVD;
        w.range( 335, 320) = RECFMT;
        w.range( 351, 336) = QID;
        w.range( 367, 352) = SQSIZE;
        w.range( 375, 368) = CATTR;
        w.range( 415, 384) = KATO;

        rx.write(w);
    }

    // DATA ------------------------------------------------------------------
    {
        ap_uint<DATA_WIDTH> blk[16] = {0};

        // Host ID (0‑15)
        const char hostid[] = "ce2}=DO`.$"; // 10 bytes
        for (int i = 0; i < sizeof(hostid) - 1; ++i) {
            blk[0].range(i*8+7, i*8) = (ap_uint<8>)hostid[i];
        }
        // CNTLID (16‑17) = 0x0001
        blk[0].range( (16*8)+15, (16*8) ) = (ap_uint<16>)0x0001;

        // SUBNQN (256‑511)
        const char subnqn[] = "nqn.2025-04.org.snucsl.subsys";
        for (size_t i = 0; i < sizeof(subnqn)-1; ++i) {
            int byte_off = 256 + i;
            int idx      = byte_off / 64;          // 64 bytes per 512‑bit word
            int bit_off  = (byte_off % 64) * 8;
            blk[idx].range(bit_off+7, bit_off) = (ap_uint<8>)subnqn[i];
        }

        // HOSTNQN (512‑777)
        const char hostnqn[] =
            "nqn.2014-08.org.nvmexpress:uuid:544d62cd-a4d0-433f-bed9-c56e1cd62654";
        for (size_t i = 0; i < sizeof(hostnqn)-1; ++i) {
            int byte_off = 512 + i;
            int idx      = byte_off / 64;
            int bit_off  = (byte_off % 64) * 8;
            blk[idx].range(bit_off+7, bit_off) = (ap_uint<8>)hostnqn[i];
        }

        for (int i = 0; i < 16; ++i) rx.write(blk[i]);
    }
}

// ---------------------------------------------------------------------------
// Property Get/Set -----------------------------------------------------------
// ---------------------------------------------------------------------------

void sendGetCAP(hls::stream<ap_uint<DATA_WIDTH>>& rx, uint16_t cid)
{
    rx.write(make_capsule_header(0x04, 0x00, 72, 0, 72));

    ap_uint<DATA_WIDTH> w = 0;
    w.range(  7,  0) = 0x7F;      // OPCODE
    w.range( 15,  8) = 0x40;      // FLAGS
    w.range( 31, 16) = cid;        // CID
    w.range( 39, 32) = 4;         // FCTYPE
    w.range(327,320) = 0x01;      // ATTRIB
    w.range(383,352) = 0x0;       // OFFSET
    rx.write(w);
}

void sendSetCC(hls::stream<ap_uint<DATA_WIDTH>>& rx, uint16_t cid)
{
    rx.write(make_capsule_header(0x04, 0x00, 72, 0, 72));

    ap_uint<DATA_WIDTH> w = 0;
    w.range(  7,  0)  = 0x7F;       // OPCODE
    w.range( 15,  8)  = 0x40;       // FLAGS
    w.range( 31, 16)  = cid;         // CID
    w.range( 39, 32)  = 0;          // FCTYPE
    w.range(327,320)  = 0;          // ATTRIB
    w.range(383,352)  = 0x14;       // OFFSET
    w.range(447,384)  = (ap_uint<64>)0x460001; // VALUE
    rx.write(w);
}

void sendGetCSTS(hls::stream<ap_uint<DATA_WIDTH>>& rx, uint16_t cid)
{
    rx.write(make_capsule_header(0x04, 0x00, 72, 0, 72));

    ap_uint<DATA_WIDTH> w = 0;
    w.range(  7,  0) = 0x7F;
    w.range( 15,  8) = 0x40;
    w.range( 31, 16) = cid;        // CID
    w.range( 39, 32) = 4;         // FCTYPE
    w.range(327,320) = 0x00;      // ATTRIB
    w.range(383,352) = 0x1c;      // OFFSET
    rx.write(w);
}

void sendGetVer(hls::stream<ap_uint<DATA_WIDTH>>& rx, uint16_t cid)
{
    rx.write(make_capsule_header(0x04, 0x00, 72, 0, 72));

    ap_uint<DATA_WIDTH> w = 0;
    w.range(  7,  0) = 0x7F;
    w.range( 15,  8) = 0x40;
    w.range( 31, 16) = cid;        // CID
    w.range( 39, 32) = 4;         // FCTYPE
    w.range(327,320) = 0x00;      // ATTRIB
    w.range(383,352) = 0x08;      // OFFSET
    rx.write(w);
}

// ---------------------------------------------------------------------------
// ADMIN command -------------------------------------------------------------
// ---------------------------------------------------------------------------

void sendAdminIdentifyCtrl(hls::stream<ap_uint<DATA_WIDTH>>& rx, uint16_t cid)
{
    rx.write(make_capsule_header(0x04, 0x00, 72, 0, 72));

    ap_uint<DATA_WIDTH> w = 0;
    const ap_uint<8>  OPCODE = 0x06;
    const ap_uint<8>  FLAGS  = 0x40;
    const ap_uint<16> CID    = 18;
    const ap_uint<8>  FCTYPE = 0x00;
    ap_uint<64> SGL_ADDR   = 0;
    ap_uint<32> SGL_LENGTH = 4096;
    w.range(  7,  0) = OPCODE;
    w.range( 15,  8) = FLAGS;
    w.range( 31, 16) = cid;
    w.range( 39, 32) = FCTYPE;
    w.range(255,192) = SGL_ADDR;
    w.range(287,256) = SGL_LENGTH;
    w.range(327,320) = 0x01;   // ATTRIB
    w.range(383,352) = 0x0;    // OFST
    rx.write(w);
}

void sendAdminSetFeaturesNumQueues(hls::stream<ap_uint<DATA_WIDTH>>& rx, uint16_t cid)
{
    rx.write(make_capsule_header(0x04, 0x00, 72, 0, 72));

    ap_uint<DATA_WIDTH> w = 0;
    w.range(  7,  0) = 0x09;    // OPCODE
    w.range( 15,  8) = 0x40;    // FLAGS
    w.range( 31, 16) = cid;      // CID
    w.range( 39, 32) = 0x00;    // FCTYPE
    // NSID
    w.range( 71, 40) = 0x0;
    // SGL
    w.range(255,192) = (ap_uint<64>)0x0;
    w.range(287,256) = (ap_uint<32>)0x0;
    // ATTRIB
    w.range(327,320) = 0x07;
    // CDW11 (OFFSET field)
    w.range(383,352) = 0x004F004F;
    rx.write(w);
}

void sendAdminIdentifyActiveNSList(hls::stream<ap_uint<DATA_WIDTH>>& rx, uint16_t cid)
{
    rx.write(make_capsule_header(0x04, 0x00, 72, 0, 72));

    ap_uint<DATA_WIDTH> w = 0;
    w.range(  7,  0) = 0x06;  // OPCODE Identify
    w.range( 15,  8) = 0x40;
    w.range( 31, 16) = cid;    // CID
    w.range( 39, 32) = 0x00;  // FCTYPE
    w.range(255,192) = (ap_uint<64>)0x0; // SGL_ADDR
    w.range(287,256) = (ap_uint<32>)4096;// SGL_LENGTH
    w.range(327,320) = 0x02;  // ATTRIB (CNS=0x02)
    w.range(383,352) = 0x0;   // OFST
    rx.write(w);
}

void sendAdminIdentifyNS(hls::stream<ap_uint<DATA_WIDTH>>& rx, uint16_t cid)
{
    rx.write(make_capsule_header(0x04, 0x00, 72, 0, 72));

    ap_uint<DATA_WIDTH> w = 0;
    w.range(  7,  0) = 0x06;  // OPCODE
    w.range( 15,  8) = 0x40;
    w.range( 31, 16) = cid;    // CID
    w.range( 39, 32) = 0x01;  // FCTYPE (NS page)
    w.range( 71, 40) = 0x00000001; // NSID
    w.range(255,192) = (ap_uint<64>)0x0; // SGL_ADDR
    w.range(287,256) = (ap_uint<32>)4096;// SGL_LENGTH
    w.range(327,320) = 0x00;  // ATTRIB
    w.range(383,352) = 0x0;   // OFST
    rx.write(w);
}

void sendAdminIdentifyNSDescList(hls::stream<ap_uint<DATA_WIDTH>>& rx, uint16_t cid)
{
    rx.write(make_capsule_header(0x04, 0x00, 72, 0, 72));

    ap_uint<DATA_WIDTH> w = 0;
    w.range(  7,  0) = 0x06;   // OPCODE Identify
    w.range( 15,  8) = 0x40;
    w.range( 31, 16) = cid;     // CID
    w.range( 39, 32) = 0x01;   // FCTYPE
    w.range( 71, 40) = 0x00000001; // NSID
    w.range(255,192) = (ap_uint<64>)0x0;
    w.range(287,256) = (ap_uint<32>)4096;
    w.range(327,320) = 0x03;   // ATTRIB (descriptor list)
    w.range(383,352) = 0x0;
    rx.write(w);
}

static const char* pdu_name(uint8_t type)
{
    switch (type) {
        case 0x00: return "ICReq";
        case 0x01: return "ICResp";
        case 0x02: return "H2CTermReq";
        case 0x03: return "C2HTermReq";
        case 0x04: return "CapsuleCmd";
        case 0x05: return "CapsuleResp";
        case 0x06: return "H2CData";
        case 0x07: return "C2HData";
        case 0x09: return "R2T";
        case 0x0A: return "KDReq";
        case 0x0B: return "KDResp";
        default:   return "Unknown";
    }
}

static const char* sct_name(uint8_t sct)
{
    switch (sct) {
        case 0x0: return "Generic";
        case 0x1: return "Command‑Specific";
        case 0x2: return "Media/Data";
        case 0x3: return "Path";
        default:  return "Reserved";
    }
}



static const char* sc_fabrics_cmd_specific(uint8_t sc)
{
    switch (sc) {
        case 0x0 : return "Success";
        case 0x1 : return "Invalid Opcode";
        case 0x2 : return "Invalid Field";
        case 0xc : return "Command Sequence Error";
        case 0x80: return "Incompatible Format";
        case 0x81: return "Controller Busy";
        case 0x82: return "Connect Invalid Parameters";
        case 0x83: return "Connect Restart Discovery";
        case 0x84: return "Connect Invalid Host";
        case 0x85: return "Invalid Queue Type";
        case 0x90: return "Discover Restart";
        case 0x91: return "Authentication Required";
        default:   return "(Reserved / Transport Specific)";
    }
}

static void dump_cqe(const ap_uint<DATA_WIDTH>& first)
{
    // PSH : 23 : 08
    uint64_t frts = first.range(15*8+7, 8*8);   // bytes 15:8
    uint16_t sqhd = first.range(17*8+7, 16*8);  // bytes 17:16
    uint16_t cid  = first.range(21*8+7, 20*8);  // bytes 21:20
    uint16_t sts  = first.range(23*8+7, 22*8);  // bytes 23:22

    // SC : 24:17 (bit)
    // SCT: 27:25 (bit)

    uint8_t sc  = (sts >> 1) & 0xFF;
    uint8_t sct = (sts >> 9) & 0x07;


    printf("    CQE: FRTS=0x%016llX  SQHD=0x%04X  CID=%u  STS=0x%04X\n",
           (unsigned long long)frts, sqhd, cid, sts);
    printf("         Status – SCT:%u(%s)  SC:%u => %s\n",
           sct, sct_name(sct), sc, sc_fabrics_cmd_specific(sc));
}

static void dump_capsule_header(const ap_uint<DATA_WIDTH>& first, int idx)
{
    uint8_t TYPE  = first.range(7,0);
    uint8_t FLAGS = first.range(15,8);
    uint8_t HLEN  = first.range(23,16);
    uint8_t PDO   = first.range(31,24);
    uint32_t PLEN = first.range(63,32);

    printf("[Capsule %d] %-11s  TYPE=0x%02X  FLAGS=0x%02X  HLEN=%u  PDO=%u  PLEN=%u",
           idx, pdu_name(TYPE), TYPE, FLAGS, HLEN, PDO, PLEN);

    if(TYPE == 0x07) {
    // C2HData
        uint16_t cid = first.range(79,64);
        printf(" CID=%u\n", cid);
    }           
    else{
        printf("\n");
    }
    if (TYPE == 0x05) dump_cqe(first);
    
}

static inline unsigned words_needed(uint32_t bytes)
{
    // 1 word = 64 B  (512‑bit)
    return (bytes + 63) / 64;
}


static void parse_id_ctrl(const struct nvme_id_ctrl* id){
    printf("--------------- Identify Controller (selected fields) ---------\n");
    printf(" VID     : 0x%04X\n",id->vid);
    printf(" SSVID   : 0x%04X\n",id->ssvid);
    printf(" SN      : %s\n",id-> sn);
    printf(" MN      : %s\n",id-> mn);
    printf(" FR      : %s\n",id-> fr);
    printf(" MDTS    : 1 << %u = %u bytes\n",id->mdts, 1u<<id->mdts);
    printf(" CNTLID  : %u\n",id->cntlid);
    printf(" VER     : 0x%06X (Major:%u Minor:%u)\n",id->ver, (id->ver>>16)&0xFFFF, id->ver&0xFFFF);
    printf(" KAS     : 0x%04X\n",id->kas);
    printf(" SQES    : 0x%02X (Min %u, Max %u)\n",id->sqes, id->sqes&0xF, id->sqes>>4);
    printf(" CQES    : 0x%02X (Min %u, Max %u)\n",id->cqes, id->cqes&0xF, id->cqes>>4);
    printf(" MAXCMD  : %u\n",id->maxcmd);
    printf(" NN      : %u\n",id->nn);
    printf(" SGLS    : 0x%08X\n",id->sgls);
    printf(" SUBNQN  : %s\n",id->subnqn);
    printf("---------------------------------------------------------------\n\n");

}

static void parse_id_ns(const nvme_id_ns* ns)
{
    /*  ------------------------------------------------------ */
    printf("------------- Identify Namespace (selected fields) ------------\n");
    printf(" NSZE  : %u blocks\n", ns->nsze);
    printf(" NCAP  : %u blocks\n", ns->ncap);
    printf(" NUSE  : %u blocks\n", ns->nuse);
    printf(" NLBAF  : %u blocks\n", ns->nlbaf);
    printf(" LBAF[0].DS : %u blocks\n", ns->lbaf[0].ds);
    printf("---------------------------------------------------------------\n");
}
static void parse_active_ns(const struct id_active_ns_list* lst){
    printf("------------- Active NS Lists (selected fields) ---------------\n");

    for(int i=0;i<10;++i) printf(" NS[%d] : %u\n",i, lst->cns[i]);
    printf("---------------------------------------------------------------\n");
}
static void parse_ns_desc(const struct identify_namespace_descriptor* base,size_t len){
    printf("------------------- Namespace Descriptor(s) -------------------\n");
    
    const uint8_t* p=(const uint8_t*)base; size_t off=0,idx=0;
    while(off+2<=len){
        uint8_t nidt=p[off]; uint8_t nidl=p[off+1];
        if(nidl==0||off+2+nidl>len) break;
        printf(" Desc %zu: NIDT=0x%02X NIDL=%u NID=",idx,nidt,nidl);
        for(int i=0;i<nidl && i<16;++i) printf("%02X",p[off+2+i]); printf("\n");
        off+=2+nidl; ++idx;
    }
    printf("---------------------------------------------------------------\n");

}
// ---------------------------------------------------------------------------
// Main ----------------------------------------------------------------------
// ---------------------------------------------------------------------------

int main()
{
    hls::stream<ap_uint<DATA_WIDTH>> nvme_tcp_rxdata;
    hls::stream<ap_uint<DATA_WIDTH>> nvme_tcp_txdata;
    hls::stream<request_packet>      nvme_req_stream;
    hls::stream<completion_packet>   nvme_cpl_stream;

    // 테스트 시퀀스 -----------------------------------------------------------
    sendICReq(nvme_tcp_rxdata, 0); // 0
    sendConnect(nvme_tcp_rxdata, 1); // 1
    sendGetCAP(nvme_tcp_rxdata, 2); // 2
    sendSetCC(nvme_tcp_rxdata, 3); // 3
    sendGetCSTS(nvme_tcp_rxdata, 4); // 4
    sendGetVer(nvme_tcp_rxdata, 5); // 5

    sendAdminIdentifyCtrl(nvme_tcp_rxdata, 6); // 6

    sendAdminSetFeaturesNumQueues(nvme_tcp_rxdata, 7);
    sendAdminIdentifyActiveNSList(nvme_tcp_rxdata, 8);
    sendAdminIdentifyNS(nvme_tcp_rxdata, 9);
    sendAdminIdentifyNSDescList(nvme_tcp_rxdata, 10);

    
    for (int i = 0; i < 1000; ++i) {
        nvme_tcp_driver_top(nvme_tcp_rxdata,
                            nvme_tcp_txdata,
                            nvme_req_stream,
                            nvme_cpl_stream);
    } 
        
   
    // Dump result ---------------------------------------------------------------
    int capsule_idx = 0;    
    uint8_t buffer[BUF_SIZE];
    int cid;
    int verbose = 1;
    while (!nvme_tcp_txdata.empty()) {
      
        ap_uint<DATA_WIDTH> first = nvme_tcp_txdata.read();
        dump_capsule_header(first, capsule_idx);
        uint8_t TYPE  = first.range(7,0);
        uint8_t PDO   = first.range(31,24);
        if(TYPE == 0x07) cid = first.range(79,64);

    
        uint32_t plen_bytes = first.range(63,32).to_uint();
        uint32_t hlen_bytes = first.range(23,16).to_uint();
        unsigned total_words = (TYPE == 0x1)? 2 : (TYPE == 0x7)? words_needed(plen_bytes - hlen_bytes) + 1 : 1;
        
        for (unsigned w = 1; w < total_words && !nvme_tcp_txdata.empty(); ++w){
            ap_uint<DATA_WIDTH> data_packet = nvme_tcp_txdata.read();
            if(w % 16 == 0 || w == (total_words - 1)) printf("[Capsule %d] Packet %d/%d read\n", capsule_idx, w, total_words-1);
            if(TYPE == 0x07){ // C2HData
                for(unsigned i = 0; i < 64; ++i) {
                    buffer[(w-1)*64+i] = data_packet.range(i*8+7, i*8);
                }
            }            
        }
        ++capsule_idx;
        if(verbose && TYPE == 0x07) {
            if(cid == 6){
                struct nvme_id_ctrl* id = (struct nvme_id_ctrl*)buffer;
                parse_id_ctrl(id);
            }
            else if(cid == 9) {
                struct nvme_id_ns* ns = (struct nvme_id_ns*)buffer;
                parse_id_ns(ns);
            }
            else if(cid == 8) {
                struct id_active_ns_list* lst = (struct id_active_ns_list*)buffer;
                parse_active_ns(lst);
            }
            else if(cid == 10) {
                struct identify_namespace_descriptor* desc = (struct identify_namespace_descriptor*)buffer;
                parse_ns_desc(desc, plen_bytes);
            }        
        }
    }

    return 0;
}
