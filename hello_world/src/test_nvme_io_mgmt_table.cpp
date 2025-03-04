#include <iostream>
#include <hls_stream.h>
#include <ap_int.h>
#include <stdint.h>

#include "nvme_driver_top.hpp"


int main() {
    hls::stream<mgmt_table_req>  req_stream;
    hls::stream<mgmt_table_resp> resp_stream;

    mgmt_table_req req;
    mgmt_table_resp resp;

    for (int i = 0; i < 5; i++) {
        req.we = true;
        req.cid = i;
        req.wdata.valid = 1;
        req.wdata.cid   = i;
        req.wdata.qid   = 10 + i;
        req.wdata.opc   = 0x01 + i;
        req.wdata.slba  = 0x1000 + i * 512;
        req.wdata.nlb   = 8 + i;
        req.wdata.prp1  = 0xdeadbeef0000 + i;

        req_stream.write(req);
        mgmt_table(req_stream, resp_stream);

        if (!resp_stream.empty()) {
            resp = resp_stream.read();
            std::cout << "WRITE [CID=" << i << "] Status=" << (int)resp.status;
            if (resp.status == 0) {
                std::cout << " | SLBA=0x" << std::hex << resp.rdata.slba
                          << " | PRP1=0x" << resp.rdata.prp1 << std::dec << std::endl;
            } else {
                std::cout << " (FAILED)" << std::endl;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        req.we = false;
        req.cid = i;

        req_stream.write(req);
        mgmt_table(req_stream, resp_stream);

        if (!resp_stream.empty()) {
            resp = resp_stream.read();
            std::cout << "READ  [CID=" << i << "] Status=" << (int)resp.status;
            if (resp.status == 0 && resp.rdata.valid) {
                std::cout << " | QID=" << resp.rdata.qid
                          << " | OPC=0x" << std::hex << (int)resp.rdata.opc
                          << " | SLBA=0x" << resp.rdata.slba
                          << " | NLB=" << std::dec << resp.rdata.nlb << std::endl;
            } else {
                std::cout << " (NOT FOUND)" << std::endl;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        req.we = false;
        req.cid = i;

        req_stream.write(req);
        mgmt_table(req_stream, resp_stream);

        if (!resp_stream.empty()) {
            resp = resp_stream.read();
            std::cout << "VERIFY DELETE [CID=" << i << "] Status=" << (int)resp.status;
            if (resp.status == 0 && resp.rdata.valid) {
                std::cout << " ERROR: Entry still exists!" << std::endl;
            } else {
                std::cout << " Successfully Deleted." << std::endl;
            }
        }
    }

    return 0;
}
