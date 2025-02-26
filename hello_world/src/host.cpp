#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>


#include <xrt/xrt_device.h>
#include <xrt/xrt_kernel.h>
#include <xrt/xrt_bo.h>
#include <experimental/xrt_ip.h>

#include "cmdlineparser.h"
// #include "xcl2.hpp"

#define IOCTL_GET_PHYS_ADDR _IOR('h', 1, unsigned long)


#define HUGEPAGE_SIZE (2 * 1024 * 1024)
#define HUGEPAGE_FILE "/sys/kernel/hugepage_info/hugepage_phys"
#define NVME_RESOURCE_FILE "/sys/bus/pci/devices/0000:18:00.0/resource"

#define SSD_ADMIN_SQ_PHYS_BASE(ssd_id) ((queue_phys_base) + 0x2000 * (ssd_id))
#define SSD_ADMIN_CQ_PHYS_BASE(ssd_id) ((queue_phys_base) + 0x2000 * (ssd_id) + 0x1000)
#define SSD_ADMIN_SQ_VIRT_BASE(ssd_id) ((queue_virt_base) + 0x2000 * (ssd_id))
#define SSD_ADMIN_CQ_VIRT_BASE(ssd_id) ((queue_virt_base) + 0x2000 * (ssd_id) + 0x1000)

// admin 큐가 0x2000 바이트를 차지한다고 가정
// admin 큐는 0x2000 바이트를 차지하고, 각 큐는 0x1000 바이트를 차지한다. 
#define SSD_IO_QUEUE_OFFSET 0x2000
#define SSD_IO_SQ_PHYS_BASE(ssd_id, qid) \
    ((queue_phys_base) + SSD_IO_QUEUE_OFFSET + (((qid) - 1) << queue_low_bit) + ((ssd_id) << ssd_low_bit) + (0x0 << ram_type_bit))
#define SSD_IO_CQ_PHYS_BASE(ssd_id, qid) \
    ((queue_phys_base) + SSD_IO_QUEUE_OFFSET + (((qid) - 1) << queue_low_bit) + ((ssd_id) << ssd_low_bit) + (0x1 << ram_type_bit))
#define SSD_IO_SQ_VIRT_BASE(ssd_id, qid) \
    ((queue_virt_base) + SSD_IO_QUEUE_OFFSET + (((qid) - 1) << queue_low_bit) + ((ssd_id) << ssd_low_bit) + (0x0 << ram_type_bit))
#define SSD_IO_CQ_VIRT_BASE(ssd_id, qid) \
    ((queue_virt_base) + SSD_IO_QUEUE_OFFSET + (((qid) - 1) << queue_low_bit) + ((ssd_id) << ssd_low_bit) + (0x1 << ram_type_bit))

#define FPGA_HOST_MEM_BASE 0x2000000000
#define FPGA_SSD_BAR_BASE FPGA_HOST_MEM_BASE
#define FPGA_SQ_DOORBELL_BASE (FPGA_SSD_BAR_BASE + 0x1000)

#define FPGA_HUGEPG_BASE (FPGA_HOST_MEM_BASE + 32768)
#define FPGA_IO_SQ_BASE (FPGA_HUGEPG_BASE + 0x2000)
#define FPGA_IO_CQ_BASE (FPGA_HUGEPG_BASE + 0x3000)

#define FPGA_PRP_BASE (FPGA_HUGEPG_BASE + 0x10000)

#define FPGA_EMPTY_MEM (FPGA_HOST_MEM_BASE + 32768 + 0x5000)

#define NVME_ADMIN_SUBMISSION_QUEUE_BASE_ADDR_OFFSET 0x28
#define MAP_SIZE (1<<16)      
#define MAP_MASK (MAP_SIZE - 1)

#define ADMIN_QUEUE_DEPTH 0x1F
#define IO_QUEUE_DEPTH 32

volatile uint32_t admin_sq_tl[32];
volatile uint32_t admin_cq_hd[32];
uint8_t phase_bit[32];
uint16_t command_id[32];
volatile uint64_t ssd_virt_base[32];


// [ssd_id][qid]
uint16_t io_sq_tail[32][256];
uint16_t io_cq_head[32][256];
uint8_t  io_phase_bit[32][256];
uint16_t io_command_id[32][256];


uint64_t queue_phys_base;
uint64_t queue_virt_base;

uint64_t queue_low_bit = 12;
uint64_t ssd_low_bit = 12;
uint64_t ram_type_bit = 12;


