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
    for (uint32_t i = 0; i < request_count; i++) {
        struct request_packet req;
        req.dw0.opc = opcode;
        req.dw1 = (uint32_t)(slba & 0xffffffffUL);
        req.dw2.slba = (uint32_t)(slba >> 32);
        req.dw2.nlb = nlb;
        // buffer address        
        req.dw3= prp1;
        request_packet_stream.write(req);
    }

}
}