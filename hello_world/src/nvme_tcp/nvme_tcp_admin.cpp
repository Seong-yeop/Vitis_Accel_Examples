#include "nvme_tcp_admin.hpp"
//#include <hls_print.h>      // Vitis/Vivado HLS용, 필요 없으면 제거


static inline int compare_subsys_nqn(
    ap_uint<DATA_WIDTH> data_packet,
    uint32_t index
){

    ap_uint<512> subsys_nqn_const_512 = ap_uint_8_to_512(subsys_nqn_const);
    switch(index){
        case 4:
            if(data_packet(29,0) != ap_uint_8_to_512(subsys_nqn_const)(29,0)){
                #ifndef __SYNTHESIS__
                printf("\tsubsys is not equal in [%d]\n", index);
                printf("\tdata_packet = %s\n", (char*)&data_packet);
                printf("\tsubsys_nqn_const = %s\n", (char*)&subsys_nqn_const_512);
                #endif
                return 0; // FALSE
            }
            else{
                return 1;
            }
        break;
        case 5: // No matters what is after \0(NULL)
        case 6:
        case 7: 
        default:
            return 1; // TRUE
        break;    
    }
}


static inline struct cmd_resp_t fabrics_cmd(struct nvme_fabrics_cmd cmd, struct nvme_properties &props) {
    struct cmd_resp_t resp;
    resp = cmd_resp_format;
    resp.psh.cid = cmd.cid;
    uint64_t val;
    switch (cmd.fctype) {
        case FCTYPE_GET_PROP:
        switch (cmd.cdw11) {
                case 0x0:  val = props.cap;  break;
                case 0x8:  val = props.vs;   break;
                case 0x14: val = props.cc;   break;
                case 0x1c: val = props.csts; break;
                default:
                    val = 0;
                    resp.psh.sf = 1 << 15 | (SCT_GENERIC & 0x7) << 9 | (SC_INVALID_FIELD & 0xFF) << 1;
            }
            resp.psh.dw0 = val & 0xffffffff;
            resp.psh.dw1 = val >> 32;     
       
            break;
        case FCTYPE_SET_PROP:
        switch (cmd.cdw11) {
                case 0x14: props.cc = cmd.cdw12;
                case 0x0:
                case 0x8:
                case 0x1c:
                    resp.psh.dw0 = cmd.cdw12;
                    props.csts = (props.csts & 0xfffffffe) | (props.cc & 1);
                    if (((props.cc >> 14) & 0x3) > 0)
                        props.csts = (props.csts & 0xfffffff3) | 0x8;
                break;
                default:
                    resp.psh.sf = 1 << 15 | (SCT_GENERIC & 0x7) << 9 | (SC_INVALID_FIELD & 0xFF) << 1;
            }
            break;
        case FCTYPE_CONNECT:
            resp.psh.sf = 1 << 15 | (SCT_GENERIC & 0x7) << 9 | (SC_COMMAND_SEQ & 0xFF) << 1;
            break;
        default:
            resp.psh.sf = 1 << 15 | (SCT_GENERIC & 0x7) << 9 | (SC_INVALID_FIELD & 0xFF) << 1;
    }
    return resp;
}

static inline struct cmd_resp_t admin_set_features(struct nvme_fabrics_cmd cmd) {
    struct cmd_resp_t resp;
    resp = cmd_resp_format;
    resp.psh.cid = cmd.cid;    
    uint8_t fid = cmd.cdw10 & 0xff;
    uint32_t result = 0;
    
    switch (fid) {
        case FEATURE_CONTROLLER_RESET:
            break;

        case FEATURE_NUMBER_OF_QUEUES: {
            uint16_t NSQR = cmd.cdw11 & 0xffff;
            uint16_t NCQR = (cmd.cdw11 >> 16) & 0xffff;

            /* 예시: 1개의 Submission/Completion 큐만 지원하는 경우 */
            NSQR = 0x0;  // 0은 1개 큐 의미
            NCQR = 0x0;
            result = ((uint32_t)NCQR << 16) | NSQR;
            resp.psh.sf = result;
            break;
        }

        case FEATURE_ASYNC_EVENT_CONFIG: {
            uint32_t AEC = cmd.cdw11;  // Async Event Config 값
            result = AEC;
            resp.psh.sf = result;
            break;
        }

        default:
            resp.psh.sf = 1 << 15 | (SCT_GENERIC & 0x7) << 9 | (SC_INVALID_FIELD & 0xFF) << 1;
            break;
    }
    return resp;
}

