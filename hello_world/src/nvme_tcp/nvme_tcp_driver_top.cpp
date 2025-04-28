#include "nvme_tcp_driver_top.hpp"

void send_buffer(
    hls::stream<ap_uint<DATA_WIDTH>> &admin_queue_txdata, // receive
    hls::stream<ap_uint<DATA_WIDTH>> &io_queue_txdata, // receive
    hls::stream<ap_uint<DATA_WIDTH>> &nvme_tcp_txdata // transmit
){
    #pragma HLS INLINE off
    #pragma HLS PIPELINE II=1
    static int current_queue = 0; 
    if(!admin_queue_txdata.empty()){
        ap_uint<DATA_WIDTH> data = admin_queue_txdata.read();
        nvme_tcp_txdata.write(data);
    }
    else if(!io_queue_txdata.empty()){ // To be implemented..
        ap_uint<DATA_WIDTH> data = io_queue_txdata.read();
        nvme_tcp_txdata.write(data);
    }
}

void process_io_queue(
    hls::stream<ap_uint<DATA_WIDTH>> &io_queue_rxdata,
    hls::stream<struct request_packet> &nvme_request_stream,
    hls::stream<struct completion_packet> &nvme_response_stream,
    hls::stream<ap_uint<DATA_WIDTH>> &out_stream
){
    #pragma HLS INLINE off
    if(!io_queue_rxdata.empty()) {
        ap_uint<DATA_WIDTH> data = io_queue_rxdata.read();
        out_stream.write(data);
        struct request_packet req;
        nvme_request_stream.write(req);

    }
    else if(!nvme_response_stream.empty()) {
        struct completion_packet read_packet;
        read_packet = nvme_response_stream.read();
    }
}

void parse_pdu(
    hls::stream<ap_uint<DATA_WIDTH>> &nvme_tcp_rxdata, // receive
    hls::stream<ap_uint<DATA_WIDTH>> &admin_queue_rxdata, // only 16B
    hls::stream<ap_uint<DATA_WIDTH>> &io_queue_rxdata // receive
){
    #pragma HLS INLINE off
    #pragma HLS PIPELINE II=1
    enum parse_state_e {WAIT_PDU_CH, PARSE_PDU_HDR, 
                        DISPATCH_ADMIN_CH, DISPATCH_ADMIN_PSH, 
                        FETCH_NVME_ADMIN_DATA, DISPATCH_NVME_ADMIN_DATA,
                        DISPATCH_IO_CH, DISPATCH_IO_PSH,
                        FETCH_NVME_IO_DATA, DISPATCH_NVME_IO_DATA};
    static parse_state_e state = WAIT_PDU_CH;
       
    static struct packet_header packet_hdr;
    static nvme_sqe_t cmd;
    static ap_uint<DATA_WIDTH> hdr_packet;
    static ap_uint<DATA_WIDTH> psh_packet;
    static ap_uint<DATA_WIDTH> data_packet;
    static uint32_t left_bytes;

    static const int dwidth_in_bytes = DATA_WIDTH / 8;
    switch(state){
        case WAIT_PDU_CH: // read CH
            if (!nvme_tcp_rxdata.empty()) {
                hdr_packet = nvme_tcp_rxdata.read();
                packet_hdr = ap_uint_to_packet_header(hdr_packet);
                // hls::print("<STATE:WAIT_PDU_CH> nvme_tcp_rxdata read (id %d)\n", packet_hdr.session_id);
                state = PARSE_PDU_HDR;
            }
        break;
        case PARSE_PDU_HDR:
            if(packet_hdr.session_id == 0){
                // hls::print("<STATE:PARSE_PDU_HDR> dispatch to admin (id %d)\n", packet_hdr.session_id);
                state = DISPATCH_ADMIN_CH;
            }
            else{
                // hls::print("<STATE:PARSE_PDU_HDR> dispatch to io (id %d)\n", packet_hdr.session_id);
                state = DISPATCH_IO_CH;
            }
        break;
        case DISPATCH_ADMIN_CH: // Write NVME CONNECT CH at icreq stream
                // hls::print("<STATE:DISPATCH_ADMIN_CH> write packet to admin queue (hdr type %d)\n", packet_hdr.pdu_hdr.type);
                admin_queue_rxdata.write(hdr_packet);
                state = DISPATCH_ADMIN_PSH;
        break;
        case DISPATCH_ADMIN_PSH: // Write NVME CONNECT PSH at icreq stream
            if (!nvme_tcp_rxdata.empty()) {
                // hls::print("<STATE:DISPATCH_ADMIN_CH> write packet to admin queue (hdr type %d)\n", packet_hdr.pdu_hdr.type);
                psh_packet = nvme_tcp_rxdata.read();
                admin_queue_rxdata.write(psh_packet);
                left_bytes = packet_hdr.pdu_hdr.plen - packet_hdr.pdu_hdr.hlen;
                if(left_bytes > 0){
                    state = FETCH_NVME_ADMIN_DATA;
                }
                else{
                    state = WAIT_PDU_CH;
                }
            }
        break;
        case FETCH_NVME_ADMIN_DATA:
            if (!nvme_tcp_rxdata.empty()) {
                // hls::print("<STATE: FETCH_NVME_ADMIN_DATA > fetch data (hdr type %d)\n", packet_hdr.pdu_hdr.type);
                data_packet = nvme_tcp_rxdata.read();
                state = DISPATCH_NVME_ADMIN_DATA;
            }
        break;
        case DISPATCH_NVME_ADMIN_DATA:
            if (!admin_queue_rxdata.full()) {
                // hls::print("<STATE: DISPATCH_NVME_ADMIN_DATA > write data to admin queue (hdr type %d)\n", packet_hdr.pdu_hdr.type);
                admin_queue_rxdata.write(data_packet);
                left_bytes -= dwidth_in_bytes;
                if(left_bytes > 0){
                    state = FETCH_NVME_ADMIN_DATA;
                }
                else{
                    left_bytes = 0;
                    state = WAIT_PDU_CH;
                }
            }
        break;
        case DISPATCH_IO_CH:
                // hls::print("<STATE: DISPATCH_IO_CH > write hdr to io queue (hdr type %d)\n", packet_hdr.pdu_hdr.type);
                io_queue_rxdata.write(hdr_packet);
                state = DISPATCH_IO_PSH;
        break;
        case DISPATCH_IO_PSH: // Write NVME CONNECT PSH at icreq stream
            if (!nvme_tcp_rxdata.empty()) {
                // hls::print("<STATE: DISPATCH_IO_PSH > write psh to io queue (hdr type %d)\n", packet_hdr.pdu_hdr.type);
                psh_packet = nvme_tcp_rxdata.read();
                io_queue_rxdata.write(psh_packet);
                left_bytes = packet_hdr.pdu_hdr.plen - packet_hdr.pdu_hdr.hlen;
                if(left_bytes > 0){
                    state = FETCH_NVME_IO_DATA;
                }
                else{
                    state = WAIT_PDU_CH;
                }
            }
        break;
        case FETCH_NVME_IO_DATA:
            if (!nvme_tcp_rxdata.empty()) {
                // hls::print("<STATE: FETCH_NVME_IO_DATA > fetch io data (hdr type %d)\n", packet_hdr.pdu_hdr.type);
                data_packet = nvme_tcp_rxdata.read();
                state = DISPATCH_NVME_IO_DATA;
            }
        break;
        case DISPATCH_NVME_IO_DATA:
            if (!io_queue_rxdata.full()) {
                // hls::print("<STATE: FETCH_NVME_IO_DATA > dispatch io data (hdr type %d)\n", packet_hdr.pdu_hdr.type);
                io_queue_rxdata.write(data_packet);
                left_bytes -= dwidth_in_bytes;
                if(left_bytes > 0){
                    state = FETCH_NVME_IO_DATA;
                }
                else{
                    left_bytes = 0;
                    state = WAIT_PDU_CH;
                }
            }
        break;
    }
}