void print_bar(int offset, int length) {
    printf("=============================\n");

    uint64_t *bar = (uint64_t *)(ssd_virt_base[0]+ offset);
    for (int i = 0; i < length; i++) {
        printf("BAR[%d]: 0x%016lx\n", offset+i, bar[i]);
    }
    printf("=============================\n");
}

void print_sq(int offset, int length) {
    printf("=============================\n");

    uint32_t *sq = (uint32_t *)(SSD_ADMIN_SQ_VIRT_BASE(0));
    for (int i = 0; i < length; i++) {
        printf("SQ[%d]: 0x%08x\n", offset+i, sq[i]);
    }
    printf("=============================\n");
}

void print_cq(int offset, int length) {
    printf("=============================\n");

    uint32_t *cq = (uint32_t *)(SSD_ADMIN_CQ_VIRT_BASE(0));
    for (int i = 0; i < length; i++) {
        printf("CQ[%d]: 0x%08x\n", offset+i, cq[i]);
    }
    printf("=============================\n");
}

void print_io_sq(int offset, int length) {

    uint32_t *sq = (uint32_t *)(SSD_IO_SQ_VIRT_BASE(0, 1));

    printf("========== PRINT IO SQ (cmd offset=%d, count=%d) ==========\n", offset, length);

    for (int cmd_i = 0; cmd_i < length; cmd_i++)
    {
        // 명령어 하나당 16 DW
        int base_idx = (offset + cmd_i) * 16;
        printf("----- SQ Command %d (DW: %d ~ %d) -----\n", offset + cmd_i, base_idx, base_idx + 15);

        for (int dw = 0; dw < 16; dw++)
        {
            printf("  DW[%2d]: 0x%08X\n", dw, sq[base_idx + dw]);
        }
        printf("\n");
    }
    printf("==========================================================\n");
}

void print_io_cq(int offset, int length) {

    uint32_t *cq = (uint32_t *)(SSD_IO_CQ_VIRT_BASE(0, 1));
    printf("========== PRINT IO CQ (cmd offset=%d, count=%d) ==========\n", offset, length);

    for (int cmd_i = 0; cmd_i < length; cmd_i++)
    {
        // CQ 엔트리(하나당 4 DW)
        int base_idx = (offset + cmd_i) * 4;
        printf("----- CQ Entry %d (DW: %d ~ %d) -----\n", offset + cmd_i, base_idx, base_idx + 3);

        for (int dw = 0; dw < 4; dw++)
        {
            printf("  DW[%2d]: 0x%08X\n", dw, cq[base_idx + dw]);
        }
        printf("\n");
    }
    printf("=========================================================\n");
}

void print_empty_mem(int offset, int length) {

    uint32_t *cq = (uint32_t *)((queue_virt_base + 0x5000));
    printf("========== PRINT empty mem (cmd offset=%d, count=%d) ==========\n", offset, length);

    for (int cmd_i = 0; cmd_i < length; cmd_i++)
    {
        // CQ 엔트리(하나당 4 DW)
        int base_idx = (offset + cmd_i) * 4;
        for (int dw = 0; dw < 4; dw++)
        {
            printf("  DW[%2d]: 0x%08X\n", base_idx + dw, cq[base_idx + dw]);
        }
    }
    printf("=========================================================\n");
}


int wait_for_next_cqe(int ssd_id)
{
    volatile uint32_t *command_base = (uint32_t *)(SSD_ADMIN_CQ_VIRT_BASE(ssd_id) + (16 * (admin_cq_hd[ssd_id] & ADMIN_QUEUE_DEPTH)));

    int unexpected_phase = phase_bit[ssd_id];
    
    int current_phase;
    do {
        current_phase = command_base[3];
        current_phase = ((current_phase >> 16) & 0x1);
    }   while (current_phase == unexpected_phase);

    // print current cqe 
    // for (int i = 0; i < 4; i++) {
    //     printf("DW[%d]: 0x%08x\n", i, command_base[i]);
    // }

    int status = command_base[3];
    status = (status >> 17);

    // Ring the doorbell.
    printf("admin_cq_hd: %02x\n", admin_cq_hd[ssd_id]);
    if (++admin_cq_hd[ssd_id] == (ADMIN_QUEUE_DEPTH + 1)) {
        admin_cq_hd[ssd_id] = 0;
        phase_bit[ssd_id] = !phase_bit[ssd_id];
    }
    uint32_t *nvme_cq0hdbl_pt = (uint32_t *)(ssd_virt_base[ssd_id] + 0x1004);
    *nvme_cq0hdbl_pt = admin_cq_hd[ssd_id];

    return status;
}