void process_admin_queue(
    hls::stream<ap_uint<DATA_WIDTH>> &admin_queue_rxdata, // receive
    hls::stream<ap_uint<DATA_WIDTH>> &out_stream
){
    #pragma HLS INLINE off
    enum admin_state_e {
        WAIT_CH,
        WAIT_PSH,
        PROCESS_ICREQ,
        PROCESS_CONNECT_CMD,
        PROCESS_CONNECT_DATA,
        SEND_CONNECT_RESP,
        PROCESS_ADMIN_CMD
    };
    static admin_state_e state = WAIT_CH;
    static uint32_t admin_status_info = NO_CONNECTION; 
    static struct packet_header packet_hdr;
    static struct nvme_fabrics_cmd cmd;
    static struct nvme_properties props = {
        .cap  = ((uint64_t)1 << 37) | (4 << 24) | (1 << 16) | 127,
        .vs   = 0x10400,
        .cc   = 0x460001,
        .csts = 0,
    };
    static ap_uint<DATA_WIDTH> hdr_packet = 0;
    static ap_uint<DATA_WIDTH> psh_packet = 0;
    static ap_uint<DATA_WIDTH> data_packet = 0;
    static ap_uint<DATA_WIDTH> resp_packet = 0;
    static uint32_t left_bytes = 0;
    static uint32_t left_packets = 0;
    static uint32_t compare_flag = 0;
    static uint32_t compare_index;
    static struct icresp_t icresp;
    static struct cmd_resp_t cmd_resp;
    struct c2hdata_hdr c2hdata_format;
    switch (state)
    {
    case WAIT_CH: // read CH
        if (!admin_queue_rxdata.empty()) {
            //hls::print("<STATE> ADMIN:WAIT_CH\n");
            hdr_packet = admin_queue_rxdata.read();
            packet_hdr = ap_uint_to_packet_header(hdr_packet);
            state = WAIT_PSH;
        }
    break;
    case WAIT_PSH:
        if (!admin_queue_rxdata.empty()) {
            psh_packet = admin_queue_rxdata.read();
            left_bytes = packet_hdr.pdu_hdr.plen - packet_hdr.pdu_hdr.hlen;
            cmd = ap_uint_to_nvme_cmd(psh_packet);
            //hls::print("<STATE> ADMIN:WAIT_PSH: Read PSH complete\n");
            //hls::print("\t ADMIN: current status: %u\n", admin_status_info);

            if(admin_status_info == NO_CONNECTION){
                state = PROCESS_ICREQ;
            }
            else if(admin_status_info == PDU_LEVEL_CONNECTION){
                state = PROCESS_CONNECT_CMD;
            }
            else{
                state = PROCESS_ADMIN_CMD;
            }
        }
    break;
    case PROCESS_ICREQ:
        if(packet_hdr.pdu_hdr.type == PDU_TYPE_ICREQ){
            //hls::print("<STATE> ADMIN:PROCESS_ICREQ\n");
            admin_status_info = PDU_LEVEL_CONNECTION;
            icresp = icresp_format;
            resp_packet(DATA_WIDTH - 1, 0) = icresp_to_ap_uint(icresp);
            out_stream.write(resp_packet); // 0 ~ 63B
            out_stream.write(0x0); // 64 ~ 127B
            //hls::print("\t Admin status updated: %u\n", admin_status_info);
            state = WAIT_CH;
        }
        else{
            state = WAIT_CH;
        }
    break;
    case PROCESS_CONNECT_CMD: // Parse the header
        //hls::print("<STATE> ADMIN:PROCESS_CONNECT_CMD\n");
        if(packet_hdr.pdu_hdr.type == PDU_TYPE_CMD){
                if (cmd.opcode == OPC_FABRICS && cmd.fctype == FCTYPE_CONNECT) {
                    state = PROCESS_CONNECT_DATA;
                    left_packets = left_bytes / 64;
                    compare_index = 0;
                    compare_flag = 1;
                    
                }
                else{
                    cmd_resp = cmd_resp_format;
                    cmd_resp.psh.sf = 1 << 15 | (SCT_GENERIC & 0x7) << 9 | (SC_COMMAND_SEQ & 0xFF) << 1;
                    resp_packet(CMD_RESP_LEN * 8 - 1, 0) = cmd_resp_to_ap_uint(cmd_resp);
                    out_stream.write(resp_packet);
                    state = WAIT_CH;
                }
        }
        else{
            state = WAIT_CH;
        }
    break;
    case PROCESS_CONNECT_DATA: // DATA_WIDTH : 512b(64B)
        //hls::print("<STATE> ADMIN:PROCESS_CONNECT_DATA\n");
        if(compare_index == left_packets){
            state = SEND_CONNECT_RESP;
        }
        else{ 
            if(!admin_queue_rxdata.empty()){
                data_packet = admin_queue_rxdata.read();
                compare_flag = compare_flag & compare_subsys_nqn(data_packet, compare_index); // true? false?
                compare_index++;
            }
        }

    break;
    case SEND_CONNECT_RESP:
        //hls::print("<STATE> ADMIN:SEND_CONNECT_RESP\n"); 
        if(compare_flag == 1){
            cmd_resp = cmd_resp_format;
            cmd_resp.psh.cid = cmd.cid;
            resp_packet(CMD_RESP_LEN * 8 - 1, 0) = cmd_resp_to_ap_uint(cmd_resp);
            out_stream.write(resp_packet);
            //hls::print("\t Admin status current: %u\n", admin_status_info);
            admin_status_info = ADMIN_CONNECTION;
            //hls::print("\t Admin status updated: %u\n", admin_status_info);
        }
        else{
            cmd_resp = cmd_resp_format;
            cmd_resp.psh.sf = 1 << 15 | (SCT_CMD_SPEC & 0x7) << 9 | (SC_CONNECT_INVALID & 0xFF) << 1;
            resp_packet(CMD_RESP_LEN * 8 - 1, 0) = cmd_resp_to_ap_uint(cmd_resp);
            out_stream.write(resp_packet);
        }
        state = WAIT_CH;
    break;
    case PROCESS_ADMIN_CMD:
        switch(cmd.opcode){ // Not implemented
            case OPC_FABRICS:
                cmd_resp = fabrics_cmd(cmd, props);
                resp_packet = 0;
                resp_packet(CMD_RESP_LEN * 8 - 1, 0) = cmd_resp_to_ap_uint(cmd_resp);
                out_stream.write(resp_packet);
                state = WAIT_CH;
                break;
            case OPC_IDENTIFY:
                switch (cmd.cdw10) {
                    case CNS_ID_NS: 
                        if(cmd.fctype == 1){
                            // Write C2HDATA HDR
                            c2hdata_format = c2hdata_hdr_4096B_const;
                            c2hdata_format.psh.cid = cmd.cid;
                            resp_packet = 0;
                            resp_packet(223,0) = c2hdata_hdr_to_ap_uint(c2hdata_format);
                            out_stream.write(resp_packet);
                            // Write C2HDATA DATA
                            for(int i = 0; i< 64; i++){
                                    resp_packet = ap_uint_8_to_512(nvme_id_ns_const_packet[i]);
                                    out_stream.write(resp_packet);
                            }
                            // Write CapsuleResp
                            cmd_resp = cmd_resp_format;
                            cmd_resp.psh.cid = cmd.cid;
                            resp_packet = 0;
                            resp_packet(CMD_RESP_LEN * 8 - 1, 0) = cmd_resp_to_ap_uint(cmd_resp);
                            out_stream.write(resp_packet);
                        }
                        else{
                            cmd_resp = cmd_resp_format;
                            cmd_resp.psh.cid = cmd.cid;
                            cmd_resp.psh.sf = 1 << 15 | (SCT_GENERIC & 0x7) << 9 | (SC_INVALID_FIELD & 0xFF) << 1;
                            resp_packet = 0;
                            resp_packet(CMD_RESP_LEN * 8 - 1, 0) = cmd_resp_to_ap_uint(cmd_resp);
                            out_stream.write(resp_packet);
                        }
                        state = WAIT_CH;
                    break;
                    case CNS_ID_CTRL: 
                        // Write C2HDATA HDR
                        c2hdata_format = c2hdata_hdr_4096B_const;
                        c2hdata_format.psh.cid = cmd.cid;
                        resp_packet = 0;
                        resp_packet(223,0) = c2hdata_hdr_to_ap_uint(c2hdata_format);
                        out_stream.write(resp_packet);
                        // Write C2HDATA DATA
                        for(int i = 0; i< 64; i++){
                            resp_packet = ap_uint_8_to_512(nvme_id_ctrl_const_packet[i]);
                            out_stream.write(resp_packet);
                        }
                        // Write CapsuleResp
                        cmd_resp = cmd_resp_format;
                        cmd_resp.psh.cid = cmd.cid;
                        resp_packet = 0;
                        resp_packet(CMD_RESP_LEN * 8 - 1, 0) = cmd_resp_to_ap_uint(cmd_resp);
                        out_stream.write(resp_packet);
                        state = WAIT_CH;

                    break;

                    case CNS_ID_ACTIVE_NSID:
                        c2hdata_format = c2hdata_hdr_4096B_const;
                        c2hdata_format.psh.cid = cmd.cid;
                        resp_packet = 0;
                        resp_packet(223,0) = c2hdata_hdr_to_ap_uint(c2hdata_format);
                        out_stream.write(resp_packet);
                        // Write C2HDATA DATA
                        for(int i = 0; i< 64; i++){
                            resp_packet = ap_uint_8_to_512(nvme_id_active_nsid_const_packet[i]);
                            out_stream.write(resp_packet);
                        }

                        // Write CapsuleResp
                        cmd_resp = cmd_resp_format;
                        cmd_resp.psh.cid = cmd.cid;
                        resp_packet = 0;
                        resp_packet(CMD_RESP_LEN * 8 - 1, 0) = cmd_resp_to_ap_uint(cmd_resp);
                        out_stream.write(resp_packet);
                        state = WAIT_CH;


                    break;
                    case CNS_ID_NS_LIST:
                        c2hdata_format = c2hdata_hdr_20B_const;
                        c2hdata_format.psh.cid = cmd.cid;
                        // Write C2HDATA Header
                        resp_packet = 0;
                        resp_packet(223,0) = c2hdata_hdr_to_ap_uint(c2hdata_format);
                        out_stream.write(resp_packet);
                        // Write C2HDATA 
                        resp_packet = 0;
                        resp_packet(159,0) = ap_uint_8_to_160(id_ns_desc_const_packet);
                        out_stream.write(resp_packet);
                        //Write CapsuleResp
                        cmd_resp = cmd_resp_format;
                        cmd_resp.psh.cid = cmd.cid;
                        resp_packet = 0;
                        resp_packet(CMD_RESP_LEN * 8 - 1, 0) = cmd_resp_to_ap_uint(cmd_resp);
                        out_stream.write(resp_packet);
                        state = WAIT_CH;

                    break;
                }
            break;
            // case OPC_GET_LOG:
                // discovery_get_log(admin_queue_rxdata, left_bytes, cmd, out_stream);
                // break;
            case OPC_SET_FEATURES:
                cmd_resp = admin_set_features(cmd);
                resp_packet(CMD_RESP_LEN * 8 - 1, 0) = cmd_resp_to_ap_uint(cmd_resp);
                out_stream.write(resp_packet);
                state = WAIT_CH;
            break;                
            case OPC_KEEP_ALIVE:
                cmd_resp = cmd_resp_format;
                cmd_resp.psh.cid = cmd.cid;    
                resp_packet(CMD_RESP_LEN * 8 - 1, 0) = cmd_resp_to_ap_uint(cmd_resp);
                out_stream.write(resp_packet);    
                state = WAIT_CH; 
            break;
            default:
                cmd_resp = cmd_resp_format;
                cmd_resp.psh.cid = cmd.cid;
                cmd_resp.psh.sf = 1 << 15 | (SCT_GENERIC & 0x7) << 9 | (SC_INVALID_OPCODE & 0xFF) << 1;
                resp_packet(CMD_RESP_LEN * 8 - 1, 0) = cmd_resp_to_ap_uint(cmd_resp);
                out_stream.write(resp_packet);  
                state = WAIT_CH;              
            break;
        }
    break;
    }
}