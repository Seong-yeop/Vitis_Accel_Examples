#include <xrt/xrt_device.h>
#include <xrt/xrt_kernel.h>
#include <xrt/xrt_bo.h>
#include <iostream>
#include <thread>
#include <chrono>

#include "cmdlineparser.h"

#define QUEUE_SIZE (4096)


int main(int argc, char **argv) {
   // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
    parser.addSwitch("--device_id", "-d", "device index", "0");
    parser.parse(argc, argv);

    // Read settings
    std::string binaryFile = parser.value("xclbin_file");
    int device_index = stoi(parser.value("device_id"));

    if (argc < 3) {
        parser.printHelp();
        return EXIT_FAILURE;
    }

    std::cout << "Open the device" << device_index << std::endl;
    auto device = xrt::device(device_index);
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);

    auto krnl = xrt::kernel(device, uuid, "process_request");

    auto sq_bo = xrt::bo(device, QUEUE_SIZE * sizeof(unsigned int), krnl.group_id(0));
    auto cq_bo = xrt::bo(device, QUEUE_SIZE * sizeof(unsigned int), krnl.group_id(1));

    auto sq_head_bo = xrt::bo(device, sizeof(unsigned int), krnl.group_id(0));
    auto sq_tail_bo = xrt::bo(device, sizeof(unsigned int), krnl.group_id(0));
    auto cq_head_bo = xrt::bo(device, sizeof(unsigned int), krnl.group_id(1));
    auto cq_tail_bo = xrt::bo(device, sizeof(unsigned int), krnl.group_id(1));

    auto sq_buffer = sq_bo.map<unsigned int*>();
    auto cq_buffer = cq_bo.map<unsigned int*>();
    auto sq_head = sq_head_bo.map<unsigned int*>();
    auto sq_tail = sq_tail_bo.map<unsigned int*>();
    auto cq_head = cq_head_bo.map<unsigned int*>(); 
    auto cq_tail = cq_tail_bo.map<unsigned int*>();

    unsigned int op = 0;
    *sq_head = *sq_tail = 0;
    *cq_head = *cq_tail = 0;
    int new_command = 0;
    int threshold = QUEUE_SIZE / 32;

    sq_head_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    for (int i = 0; i < QUEUE_SIZE / 4; i++) {
        std::cout << "Adding new command to SQ: sq_tail = " << *sq_tail << std::endl;
        sq_buffer[*sq_tail] = op++;
        *sq_tail = (*sq_tail + 1) % QUEUE_SIZE;
    }
    // sync SQ and execute the kernel (kernel is running continuously)
    sq_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    sq_tail_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    cq_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    cq_head_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    cq_tail_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << *sq_head << " " << *sq_tail << std::endl;

    std::cout << "Execute the kernel" << std::endl;
    auto run = krnl(sq_bo, cq_bo, sq_head_bo, sq_tail_bo, cq_head_bo, cq_tail_bo);  // 커널을 한 번만 실행 (종료되지 않음)
    // auto run = krnl(sq_bo, cq_bo);
    std::cout << "Kernel is running" << std::endl;

    // Polling and adding commands loop
    int i = 0;
    while (i < 10) {
        i++;
        // CQ buffer, Tail pointer sync
        cq_tail_bo.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        cq_bo.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

        // Get data from CQ
        while (*cq_head != *cq_tail) {
            std::cout << *cq_head << " " << *cq_tail << std::endl;
            std::cout << "Completion Queue Result: " << cq_buffer[*cq_head] << std::endl;
            *cq_head = (*cq_head + 1) % QUEUE_SIZE;
        }
        cq_head_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        // Add new commands to SQ
        new_command = 0;
        while (new_command < threshold) {
            if ((*sq_tail + 1) % QUEUE_SIZE == *sq_head) {
                break;
            }
            new_command++;  
            std::cout << "Adding new command to SQ: sq_tail = " << *sq_tail << std::endl;
            sq_buffer[*sq_tail] = op++;
            *sq_tail = (*sq_tail + 1) % QUEUE_SIZE;

        }
        sq_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        sq_tail_bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Polling..." << std::endl;
    }

    return 0;
}
