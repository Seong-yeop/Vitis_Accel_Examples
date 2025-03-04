
#include <hls_stream.h>
#include <ap_int.h>
#include <stdint.h>

#include "nvme_driver_top.hpp"


inline int find_entry(ap_uint<16> cid, struct cmd_info tbl[MAX_CMD_INFO_TBL_SIZE]) {
    #pragma HLS INLINE
    #pragma HLS PIPELINE II=1
    int result = -1;
    for (int i = 0; i < MAX_CMD_INFO_TBL_SIZE; i++) {
        #pragma HLS UNROLL factor=4
        if (tbl[i].valid && tbl[i].cid == cid) {
            result = i;
        }
    }
    return result;
}

inline int find_empty_slot(struct cmd_info tbl[MAX_CMD_INFO_TBL_SIZE]) {
    #pragma HLS INLINE
    #pragma HLS PIPELINE II=1
    for (int i = 0; i < MAX_CMD_INFO_TBL_SIZE; i++) {
        #pragma HLS UNROLL factor=4
        if (!tbl[i].valid)
            return i;
    }
    return -1;
}

int cmd_info_register(struct cmd_info new_entry, struct cmd_info tbl[MAX_CMD_INFO_TBL_SIZE]) {
    int idx = find_empty_slot(tbl);
    if (idx != -1) {
        tbl[idx] = new_entry;
        tbl[idx].valid = true;
        return idx;
    }
    return idx;
}

int cmd_info_query(ap_uint<16> cid, struct cmd_info tbl[MAX_CMD_INFO_TBL_SIZE]) {
    int idx = find_entry(cid, tbl);
    // if (idx != -1) {
    //     tbl[idx].valid = false;
    // }
    return idx;
}

int cmd_info_clear(ap_uint<16> done_cid, struct cmd_info tbl[MAX_CMD_INFO_TBL_SIZE]) {
    int idx = find_entry(done_cid, tbl);
    if (idx != -1) {
        tbl[idx].valid = false;
    }
    return inx;
}

void mgmt_table(
    hls::stream<mgmt_table_req>  &submit_in_req,
    hls::stream<mgmt_table_resp> &submit_out_resp
    hls::stream<mgmt_table_req>  &cpl_in_req,
    hls::stream<mgmt_table_resp> &cpl_out_resp
) {
    #pragma HLS PIPELINE II=1

    static cmd_info cmd_info_tbl[MAX_CMD_INFO_TBL_SIZE];
    #pragma HLS bind_storage variable=cmd_info_tbl type=RAM_2P impl=BRAM
    #pragma HLS DEPENDENCE variable=cmd_info_tbl inter false

    struct mgmt_table_req  rep;
    struct mgmt_table_resp resp;

    // Submit request 
    if (!submit_in_req.empty() && !submit_out_resp.full()) 
    {
        req  = submit_in_req.read();
        if (rep.op == 1) {
            int idx = cmd_info_register(req.wdata, cmd_info_tbl);
            if (idx != -1) {
                resp.rdata = cmd_info_tbl[idx];
                resp.status = 0;
            } else {
                resp.rdata = cmd_info();
                resp.status = -1;
            }
        }
        submit_out_resp.write(resp);
    }
    // Completion request
    else if (!cpl_in_req.empty() && !cpl_out_resp.full()) 
    {
        req  = cpl_in_req.read();
        if (rep.op == 2) 
        {
            int idx = cmd_info_clear(req.cid, cmd_info_tbl);
            if (idx != -1) {
                resp.rdata = cmd_info_tbl[idx];
                resp.status = 0;
            } else {
                resp.rdata = cmd_info();
                resp.status = -1;
            }
        }
        cpl_out_resp.write(resp);
    }

    // if (!in_req.empty() && !out_resp.full()) {
    //     struct mgmt_table_req  req  = in_req.read();
    //     struct mgmt_table_resp resp;

    //     switch (rep.op) {
    //         case 0: // read
    //             int idx = cmd_info_query(req.cid, cmd_info_tbl);
    //             if (idx != -1) {
    //                 resp.rdata = cmd_info_tbl[idx];
    //                 resp.status = 0;
    //             } else {
    //                 resp.rdata = cmd_info();
    //                 resp.status = -1;
    //             }
    //             break;
    //         case 1: // write
    //             int idx = cmd_info_register(req.wdata, cmd_info_tbl);
    //             if (idx != -1) {
    //                 resp.rdata = cmd_info_tbl[idx];
    //                 resp.status = 0;
    //             } else {
    //                 resp.rdata = cmd_info();
    //                 resp.status = -1;
    //             }
    //             break;
    //         case 2: // clear
    //             int idx = cmd_info_clear(req.cid, cmd_info_tbl);
    //             if (idx != -1) {
    //                 resp.rdata = cmd_info_tbl[idx];
    //                 resp.status = 0;
    //             } else {
    //                 resp.rdata = cmd_info();
    //                 resp.status = -1;
    //             }
    //             break;
    //     }
    //     out_resp.write(resp);
    // }
}