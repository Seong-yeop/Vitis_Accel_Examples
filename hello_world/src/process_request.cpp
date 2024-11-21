#include <stdint.h>
#include <hls_stream.h>
#include <cstdio>  // printf를 사용하기 위한 표준 C 라이브러리 포함

#include "process_request.hpp"
#include "tcp.hpp"

#define QUEUE_SIZE 1024

// 더미 데이터를 채우는 함수
template <int D>
void fill_rx_packet_stream(hls::stream<rx_packet<D>>& rx_packet_stream) {
        #pragma HLS INLINE off

        static bool state = 0;

        switch (state) {
            case 0:
            {
                if (!rx_packet_stream.full()) {
                    state = 1;
                }
                break;
            }
            case 1:
            {
                rx_packet<D> packet;
                packet.session_id = state;               // 세션 ID를 0부터 num_packets-1까지 채움
                packet.length = 64;                  // 예를 들어, 고정된 길이 64 설정
                packet.data.data = 0xDEADBEEF;   // 더미 데이터 (임의의 패턴)
                packet.data.keep = -1;               // 모든 바이트가 유효하다는 의미로 `keep`을 설정 (전체 비트 1)
                packet.data.last = 0; // 마지막 패킷에는 `last` 플래그 설정

                // 스트림에 패킷 추가
                rx_packet_stream.write(packet);
                state = 0;
                break;
            }

        }
}

template <int D>
void rx_handler(char* rxq_addresses, 
                char* rx_buffer,
                hls::stream<rx_packet<D>>& rx_packet_stream,
                hls::stream<uint32_t>& rx_head_stream,
                hls::stream<uint32_t>& rx_tail_stream)
{
    #pragma HLS INLINE off
    static rx_state state = rx_state::IDLE;
    static uint32_t _rx_tail = 0;
    static uint32_t _rx_head = 0;

    #pragma HLS reset variable=state
    #pragma HLS reset variable=_rx_tail

    switch (state) {
        case rx_state::IDLE:
        {
            printf("IDLE\n");
            // `rx_head_stream`에서 `rx_head` 값을 읽을 준비가 되었는지 확인
            if (!rx_head_stream.empty()) {
                _rx_head = rx_head_stream.read();
                state = rx_state::IDLE;
            }
            else {
                if ((_rx_tail + 1) % QUEUE_SIZE == _rx_head) {
                    state = rx_state::IDLE;
                }
                else {
                    state = rx_state::WRITE_PACKET;
                }

            }
            break;
        }
        case rx_state::WRITE_PACKET:
        {
            printf("WRITE_PACKET\n");
            // `rx_packet_stream`에서 데이터를 읽어 패킷 저장
            if (!rx_packet_stream.empty()) {
                rx_packet<D> packet = rx_packet_stream.read();

                printf("Received packet: session_id=%d, length=%d\n", packet.session_id, packet.length);
                // rx_packet_host 구조체에 필요한 데이터를 설정
                rx_packet_host packet_host;
                packet_host.session_id = packet.session_id;
                packet_host.length = packet.length;

                int offset = _rx_tail * sizeof(rx_packet_host);
                // rxq_addresses 배열에 패킷을 복사
                rxq_addresses[offset]     = (packet_host.session_id >> 0) & 0xFF;
                rxq_addresses[offset + 1] = (packet_host.session_id >> 8) & 0xFF;
                rxq_addresses[offset + 2] = (packet_host.length >> 0) & 0xFF;
                rxq_addresses[offset + 3] = (packet_host.length >> 8) & 0xFF;

                // Copy the data to the buffer
                // net_axis<D> data = '1234';
                // write_bandwidth(rx_buffer + 1500*_rx_tail, packet.data.data, packet.length);
                rx_buffer[1500*_rx_tail + 0] = 65;
                rx_buffer[1500*_rx_tail + 1] = 66;
                rx_buffer[1500*_rx_tail + 2] = 67;
                rx_buffer[1500*_rx_tail + 3] = 68;
                rx_buffer[1500*_rx_tail + 4] = 69;

                // `_rx_tail` 업데이트
                _rx_tail = (_rx_tail + 1) % QUEUE_SIZE;
                // 다음 상태로 이동하여 tail을 업데이트할 준비
                state = rx_state::UPDATE_TAIL;
            }
            else {
                // 패킷이 없는 경우 IDLE 상태로 전환
                state = rx_state::IDLE;
            }
            break;
        }

        case rx_state::UPDATE_TAIL:
        {   
            printf("UPDATE_TAIL\n");
            // 업데이트된 `_rx_tail`을 `rx_tail_stream`에 기록
            if (!rx_tail_stream.full()) {
                rx_tail_stream.write(_rx_tail);
                state = rx_state::IDLE;
            }
            else {
                state = rx_state::UPDATE_TAIL;
            }
            break;
        }
    }
}



void update_rx_tail(hls::stream<uint32_t>& rx_tail_stream, uint32_t* rx_tail, uint32_t* tx_head) {
    if (!rx_tail_stream.empty()) { // stream에 데이터가 안 들어오는 듯
        *rx_tail = rx_tail_stream.read();
    }
    else {
    }
}

void update_rx_head_stream(uint32_t* rx_head, hls::stream<uint32_t>& rx_head_stream) {
    static uint32_t rx_head_prev = 0;
    static bool first_write = true; // 첫 번째 값을 기록했는지 여부

    if (!rx_head_stream.full()) {
        // 첫 번째 값이거나 이전 값과 다를 때만 스트림에 기록
        if (first_write || *rx_head != rx_head_prev) {
            rx_head_prev = *rx_head;
            rx_head_stream.write(*rx_head);
            first_write = false; // 첫 번째 기록 이후에는 false로 설정
        }
    }
}

