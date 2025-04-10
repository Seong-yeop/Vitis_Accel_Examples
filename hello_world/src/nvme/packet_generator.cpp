#include "nvme_driver_top.hpp"

extern "C" {
void packet_generator(
    uint32_t opcode,
    uint64_t slba,
    uint32_t nlb,
    uint64_t prp1,
    uint32_t request_count,
    hls::stream<struct request_packet> &request_packet_stream
)
{
    static uint16_t cid = 0;
    uint64_t slba_base = slba;
    uint64_t prp1_base = prp1;
    
    for (uint32_t i = 0; i < request_count; i++) {
        struct request_packet req;
        req.dw0.opc = opcode;
        req.dw0.id = cid++;
        req.dw1 = (uint32_t)(slba_base & 0xffffffffUL);
        req.dw2.slba = (uint32_t)(slba_base >> 32);
        slba_base += nlb;
        req.dw2.nlb = nlb;
        // buffer address        
        req.dw3 = prp1_base;
        prp1_base += nlb * 0x1000;
        request_packet_stream.write(req);
    }
}
}