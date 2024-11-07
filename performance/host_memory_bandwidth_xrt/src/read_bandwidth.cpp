#include <ap_int.h>
#include <hls_stream.h>
#include <iostream>

auto constexpr DATA_WIDTH = 512;
auto constexpr c_widthInBytes = DATA_WIDTH / 8;
auto constexpr c_maxBurstSize = 4 * 1024; // 4KB
auto constexpr c_burstLength = c_maxBurstSize / c_widthInBytes;

using TYPE = ap_uint<DATA_WIDTH>;

extern "C" {
void read_bandwidth(TYPE* input0, hls::stream<TYPE>& out, int64_t buf_size, int64_t iter) {
#pragma HLS INTERFACE m_axi port = input0 offset = slave bundle = gmem max_read_burst_length = \
    64 num_read_outstanding = 16
#pragma HLS INTERFACE axis port = out
#pragma HLS INTERFACE s_axilite port = input0
#pragma HLS INTERFACE s_axilite port = buf_size
#pragma HLS INTERFACE s_axilite port = iter
#pragma HLS INTERFACE s_axilite port = return

    uint32_t factor = buf_size / c_maxBurstSize;
    uint32_t Indx = 0;
    uint32_t baseAddr = 0;

    if (buf_size <= 8 * 1024) {
    bandwidth_small_1:
        for (int itr = 0; itr < iter * factor; itr++) {
        bandwidth_small_2:
            for (int i = 0; i < c_burstLength; i++) {
                // Write data from input0 to the stream
                out.write(input0[baseAddr + i]);                 
            }
            Indx = itr % factor;
            baseAddr = c_burstLength * Indx;
        }
    } else {
        buf_size = buf_size / c_widthInBytes;
    bandwidth_large_1:
        for (int64_t i = 0; i < iter; i++) {
        bandwidth_large_2:
            for (int64_t blockindex = 0; blockindex < buf_size; blockindex++) {
                // Write data from input0 to the stream
                out.write(input0[blockindex]);
            }
        }
    }
}
}
