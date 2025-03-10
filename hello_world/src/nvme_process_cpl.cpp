#include "nvme_driver_top.hpp"


#define DELAY_CYCLES 3000000

void nvme_process_cpl(
    nvme_cqe_t *io_cq_base_addr,
    uint32_t *dbl_base_address,
    uint64_t &completed_requests_count,  
    uint64_t &completed_requests_bytes,
    hls::stream<struct mgmt_table_req> &cpl_in_req,
    hls::stream<struct mgmt_table_resp> &cpl_out_resp,
    hls::stream<uint32_t> &nvme_cq_polling_stream,
    hls::stream<uint32_t> &sq_head_stream_write,
    uint32_t delay_cycles
) 
{
    #pragma HLS INTERFACE mode=m_axi port=dbl_base_address depth=512

    // #pragma HLS PIPELINE II=1
    enum class cq_state {IDLE, READ_ENTRY, DELAY, UPDATE_SQ_HEAD, UPDATE_CQ_HEAD, COMPLETION, WAIT_RESP, SEND_REQ};

    static uint32_t cq_head = 0; // CQ Head
    static uint8_t cq_phase = 0; // CQ Phase   
    static cq_state state = cq_state::IDLE; 
    static uint32_t delay_counter = 0;
    
    static nvme_cqe_t cqe;
    static struct mgmt_table_req req;
    static struct mgmt_table_resp resp;

    static uint64_t _completed_requests_bytes = 0;
    static uint64_t _completed_requests_count = 0;

    static uint32_t current_delay_cycles = DELAY_CYCLES;
    current_delay_cycles = delay_cycles;


    switch (state) {
        case cq_state::IDLE:
                if (!nvme_cq_polling_stream.empty()) {
                    nvme_cq_polling_stream.read();
                    state = cq_state::READ_ENTRY;
                }
            break;

        case cq_state::READ_ENTRY:
            cqe = io_cq_base_addr[cq_head];
            // Check phase bit
            if (cqe.phase_tag == cq_phase) {
                state = cq_state::DELAY;
                break;
            }
            state = cq_state::UPDATE_CQ_HEAD;
            break;

        case cq_state::DELAY:
            delay_counter++;
            if (delay_counter == current_delay_cycles) {
                delay_counter = 0;
                state = cq_state::READ_ENTRY;
            }
            break;
    
        case cq_state::UPDATE_CQ_HEAD:
            cq_head = cq_head + 1;
            if (cq_head == IO_QUEUE_MAX_DEPTH) {
                cq_head = 0;
                cq_phase = !cq_phase;
            }
            state = cq_state::COMPLETION;
            break;
    
        case cq_state::COMPLETION:
            dbl_base_address[3] = cq_head;
            _completed_requests_count++;
            completed_requests_count = _completed_requests_count;
            state = cq_state::UPDATE_SQ_HEAD;
            break;
        
        case cq_state::UPDATE_SQ_HEAD:
            if (!sq_head_stream_write.full()) {
                sq_head_stream_write.write(cqe.sq_head);
                state = cq_state::SEND_REQ;
            }
            break;
            
        case cq_state::SEND_REQ:
            if (!cpl_in_req.full()) {
                req.op = 2;
                req.cid = cqe.cid;
                cpl_in_req.write(req);
                state = cq_state::WAIT_RESP;
            }
            break;
        
        case cq_state::WAIT_RESP:
            if (!cpl_out_resp.empty()) {
                resp = cpl_out_resp.read();
                _completed_requests_bytes += (resp.rdata.nlb + 1) * 4096;
                completed_requests_bytes = _completed_requests_bytes;
                state = cq_state::IDLE;
            }
            break;
    } 
}