int wait_for_next_io_cqe(int ssd_id, int qid)
{
    volatile uint32_t *cq_hdbell =
        (uint32_t *)(ssd_virt_base[ssd_id] + 0x1000 + (2 * qid + 1) * 4);

    uint16_t head = *cq_hdbell;
    uint8_t phase = io_phase_bit[ssd_id][qid];

    volatile uint32_t *cqe_base =
        (volatile uint32_t *)(SSD_IO_CQ_VIRT_BASE(ssd_id, qid) + (16 * head));

    uint32_t *sq_base = (uint32_t *)(SSD_IO_SQ_VIRT_BASE(ssd_id, qid));
    for (int i = 0; i < 16; i++) {
        printf("SQ[%d]: 0x%08x\n", i, sq_base[i]);
    }
    while (((cqe_base[3] >> 16) & 0x1) == phase) {
        usleep(1);
        // for (int i = 0; i < 4; i++) {
        //     printf("CQ[%d]: 0x%08x\n", i, cqe_base[i]);
        // }
    }

    int status = (cqe_base[3] >> 17);

    uint16_t new_head = (head + 1) & (IO_QUEUE_DEPTH - 1); 
    io_cq_head[ssd_id][qid] = new_head;

    if (new_head == 0) {
        io_phase_bit[ssd_id][qid] = !io_phase_bit[ssd_id][qid];
    }

    *cq_hdbell = new_head;

    return status;
}

/**
 * Insert a command to the IO submission queue.
 *
 * @param ssd_id  : SSD ID
 * @param qid     : IO Queue ID
 * @param command : 16 DW command
 */
void insert_io_sq(int ssd_id, int qid, const uint32_t command[16])
{
    uint16_t tail = io_sq_tail[ssd_id][qid];

    volatile uint32_t *command_base =
        (volatile uint32_t *)(SSD_IO_SQ_VIRT_BASE(ssd_id, qid) + (64 * tail));

    for (int i = 0; i < 16; i++) {
        command_base[i] = command[i];
    }

    tail = (tail + 1) & (IO_QUEUE_DEPTH - 1);  
    io_sq_tail[ssd_id][qid] = tail;

    volatile uint32_t *sq_tdbell =
        (uint32_t *)(ssd_virt_base[ssd_id] + 0x1000 + (2 * qid) * 4); 
    *sq_tdbell = tail;
}


/**
 * Insert a simple NVM Read command (1 block) into the IO SQ and wait for the CQE
 *
 * @param ssd_id     : SSD identifier
 * @param qid        : IO queue ID
 * @param slba       : Starting LBA (e.g., 0)
 * @param nblocks    : Number of blocks to read (e.g., 1)
 * @param prp1       : Physical address of the buffer to receive data
 * @param nsid       : Namespace ID (usually 1)
 * @return           : Status Field from the CQ (0 if successful)
 */
int nvme_io_read(int ssd_id, int qid,
    uint64_t slba, uint16_t nblocks,
    uint64_t prp1, uint32_t nsid)
{
    uint16_t cid = io_command_id[ssd_id][qid];
    io_command_id[ssd_id][qid] = cid + 1;

    uint32_t cmd[16];
    memset(cmd, 0, sizeof(cmd));

    // DW0
    //  - [31:16] Command ID
    //  - [7:0]   Opcode=0x02(Read)
    cmd[0] = (cid << 16) | 0x02;

    // DW1 = NSID
    cmd[1] = nsid;

    // DW6~7 = PRP1
    cmd[6] = (uint32_t)(prp1 & 0xFFFFFFFF);
    cmd[7] = (uint32_t)(prp1 >> 32);

    // DW10 = [31:16] = NLB - 1 (nblocks - 1), [15:0] = reserved
    cmd[10] = ((uint32_t)(nblocks - 1) << 16);

    // DW12~13 = SLBA
    cmd[12] = (uint32_t)(slba & 0xFFFFFFFF);
    cmd[13] = (uint32_t)(slba >> 32);

    insert_io_sq(ssd_id, qid, cmd);

    return wait_for_next_io_cqe(ssd_id, qid);
}

// Write Opcode = 0x01
int nvme_io_write(int ssd_id, int qid,
    uint64_t slba, uint16_t nblocks,
    uint64_t prp1, uint32_t nsid)
{
    uint16_t cid = io_command_id[ssd_id][qid];
    io_command_id[ssd_id][qid] = cid + 1;

    uint32_t cmd[16];
    memset(cmd, 0, sizeof(cmd));

    // DW0 : CID + opcode=0x01
    cmd[0] = (cid << 16) | 0x01;
    cmd[1] = nsid;
    cmd[6] = (uint32_t)(prp1 & 0xFFFFFFFF);
    cmd[7] = (uint32_t)(prp1 >> 32);
    cmd[10] = ((uint32_t)(nblocks - 1) << 16);
    cmd[12] = (uint32_t)(slba & 0xFFFFFFFF);
    cmd[13] = (uint32_t)(slba >> 32);

    insert_io_sq(ssd_id, qid, cmd);
    return wait_for_next_io_cqe(ssd_id, qid);
}



