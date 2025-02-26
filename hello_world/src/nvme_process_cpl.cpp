#include "nvme_driver_top.hpp"


void nvme_process_cpl(
    nvmd_cqe_t *io_cq_base_addr,
    uint32_t *dbl_base_address,
    
    uint32_t &completed_requests_count,  
    uint32_t &completed_requests_bytes
) 
{
    // #pragma HLS PIPELINE II=1
    enum class cq_state {IDLE, READ_ENTRY, UPDATE_HEAD, COMPLETION};

    // static uint32_t cq_tail, // CQ Tail 
    static uint32_t cq_head = 0; // CQ Head
    static uint8_t cq_phase = 0; // CQ Phase   
    static cq_state state = cq_state::IDLE; 
    static uint32_t delay_counter = 0;
    
    static struct nvme_cqe_t cqe;

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
            if (cq_head == cq_size) {
                cq_head = 0;
                cq_phase = !cq_phase;
            }
            state = cq_state::COMPLETION;
            break;
        
        case cq_state::COMPLETION:
            nvme_cqh_dbl[0] = cq_head;
            completed_requests_count++;
            // TODO: Update completed_requests_bytes based on ID management table
            state = cq_state::IDLE;
            break;
    } 
}