#include "packet_generator.hpp"

extern "C" {
void packet_generator(
    hls::stream<ap_uint<DATA_WIDTH>> &nvme_tcp_rxdata,
    hls::stream<ap_uint<DATA_WIDTH>> &nvme_tcp_txdata
)
{

    nvme_tcp_rxdata.write(0x0); // dummy write
    ap_uint<DATA_WIDTH> dummy_data = nvme_tcp_txdata.read(); // dummy write
    /*
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
    */
}
}