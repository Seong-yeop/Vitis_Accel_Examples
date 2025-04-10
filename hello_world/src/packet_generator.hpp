#include <hls_stream.h>
#include <ap_int.h>
#include <stdint.h>
#define DATA_WIDTH 512

extern "C" {
    void packet_generator(
        hls::stream<ap_uint<DATA_WIDTH>> &nvme_tcp_rxdata,
        hls::stream<ap_uint<DATA_WIDTH>> &nvme_tcp_txdata
    );
}


