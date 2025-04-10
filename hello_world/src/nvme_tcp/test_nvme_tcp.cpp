#include <stdio.h>
#include "nvme_tcp_driver_top.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>    // for memcpy
#include <cstdint>
#include <hls_stream.h>
#include <ap_int.h>
#define DATA_WIDTH 512
#define file_path "/home/csl/heejae/Vitis_Accel_Examples/hello_world/src/nvme_tcp/trace.log"

// 테스트 벤치 메인 함수
int main() {
    hls::stream<ap_uint<DATA_WIDTH>> nvme_tcp_rxdata;
    hls::stream<ap_uint<DATA_WIDTH>> nvme_tcp_txdata;
    hls::stream<request_packet> nvme_request_stream;
    hls::stream<completion_packet> nvme_response_stream;

    static nvme_cqe_t admin_cq_base_addr[16];
    static nvme_sqe_t admin_sq_base_addr[16];
    static uint32_t   bar_base_address[16];
    static uint32_t   dbl_base_address[16];
    static uint64_t   buffer_virtual_address[256];

    // --------------------------------------------------------
    // trace.log 파일 열어서 64바이트씩 읽어오기
    // --------------------------------------------------------
    std::ifstream infile(file_path, std::ios::binary);
    if(!infile.is_open()){
        std::cerr << "Failed to open trace.log" << std::endl;
        return -1;
    }

    // 64바이트 버퍼
    const size_t chunk_size_bytes = DATA_WIDTH / 8; // 64바이트
    char buffer[chunk_size_bytes];

    while(true) {
        infile.read(buffer, chunk_size_bytes);
        std::streamsize read_size = infile.gcount();
        
        if(read_size <= 0) {
            // 더 이상 읽을 데이터가 없으면 종료
            break;
        }

        // 64바이트보다 작게 읽혔을 경우(파일 끝), 남는 부분은 0으로 채운다
        if(read_size < (std::streamsize)chunk_size_bytes) {
            std::memset(buffer + read_size, 0, chunk_size_bytes - read_size);
        }

        // ap_uint<512>에 복사
        ap_uint<DATA_WIDTH> data_512b = 0;
        // NOTE: ap_uint<>은 bit 단위로 접근이 복잡하므로, 
        //       간단히 바이트 단위로 reinterpret_cast 후 memcpy 하는 경우가 많습니다.
        //       여기서는 정석적으로 loop로 bit shift를 해도 되고, 아래처럼 union을 써도 됩니다.
        
        // 방법1) 바이트 단위 for-loop로 직접 대입
        for(int i = 0; i < (int)chunk_size_bytes; i++) {
            // buffer[i]를 data_512b의 (i*8 ~ i*8+7) 비트 구간에 넣기
            ap_uint<8> temp = (unsigned char) buffer[i];
            data_512b.range((i+1)*8 -1, i*8) = temp;
        }

        // 스트림에 write
        nvme_tcp_rxdata.write(data_512b);

        // DUT 호출
        nvme_tcp_driver_top(
            nvme_tcp_rxdata,
            nvme_tcp_txdata,
            admin_cq_base_addr,
            admin_sq_base_addr,
            bar_base_address,
            dbl_base_address,
            /* buffer_physical_address */ 0ULL,
            buffer_virtual_address,
            nvme_request_stream,
            nvme_response_stream
        );

        // --------------------------------------------------------
        // 3) DUT 동작 후: nvme_request_stream에서 request_packet 확인
        //    (실제 NVMe command가 잘 올라왔는지 보기 위한 예)
        // --------------------------------------------------------
        while(!nvme_request_stream.empty()) {
            request_packet req = nvme_request_stream.read();
            std::cout << "[REQUEST] "
                      << "CMD ID=" << req.dw0.id
                      << ", OPC=" << req.dw0.opc
                      << ", SQID=" << req.dw0.sqid
                      << ", NLB=" << req.dw2.nlb
                      << ", SLBA=" << ( (uint64_t)req.dw2.slba << 32 | req.dw1 )
                      << std::endl;
        }

        // 여기서는 예시로 “한 번 읽을 때마다 response 해보자”라고 가정
        // 실제로는 NVMe 디바이스 측에서 completion이 오면 넣어주는 흐름이겠지만
        // 우리가 테스트이므로 임의의 completion_packet을 만들어 스트림에 넣어봄
        completion_packet cpl;
        cpl.dw0.id = 0xABCD;   // 임의의 값
        cpl.dw0.sc = 0x00;     // status code
        cpl.dw0.sct = 0x00;    // status code type
        nvme_response_stream.write(cpl);

        // 다시 DUT 호출 -> response를 처리하여 txdata가 나오는지 확인
        nvme_tcp_driver_top(
            nvme_tcp_rxdata,
            nvme_tcp_txdata,
            admin_cq_base_addr,
            admin_sq_base_addr,
            bar_base_address,
            dbl_base_address,
            /* buffer_physical_address */ 0ULL,
            buffer_virtual_address,
            nvme_request_stream,
            nvme_response_stream
        );

        // --------------------------------------------------------
        // 4) DUT 동작 후: nvme_tcp_txdata에서 데이터 확인
        //    (completion_packet이 변환되어 전송되었는지 확인)
        // --------------------------------------------------------
        while(!nvme_tcp_txdata.empty()) {
            ap_uint<DATA_WIDTH> tx_data = nvme_tcp_txdata.read();

            // 여기서는 단순히 64바이트를 16진수로 찍거나, 원하는 포맷으로 확인
            // 예시로 몇 바이트만 찍어보기
            std::cout << "[TXDATA] 64B CHUNK: ";
            for(int i = 0; i < (int)chunk_size_bytes; i++) {
                unsigned char c = (unsigned char)tx_data.range((i+1)*8 -1, i*8).to_uint();
                std::cout << std::hex << (int)c << " ";
            }
            std::cout << std::dec << std::endl;
        }
    }

    infile.close();

    std::cout << "Testbench Done." << std::endl;
    return 0;

}
