
#include <hls_stream.h>
#include <ap_int.h>
#include <stdint.h>

#include "nvme_driver_top.hpp"

// int find_entry(ap_uint<16> cid, struct cmd_info tbl[MAX_CMD_INFO_TBL_SIZE]) {
//     #pragma HLS PIPELINE II=1
//     for (int i = 0; i < MAX_CMD_INFO_TBL_SIZE; i++) {
//         if (tbl[i].valid && tbl[i].cid == cid) {
//             return i;
//         }
//     }
//     return -1;
// }

// int find_empty_slot(struct cmd_info tbl[MAX_CMD_INFO_TBL_SIZE]) {
//     #pragma HLS PIPELINE II=1
//     for (int i = 0; i < MAX_CMD_INFO_TBL_SIZE; i++) {
//         if (!tbl[i].valid)
//             return i;
//     }
//     return -1;
// }


// void mgmt_table(
//     hls::stream<struct mgmt_table_req>  &submit_in_req,
//     hls::stream<struct mgmt_table_resp> &submit_out_resp,
//     hls::stream<struct mgmt_table_req>  &cpl_in_req,
//     hls::stream<struct mgmt_table_resp> &cpl_out_resp
// ) {
//     #pragma HLS DATAFLOW disable_start_propagation
//     #pragma HLS INTERFACE ap_ctrl_none port=return
//     static cmd_info cmd_info_tbl[MAX_CMD_INFO_TBL_SIZE];
//     #pragma HLS bind_storage variable=cmd_info_tbl type=RAM_2P impl=BRAM
//     // #pragma HLS ARRAY_PARTITION variable=cmd_info_tbl cyclic factor=32
//     // #pragma HLS DEPENDENCE variable=cmd_info_tbl inter false
//     // #pragma HLS DEPENDENCE variable=cmd_info_tbl intra false

//     if (!submit_in_req.empty() && !submit_out_resp.full()) {
//         struct mgmt_table_req req = submit_in_req.read();
//         struct mgmt_table_resp resp;
//         int idx = cmd_info_register(req.wdata, cmd_info_tbl);
//         resp.rdata = (idx != -1) ? cmd_info_tbl[idx] : cmd_info();
//         resp.status = (idx != -1) ? 0 : -1;
//         submit_out_resp.write(resp);
//     }
//     if (!cpl_in_req.empty() && !cpl_out_resp.full()) {
//         struct mgmt_table_req req = cpl_in_req.read();
//         struct mgmt_table_resp resp;
//         int idx = cmd_info_clear(req.cid, cmd_info_tbl);
//         resp.rdata = (idx != -1) ? cmd_info_tbl[idx] : cmd_info();
//         resp.status = (idx != -1) ? 0 : -1;
//         cpl_out_resp.write(resp);
//     }
// }

// /**
//  * cmd_info_register: cmd_info를 등록
//  *  - cid를 이용해 find_empty_slot으로 바로 idx를 구해본다.
//  */
// int cmd_info_register(struct cmd_info new_entry, struct cmd_info tbl[MAX_CMD_INFO_TBL_SIZE]) {
//     // int idx = find_empty_slot(new_entry.cid, tbl);
//     return idx;  // -1이면 이미 자리가 차 있음을 의미
// }

// /**
//  * cmd_info_clear: cid로부터 idx를 구해서 valid를 해제
//  */
// int cmd_info_clear(ap_uint<16> done_cid, struct cmd_info tbl[MAX_CMD_INFO_TBL_SIZE]) {
//     int idx = find_entry(done_cid, tbl);
//     if (idx != -1) {
//         tbl[idx].valid = false;
//     }
//     return idx;
// }


// int find_entry(ap_uint<16> cid, struct cmd_info tbl[MAX_CMD_INFO_TBL_SIZE]) {
//     #pragma HLS PIPELINE II=1
//     int idx = cid % IO_QUEUE_MAX_DEPTH;
//     if (tbl[idx].valid && tbl[idx].cid == cid) {
//         return idx;
//     }
//     return -1;
// }

// int find_empty_slot(ap_uint<16> cid, struct cmd_info tbl[MAX_CMD_INFO_TBL_SIZE]) {
//     #pragma HLS PIPELINE II=1
//     int idx = cid % IO_QUEUE_MAX_DEPTH;
//     if (!tbl[idx].valid) {
//         return idx;
//     }
//     return -1;
// }


void mgmt_table(
    hls::stream<struct mgmt_table_req>  &submit_in_req,
    hls::stream<struct mgmt_table_resp> &submit_out_resp,
    hls::stream<struct mgmt_table_req>  &cpl_in_req,
    hls::stream<struct mgmt_table_resp> &cpl_out_resp,
    hls::stream<uint32_t> &sq_head_stream_write,
    hls::stream<uint32_t> &sq_head_stream_read
) {
    #pragma HLS INLINE off
    #pragma HLS PIPELINE II=1
    
    static uint32_t sq_head = 0;
    static uint32_t pre_sq_head = 0;
    static cmd_info cmd_info_tbl[MAX_CMD_INFO_TBL_SIZE];
    #pragma HLS bind_storage variable=cmd_info_tbl type=RAM_T2P impl=BRAM

    if (!submit_in_req.empty() && !submit_out_resp.full()) {
        struct mgmt_table_req req = submit_in_req.read();
        struct mgmt_table_resp resp;
        cmd_info_tbl[req.cid % IO_QUEUE_MAX_DEPTH] = req.wdata;
        resp.rdata = cmd_info();
        resp.status = 0;
        submit_out_resp.write(resp);
    }

    if (!cpl_in_req.empty()) {
        struct mgmt_table_req req = cpl_in_req.read();
        struct mgmt_table_resp resp;
        resp.rdata = cmd_info_tbl[req.cid % IO_QUEUE_MAX_DEPTH];
        resp.status = 0;
        cpl_out_resp.write(resp);
    }

    // Read sq_head from CQE
    if (!sq_head_stream_write.empty()) {
        sq_head = sq_head_stream_write.read();
    }

    // Update sq_head for nvme_submit_cmd module
    if (sq_head != pre_sq_head && !sq_head_stream_read.full()) {
        sq_head_stream_read.write(sq_head);
        pre_sq_head = sq_head;
    }
}