void insert_admin_sq(int ssd_id, uint32_t command[])
{
    // printf("Inserting command to admin SQ\n");
    volatile uint32_t *command_base = (uint32_t *)(SSD_ADMIN_SQ_VIRT_BASE(ssd_id) + (64 * admin_sq_tl[ssd_id]));

    for (int i=0; i<16; i++)
    {
        command_base[i] = command[i];
    }

    // Ring the doorbell.
    command_id[ssd_id]++;
    admin_sq_tl[ssd_id] = (admin_sq_tl[ssd_id] + 1) & ADMIN_QUEUE_DEPTH;
    volatile uint32_t *nvme_sq0tdbl_pt = (uint32_t *)(ssd_virt_base[ssd_id] + 0x1000);
    *nvme_sq0tdbl_pt = admin_sq_tl[ssd_id];
    return;
}

int nvme_set_num_of_qp(int ssd_id, uint16_t queue_count)
{
    uint16_t queue_count_zerobased = (queue_count - 1);
    uint32_t command[16];
    // Now fill in each dw of command.
    // DW 0: bit 31-16 cmd_id, bit 15-10 rsvd, bit 9-8 fuse, bit 7-0 opcode.
    command[0] = (command_id[ssd_id] << 16) + (0x09 << 0);
    // DW 1: bit 31-0 namespace, all 1's in this case.
    command[1] = 0xffffffff;
    // DW 2-9 rsvd.
    for (int i=2; i<=9; i++)
    {
        command[i] = 0;
    }
    // DW 10: bit 31 save, bit 30-8 rsvd, bit 7-0 feature ID.
    command[10] = (0x07 << 0);
    // DW 11: bit 31-16 number of CQ zerobased, bit 15-0 number of SQ zerobased.
    command[11] = (queue_count_zerobased << 16) + (queue_count_zerobased << 0);
    // DW 12-15 rsvd.
    for (int i=12; i<=15; i++)
    {
        command[i] = 0;
    }
    // for (int i=0; i<16; i++)
    // {
    //     fprintf(stdout, "DW %2d: %08x\n", i, command[i]);
    // }
    insert_admin_sq(ssd_id, command);
    return wait_for_next_cqe(ssd_id);
}


int nvme_create_cq(int ssd_id, uint16_t cq_id, uint16_t cq_depth, uint64_t cq_addr)
{
    uint16_t cq_depth_zerobased = cq_depth - 1;
    uint32_t command[16] = {0};
    // Now fill in each dw of command.
    // DW 0: bit 31-16 cmd_id, bit 15-10 rsvd, bit 9-8 fuse, bit 7-0 opcode.
    command[0] = (command_id[ssd_id] << 16) + (0x05 << 0);
    // DW 1-5 rsvd.
    for (int i=1; i<=5; i++)
    {
        command[i] = 0;
    }
    // DW 6-7: bit 63-0 PRP1
    command[6] = (uint32_t)(cq_addr & 0xffffffff);
    command[7] = (uint32_t)(cq_addr >> 32);
    // DW 8-9 rsvd.
    command[8] = 0;
    command[9] = 0;
    // DW 10: bit 31-16 queue depth, bit 15-0 queue id
    command[10] = (cq_depth_zerobased << 16) + (cq_id << 0);
    // DW 11: Bit 31-16 interrupt vector, bit 15-2 esvd, bit 1 int enable, bit 0 phys cont
    command[11] = 1;
    // DW 12-15 rsvd
    for (int i=12; i<=15; i++)
    {
        command[i] = 0;
    }
    // for (int i=0; i<16; i++)
    // {
    //     fprintf(stdout, "DW %2d: %08x\n", i, command[i]);
    // }
    insert_admin_sq(ssd_id, command);
    return wait_for_next_cqe(ssd_id);
}

