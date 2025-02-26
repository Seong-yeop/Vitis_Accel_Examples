#include <iostream>
#include <hls_stream.h>
#include <cstdint>

#include "nvme_driver_top.hpp"


int main() {
    struct nvme_cqe cqe[IO_QUEUE_MAX_DEPTH] = {0};
    struct nvme_seq seq[IO_QUEUE_MAX_DEPTH] = {0};

    uint32_t cq_head = 0;
    uint32_t cq_tail = 0;
    uint32_t sq_head = 0;
    uint32_t sq_tail = 0;
    uint8_t stop = 0;
    
    uint32_t cq_id = 0;
    uint32_t cq_size = IO_QUEUE_MAX_DEPTH;

    uint32_t dbl[100];


    return 0;
}