extern "C" {
    void nvme_tcp_driver_top(
        // from network stream
        hls::stream<ap_uint<DATA_WIDTH>> &nvme_tcp_rxdata, // receive
        hls::stream<ap_uint<DATA_WIDTH>> &nvme_tcp_txdata, // transmit
        // for nvme driver
        hls::stream<struct request_packet> &nvme_request_stream,
        hls::stream<struct completion_packet> &nvme_response_stream
    )
    {
        #pragma HLS DATAFLOW disable_start_propagation
        #pragma HLS INTERFACE ap_ctrl_none port=return

        #pragma HLS INTERFACE axis port=nvme_tcp_rxdata
        #pragma HLS INTERFACE axis port=nvme_tcp_txdata

   
        static hls::stream<ap_uint<DATA_WIDTH>>  admin_queue_rxdata;
        #pragma HLS STREAM variable=admin_queue_rxdata depth=4096
        static hls::stream<ap_uint<DATA_WIDTH>>  io_queue_rxdata;
        #pragma HLS STREAM variable=io_queue_rxdata depth=4096
   
        static hls::stream<ap_uint<DATA_WIDTH>>  admin_queue_txdata;
        #pragma HLS STREAM variable=admin_queue_txdata depth=4096
        static hls::stream<ap_uint<DATA_WIDTH>>  io_queue_txdata;
        #pragma HLS STREAM variable=io_queue_txdata depth=4096

        parse_pdu(
            nvme_tcp_rxdata, // receive
            admin_queue_rxdata, // only 16B
            io_queue_rxdata // receive
        );

        process_io_queue(
            io_queue_rxdata,
            nvme_request_stream,
            nvme_response_stream,
            io_queue_txdata
        );

        process_admin_queue(
            admin_queue_rxdata, // receive
            admin_queue_txdata
        );

        send_buffer(
            admin_queue_txdata, // receive
            io_queue_txdata, // receive
            nvme_tcp_txdata // transmit
        );
        
    }
}