int nvme_create_sq(int ssd_id, uint16_t sq_id, uint16_t cq_id, uint16_t sq_depth, uint64_t sq_addr)
{
    uint16_t sq_depth_zerobased = sq_depth - 1;
    uint32_t command[16];
    // Now fill in each dw of command.
    // DW 0: bit 31-16 cmd_id, bit 15-10 rsvd, bit 9-8 fuse, bit 7-0 opcode.
    command[0] = (command_id[ssd_id] << 16) + (0x01 << 0);
    // DW 1-5 rsvd.
    for (int i=1; i<=5; i++)
    {
        command[i] = 0;
    }
    // DW 6-7: bit 63-0 PRP1
    command[6] = (uint32_t)(sq_addr & 0xffffffff);
    command[7] = (uint32_t)(sq_addr >> 32);
    // DW 8-9 rsvd.
    command[8] = 0;
    command[9] = 0;
    // DW 10: bit 31-16 queue depth, bit 15-0 queue id
    command[10] = (sq_depth_zerobased << 16) + (sq_id << 0);
    // DW 11: Bit 31-16 cq_id, bit 15-2 esvd, bit 1 int enable, bit 0 phys cont
    command[11] = (cq_id << 16) + (0x1 << 0);
    // DW 12-15 rsvd
    for (int i=12; i<=15; i++)
    {
        command[i] = 0;
    }    
    // for (int i=0; i<16; i++)
    // {
    //     fprintf(stdout, "DW %2d: %08x\n", i, command[i]);
    // }
    
    insert_admin_sq(ssd_id, command);
    return wait_for_next_cqe(ssd_id);
}


int get_smart_info(int ssd_id, uint64_t phys_addr)
{
    uint32_t command[16];
    // Now fill in each dw of command.
    // DW 0: bit 31-16 cmd_id, bit 15-10 rsvd, bit 9-8 fuse, bit 7-0 opcode.
    command[0] = (command_id[ssd_id] << 16) + (0x02 << 0);
    // DW 1: bit 31-0 namespace
    command[1] = 0xffffffff;
    // DW 2-5 rsvd.
    for (int i=2; i<=5; i++)
    {
        command[i] = 0;
    }
    // DW 6-7: bit 63-0 PRP1
    command[6] = (uint32_t)(phys_addr & 0xffffffff);
    command[7] = (uint32_t)(phys_addr >> 32);
    // DW 8-9: bit 63-0 PRP2, rsvd in this case.
    command[8] = 0;
    command[9] = 0;
    // DW 10: bit 31-16 num of dwords lower, bit 15 retain async event, 
    // bit 14-8 rsvd, dw 7-0 log id
    command[10] = (0x400 << 16) + (0x0 << 15) + 0x02;
    // DW 11: bit 31-16 rsvd, bit 15-0 num of dwords upper.
    command[11] = 0x0;
    // DW 12-13: bit 63-0 log page offset. 0 in this case.
    command[12] = 0x0;
    command[13] = 0x0;
    // DW 14: bit 31-0 UUID. 0 in this case.
    command[14] = 0x0;
    // DW 15 rsvd.
    command[15] = 0x0;
    insert_admin_sq(ssd_id, command);
    return wait_for_next_cqe(ssd_id);
}

int get_temperature_info(int ssd_id, uint64_t phys_addr)
{
    uint32_t command[16];
    // Now fill in each dw of command.
    // DW 0: bit 31-16 cmd_id, bit 15-10 rsvd, bit 9-8 fuse, bit 7-0 opcode.
    command[0] = (command_id[ssd_id] << 16) + (0x02 << 0);
    // DW 1: bit 31-0 namespace
    command[1] = 0xffffffff;
    // DW 2-5 rsvd.
    for (int i=2; i<=5; i++)
    {
        command[i] = 0;
    }
    // DW 6-7: bit 63-0 PRP1
    command[6] = (uint32_t)(phys_addr & 0xffffffff);
    command[7] = (uint32_t)(phys_addr >> 32);
    // DW 8-9: bit 63-0 PRP2, rsvd in this case.
    command[8] = 0;
    command[9] = 0;
    // DW 10: bit 31-16 num of dwords lower, bit 15 retain async event, 
    // bit 14-8 rsvd, dw 7-0 log id
    command[10] = (0x1 << 16) + (0x0 << 15) + 0x02;
    // DW 11: bit 31-16 rsvd, bit 15-0 num of dwords upper.
    command[11] = 0x0;
    // DW 12-13: bit 63-0 log page offset. 200 (0xc8) in this case.
    command[12] = 0xc8;
    command[13] = 0x0;
    // DW 14: bit 31-0 UUID. 0 in this case.
    command[14] = 0x0;
    // DW 15 rsvd.
    command[15] = 0x0;
    insert_admin_sq(ssd_id, command);
    return wait_for_next_cqe(ssd_id);
}

/**
 * Send Identify Controller command and receive the result in the buffer (phys_addr)
 *
 * @param ssd_id     : SSD identifier among multiple devices
 * @param phys_addr  : Physical buffer address to receive Identify result (used as PRP1)
 * @return           : Status Field from the CQ (non-zero value indicates an error)
 */
