#include "nvme_driver_top.hpp"


void nvme_process_cpl(
    nvme_cqe_t *io_cq_base_addr,
    uint32_t *dbl_base_address,
    uint32_t &completed_requests_count,  
    uint32_t &completed_requests_bytes,
    hls::stream<struct mgmt_table_req> &cpl_in_req,
    hls::stream<struct mgmt_table_resp> &cpl_out_resp
) 
{
    // #pragma HLS PIPELINE II=1
    enum class cq_state {IDLE, READ_ENTRY, UPDATE_HEAD, COMPLETION};

    // static uint32_t cq_tail, // CQ Tail 
    static uint32_t cq_head = 0; // CQ Head
    static uint8_t cq_phase = 0; // CQ Phase   
    static cq_state state = cq_state::IDLE; 
    static uint32_t delay_counter = 0;
    
    static nvme_cqe_t cqe;

    switch (state) {
        case cq_state::IDLE:
                if (delay_counter < DELAY_CYCLES) {
                    delay_counter++;
                }
                else {
                    delay_counter = 0;
                    state = cq_state::READ_ENTRY;
                }
            break;

        case cq_state::READ_ENTRY:
            cqe = io_cq_base_addr[cq_head];
            // Check phase bit
            if (cqe.dw3.p != cq_phase) {
                state = cq_state::IDLE;
                break;
            }
            state = cq_state::UPDATE_HEAD;
            break;

        case cq_state::UPDATE_HEAD:
            cq_head = cq_head + 1;
            if (cq_head == IO_QUEUE_MAX_DEPTH) {
                cq_head = 0;
                cq_phase = !cq_phase;
            }
            state = cq_state::COMPLETION;
            break;
        
        case cq_state::COMPLETION:
            dbl_base_address[3] = cq_head;
            completed_requests_count++;

            // TODO: Update completed_requests_bytes based on ID management table
            struct mgmt_table_req req;
            struct mgmt_table_resp resp;

            req.op = 2;
            req.cid = cqe.cid;
            
            cpl_in_req.write(req);
            resp = cpl_out_resp.read();

            completed_requests_bytes += (resp.rdata.nlb + 1) * 4096;
            
            state = cq_state::IDLE;
            break;
    } 
}