void update_tx_tail_stream(uint32_t* tx_tail, hls::stream<uint32_t>& tx_tail_stream) {
    static uint32_t tx_tail_prev = 0;
    static bool first_write = true;
    if(!tx_tail_stream.full()) {
        if (first_write || *tx_tail != tx_tail_prev) {
            tx_tail_prev = *tx_tail;
            tx_tail_stream.write(*tx_tail);
            first_write = false;
        }
    }
}

void update_tx_head(hls::stream<uint32_t>& tx_head_stream, uint32_t* tx_head) {
    if (!tx_head_stream.empty()) {
        *tx_head = tx_head_stream.read();
    }
}

void tx_handler(uint16_t* txq_addresses,
                char* tx_buffer,
                hls::stream<uint32_t>& tx_head_stream,
                hls::stream<uint32_t>& tx_tail_stream) 
{
    #pragma HLS INLINE OFF
    enum tx_state {IDLE, SEND_PACKET, UPDATE_HEAD};
    static tx_state state = tx_state::IDLE;
    static uint32_t _tx_head = 0;
    static uint32_t _tx_tail = 0;

    #pragma HLS reset variable=state
    #pragma HLS reset variable=_tx_head

    switch (state) {
        case tx_state::IDLE:
        {
            printf("IDLE\n");
            // Check if the tx_head_stream is ready to read
            if (!tx_tail_stream.empty()) {
                _tx_tail = tx_tail_stream.read();
                state = tx_state::IDLE;
            }
            else {
                if (_tx_head == _tx_tail) { // empty
                    state = tx_state::IDLE;
                }
                else {
                    state = tx_state::SEND_PACKET;
                }
            }
            break;
        }
        case tx_state::SEND_PACKET:
        {
            printf("SEND_PACKET\n");
            // Process or send the data
            uint16_t session_id = txq_addresses[_tx_head * 2];
            uint16_t length = txq_addresses[_tx_head * 2 + 1];

            printf("Received packet: session_id=%u, length=%u\n", session_id, length);
            
            for (uint16_t i = 0; i < 1500; i++) {
                char tmp = tx_buffer[1500 * _tx_head + i];
                printf("%c", tmp);
            }
            printf("\n");

            // Update the tx_head
            _tx_head = (_tx_head + 1) % QUEUE_SIZE;
            state = tx_state::UPDATE_HEAD;
            break;
        }
        case tx_state::UPDATE_HEAD:
        {
            printf("UPDATE_HEAD\n");
            // Update the tx_head_stream
            if (!tx_head_stream.full()) {
                tx_head_stream.write(_tx_head);
                state = tx_state::IDLE;
            }
            else {
                state = tx_state::UPDATE_HEAD;
            }
            break;
        }
    }
}


extern "C" {
void process_request(char* rxq_addresses, 
        char* rx_buffer,
        uint16_t* txq_addresses,
        char* tx_buffer,
        uint32_t* rx_head, 
        uint32_t* rx_tail, 
        uint32_t* tx_head, 
        uint32_t* tx_tail)
{
    #pragma HLS INTERFACE mode=m_axi port=rxq_addresses bundle=gmem0 offset=slave max_read_burst_length=64
    #pragma HLS INTERFACE mode=m_axi port=rx_buffer bundle=gmem0 offset=slave max_read_burst_length=64
    #pragma HLS INTERFACE mode=m_axi port=txq_addresses bundle=gmem1 offset=slave max_write_burst_length=64
    #pragma HLS INTERFACE mode=m_axi port=tx_buffer bundle=gmem1 offset=slave max_write_burst_length=64
    #pragma HLS INTERFACE mode=s_axilite port=rx_head 
    #pragma HLS INTERFACE mode=s_axilite port=rx_tail 
    #pragma HLS INTERFACE mode=s_axilite port=tx_head 
    #pragma HLS INTERFACE mode=s_axilite port=tx_tail 
    #pragma HLS INTERFACE mode=s_axilite port=return

    static hls::stream<rx_packet<512>> rx_packet_stream;
    #pragma HLS STREAM variable=rx_packet_stream type=fifo depth=512

    static hls::stream<uint32_t> rx_head_stream;
    static hls::stream<uint32_t> rx_tail_stream;
    #pragma HLS STREAM variable=rx_head_stream type=fifo depth=20
    #pragma HLS STREAM variable=rx_tail_stream type=fifo depth=20


    static hls::stream<uint32_t> tx_head_stream;
    static hls::stream<uint32_t> tx_tail_stream;
    #pragma HLS STREAM variable=tx_head_stream type=fifo depth=20
    #pragma HLS STREAM variable=tx_tail_stream type=fifo depth=20


    // for (int i = 0; i < 10; i++) {
    //     #pragma HLS PIPELINE OFF
    //     rx_packet<512> packet;
    //     packet.session_id = i;
    //     packet.length = 64;
    //     rx_packet_stream.write(packet);
    // }

    // #pragma HLS DATAFLOW
    static int count = 0;
    while (count++ < 20) {
        update_tx_tail_stream(tx_tail, tx_tail_stream);
        tx_handler(txq_addresses, tx_buffer, tx_head_stream, tx_tail_stream);
        update_tx_head(tx_head_stream, tx_head);
    }
    // static int count = 0;
    // while (count++ < 200) {
    //     // fill_rx_packet_stream(rx_packet_stream);
    //     update_rx_head_stream(rx_head, rx_head_stream);
    //     rx_handler(rxq_addresses, rx_buffer, rx_packet_stream, rx_head_stream, rx_tail_stream);
    //     update_rx_tail(rx_tail_stream, rx_tail, tx_head); 
    // }

}
}