int nvme_identify_controller(int ssd_id, uint64_t phys_addr)
{

    uint32_t command[16];
    memset(command, 0, sizeof(command));
    printf("memset identify controller memset to 0 fin\n");
    // DW0: Opcode = 0x06 (Identify), Command ID는 command_id[ssd_id]를 사용
    command[0] = (command_id[ssd_id] << 16) | 0x06;

    // DW1: Namespace ID → Identify Controller는 NSID=0 또는 0xffffffff 사용
    //     (스펙상 NSID는 무시되므로 0 사용)
    command[1] = 0x0;

    // DW6~7: PRP1에 Identify Data를 받을 물리 주소 지정
    command[6] = (uint32_t)(phys_addr & 0xffffffff);
    command[7] = (uint32_t)((phys_addr >> 32) & 0xffffffff);

    // DW8~9: PRP2는 4KB 넘게 받을 때 주로 사용하므로 여기서는 0
    command[8] = 0;
    command[9] = 0;

    // DW10: [7:0] CNS = 1 → Identify Controller Data Structure
    //       (나머지 비트는 0)
    command[10] = 1;  // CNS=1

    // 나머지는 전부 0 (memset으로 초기화됐음)

    // Admin SQ에 넣고 Doorbell

    insert_admin_sq(ssd_id, command);
    printf("memset identify controller insert admin sq fin\n");

    // CQ completion까지 폴링
    return wait_for_next_cqe(ssd_id);
}

int main(int argc, char **argv)
{

    memset(io_sq_tail, 0, sizeof(io_sq_tail));
    memset(io_cq_head, 0, sizeof(io_cq_head));
    memset(io_phase_bit, 0, sizeof(io_phase_bit));
    memset(io_command_id, 0, sizeof(io_command_id));

    FILE *fp;
    int fd;
    void *map_base, *hugepg_base, *virt_addr;
    off_t target_offset = NVME_ADMIN_SUBMISSION_QUEUE_BASE_ADDR_OFFSET;

    uint64_t nvme_pcie_bar_base = 0;

    sda::utils::CmdLineParser parser;

    parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
    parser.addSwitch("--device_id", "-d", "device index", "0");
    parser.parse(argc, argv);

    std::string binaryFile = parser.value("xclbin_file");
    int device_index = stoi(parser.value("device_id"));

    if (argc < 3) {
        parser.printHelp();
        exit(EXIT_FAILURE);
    }

    std::cout << "Open the device" << device_index << std::endl;
    auto device = xrt::device(device_index);
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);
    
    fp = fopen(NVME_RESOURCE_FILE, "rb");
    if (fp == NULL) {
        perror("fopen nvme resource file");
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "0x%lx", &nvme_pcie_bar_base);
    fclose(fp);

    printf("BAR 0 of nvme device (physical address) is 0x%lx\n", nvme_pcie_bar_base);

    fd = open("/dev/hugepage_dev", O_RDWR);
    if (fd < 0) {
        perror("open hugepage");
        return 1;
    }

    uint64_t phys_addr;
    if (ioctl(fd, IOCTL_GET_PHYS_ADDR, &phys_addr) < 0) {
        perror("ioctl");
        close(fd);
        return 1;
    }
    printf("Huge page physical address: 0x%lx\n", phys_addr);

    void *huge_base = mmap(NULL, HUGEPAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    memset(huge_base, 0, HUGEPAGE_SIZE);
    mlock(huge_base, HUGEPAGE_SIZE);
    // ((volatile char *)queue_virt_base)[0] = 0;

    queue_virt_base = (uint64_t)huge_base;
    int mfd = open("/dev/mem", O_RDWR | O_SYNC);

    ssd_virt_base[0] = (uint64_t) mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mfd, nvme_pcie_bar_base);
    memset((void *)ssd_virt_base[0], 0, MAP_SIZE);

    print_bar(0, 10);

    if (huge_base == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    printf("Hugepage allocated at virtual address: %p\n", (void*)queue_virt_base);

    queue_phys_base = phys_addr;
    printf("Hugepage physical address: 0x%lx\n", queue_phys_base);
    if (queue_phys_base == 0) {
        fprintf(stderr, "Failed to get physical address\n");
        munmap(huge_base, HUGEPAGE_SIZE);
        return 1;
    }

    uint64_t nvme_ctrl_cap = *(volatile uint64_t *)(ssd_virt_base[0]);
    uint64_t nvme_ctrl_mpsmin = (nvme_ctrl_cap >> 48) & 0xf;
    if (nvme_ctrl_mpsmin > 0)
    {
        fprintf(stderr, "ERROR: The nvme device doesn't support 4KB page.\n");
        exit(1);
    }
    uint64_t nvme_ctrl_dstrd = (nvme_ctrl_cap >> 32) & 0xf;
    printf("Doorbell stride: %lu\n", nvme_ctrl_dstrd);

    if (nvme_ctrl_dstrd > 0)
    {
        fprintf(stderr, "ERROR: The nvme device doesn't support 4B doorbell stride.\n");
        exit(1);
    }
    uint64_t nvme_ctrl_mqes = nvme_ctrl_cap & 0xffff;
    if (nvme_ctrl_mqes < 32)
    {
        fprintf(stderr, "ERROR: The nvme device doesn't support 32 queue entries.\n");
        exit(1);
    }
    // printf("NVMe CAP register: 0x%016lx\n", nvme_ctrl_cap);
    // printf("Doorbell stride: %lu\n", nvme_ctrl_dstrd);
    // dstrd = 0, 2^0 = 1, 1 * 4 = 4

    volatile uint32_t *nvme_cc_pt = (volatile uint32_t *)(ssd_virt_base[0] + 0x14);
    *nvme_cc_pt = 0x460000;
    printf("NVMe CC register: 0x%08x\n", *nvme_cc_pt);

    volatile uint32_t *nvme_csts_pt = (volatile uint32_t *)(ssd_virt_base[0]+ 0x1C);
    while (*nvme_csts_pt != 0) {
        sleep(1);
        // printf("Waiting for controller to be ready. CSTS is %08x.\n", *nvme_csts_pt);
    }
    printf("System reset complete. CSTS is %08x.\n", *nvme_csts_pt);

    // Set admin queue size to 0x1F (32)
    volatile uint32_t *nvme_aqa_pt = (uint32_t *)(ssd_virt_base[0] + 0x24);
    // Set SQ,CQ depth to 0x1F
    *nvme_aqa_pt = (ADMIN_QUEUE_DEPTH << 16) + ADMIN_QUEUE_DEPTH;
    printf("NVMe AQA register: 0x%016x\n", *nvme_aqa_pt);

    // Set admin SQ base address
    uint64_t *nvme_asq_pt = (uint64_t *)(ssd_virt_base[0]+ 0x28);
    *nvme_asq_pt = SSD_ADMIN_SQ_PHYS_BASE(0); // Memory address of admin SQ 
    printf("NVMe ASQ register: 0x%016lx\n", *nvme_asq_pt);

    uint64_t *nvme_acq_pt = (uint64_t *)(ssd_virt_base[0] + 0x30);
    *nvme_acq_pt = SSD_ADMIN_CQ_PHYS_BASE(0); // Memory address of admin CQ
    printf("NVMe ACQ register: 0x%016lx\n", *nvme_acq_pt);

    *nvme_cc_pt = 0x460001;
    printf("NVMe CC register: 0x%08x\n", *nvme_cc_pt); 

    uint64_t *tmp = (uint64_t *)(ssd_virt_base[0] + 0x68);
    // *tmp = 0xf0;

    nvme_csts_pt = (volatile uint32_t *)(ssd_virt_base[0] + 0x1C);
    while(*nvme_csts_pt == 0) {
        sleep(1);
        // printf("Waiting for controller to be ready. CSTS is %08x.\n", *nvme_csts_pt);
    }
    printf("Controller ready. CSTS is %08x.\n", *nvme_csts_pt);
    
    print_bar(0, 10);

    admin_sq_tl[0] = 0;
    admin_cq_hd[0] = 0;

    printf("Admin SQ physical base address: %lx\n", SSD_ADMIN_SQ_PHYS_BASE(0));
    printf("Admin CQ physical base address: %lx\n", SSD_ADMIN_CQ_PHYS_BASE(0));
    printf("Admin SQ virt base address: %lx\n", SSD_ADMIN_SQ_VIRT_BASE(0));
    printf("Admin CQ virt base address: %lx\n", SSD_ADMIN_CQ_VIRT_BASE(0));

    volatile uint32_t *nvme_cq_base = (volatile uint32_t*)SSD_ADMIN_CQ_VIRT_BASE(0);
    for (int i = 0; i < 128; i++)
        nvme_cq_base[i] = 0;

    printf("Created the ADMIN queue pair.\n");

    uint64_t identify_buf_phys = queue_phys_base + 0x20000;
    uint8_t *identify_buf_virt = (uint8_t *)(queue_virt_base + 0x20000);

    // 만약 식별된 정보를 담을 버퍼를 4KB 초기화
    memset(identify_buf_virt, 0, 4096);

    // 실제 Identify Controller 명령 실행
    int status = nvme_identify_controller(0, identify_buf_phys);
    if (status != 0) {
        fprintf(stderr, "Identify Controller command failed, status=0x%x\n", status);
    } else {
        // identify_buf_virt에 컨트롤러 정보 구조체가 담김
        // NVMe 1.4 spec 기준 4096바이트 구조체 (Identify Controller Data Structure)
        printf("PCI Vendor ID: 0x%04x\n", *(uint16_t *)(identify_buf_virt + 0x00));
        // 등등 필요한 필드를 파싱
    }

    int cmd_ret;
    cmd_ret = nvme_set_num_of_qp(0, 1);
    if (cmd_ret != 0) {
        fprintf(stderr, "Failed to set the queue pair\n");
        return 1;
    }

    uint64_t cq_addr = SSD_IO_CQ_PHYS_BASE(0, 1);
    printf("CQ address: 0x%lx\n", cq_addr);
    uint16_t qid = 1;
    uint16_t queue_depth = 128;

    cmd_ret = nvme_create_cq(0, qid, queue_depth, cq_addr);
    if (cmd_ret != 0) {
        fprintf(stderr, "Failed to create the queue pair\n");
        return 1;
    }

    uint64_t sq_addr = SSD_IO_SQ_PHYS_BASE(0, 1);
    cmd_ret = nvme_create_sq(0, qid, qid, queue_depth, sq_addr);
    if (cmd_ret != 0) {
        fprintf(stderr, "Failed to create the queue pair\n");
        return 1;
    }


    printf("sq_addr: 0x%lx\n", sq_addr);
    printf("cq_addr: 0x%lx\n", cq_addr);
    
    printf("IO queue pair created.\n");

    uint64_t buffer_phys = (uint64_t)(queue_phys_base + 0x10000);

    cmd_ret = get_smart_info(0, buffer_phys);
    if (cmd_ret != 0) {
        fprintf(stderr, "Failed to get smart info\n");
        return 1;
    }
    
    uint8_t *smart_info = (uint8_t *)(huge_base + 0x10000);
    if (smart_info[0] != 0x0) {
        printf("SSD reported critical warning 0x%02x\n", smart_info[0]);
    }

    uint16_t temperature_kelvin = *(uint16_t *)(smart_info + 1);
    int temperature_celsius = temperature_kelvin - 273;

    printf("SSD Temperature: %u K (%d°C)\n", temperature_kelvin, temperature_celsius);

    auto ip = xrt::ip(device, uuid, "nvme_driver_top:{nvme_driver_top_1}");
    auto packet_generator_krnl = xrt::kernel(device, uuid, "packet_generator");

 
    uint64_t io_buf_phys = queue_phys_base + 0x10000;
    uint8_t *io_buf_virt = (uint8_t *)(huge_base + 0x10000);
    memset(io_buf_virt, 0x38, 16384);  // 4096바이트(1블록) 예시
    
    uint32_t tail = io_sq_tail[0][1];

    ip.write_register(0x34, (FPGA_IO_SQ_BASE) & 0xFFFFFFFF);
    ip.write_register(0x38, (FPGA_IO_SQ_BASE >> 32) & 0xFFFFFFFF);

    ip.write_register(0x28, (FPGA_IO_CQ_BASE) & 0xFFFFFFFF);
    ip.write_register(0x2c, (FPGA_IO_CQ_BASE >> 32) & 0xFFFFFFFF);

    ip.write_register(0x40, (FPGA_SQ_DOORBELL_BASE) & 0xFFFFFFFF);    
    ip.write_register(0x44, (FPGA_SQ_DOORBELL_BASE >> 32) & 0xFFFFFFFF);


    for (uint64_t i = 0; i < 10; i++) {
        auto packet_generator_run = packet_generator_krnl(0x1, 10+i, 1, io_buf_phys, 1);
        packet_generator_run.wait();
    }

    // ip.write_register(0x4c, io_buf_phys & 0xFFFFFFFF);
    // ip.write_register(0x50, (io_buf_phys >> 32) & 0xFFFFFFFF);

    // ip.write_register(0x58, 0x1);
    // ip.write_register(0x60, 0);
    // ip.write_register(0x68, 1);
    // ip.write_register(0x70, 1);

    print_io_cq(0, 10);    

    
    munmap((void*)ssd_virt_base[0], MAP_SIZE);
    munmap(huge_base, HUGEPAGE_SIZE);

    return 0;
}
