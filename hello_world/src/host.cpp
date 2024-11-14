#include <xrt/xrt_device.h>
#include <xrt/xrt_kernel.h>
#include <xrt/xrt_bo.h>
#include <experimental/xrt_ip.h>

#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <vector>

#include "cmdlineparser.h"

#define QUEUE_SIZE 1024
#define BUFFER_SIZE 1500

struct rx_packet {
    uint16_t session_id;
    uint16_t length;
    void *data;
};

struct tx_packet {
    uint16_t session_id;
    uint16_t length;
    void *data;
};

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

    auto krnl = xrt::kernel(device, uuid, "process_request:{process_request_1}", xrt::kernel::cu_access_mode::exclusive);
    // auto ip = xrt::ip(device, uuid, "process_request");

    // Map RXQ addresses to the kernel argument
    // auto rxq_addresses = xrt::bo(device, QUEUE_SIZE*sizeof(struct rx_packet), xrt::bo::flags::host_only, krnl.group_id(0));
    auto rxq_addresses = xrt::bo(device, QUEUE_SIZE*sizeof(struct rx_packet), krnl.group_id(0));
    std::fill(rxq_addresses.map<uint8_t*>(), rxq_addresses.map<uint8_t*>() + QUEUE_SIZE*sizeof(struct rx_packet), 0);

    auto txq_addresses = xrt::bo(device, QUEUE_SIZE*sizeof(uint64_t), xrt::bo::flags::host_only, krnl.group_id(0));

    uint32_t *rx_head = (uint32_t*)malloc(sizeof(uint32_t));
    uint32_t *rx_tail = (uint32_t*)malloc(sizeof(uint32_t));
    uint32_t *tx_head = (uint32_t*)malloc(sizeof(uint32_t));
    uint32_t *tx_tail = (uint32_t*)malloc(sizeof(uint32_t));

    *rx_head = 0;
    *rx_tail = 0;

    auto rx_head_offset = krnl.offset(2);
    auto rx_tail_offset = krnl.offset(3);
    auto tx_head_offset = krnl.offset(4);
    auto tx_tail_offset = krnl.offset(5);

    printf("rx tail offset: %d\n", rx_tail_offset);

    rxq_addresses.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    std::cout << "Execute the kernel" << std::endl;
    auto run = krnl(rxq_addresses, txq_addresses);  
    std::cout << "Kernel is running" << std::endl;
    
    run.wait();
    std::cout << "Kernel is done" << std::endl;

    // print rx, tx head and tail
    *rx_head = krnl.read_register(rx_head_offset);
    *rx_tail = krnl.read_register(rx_tail_offset);
    *tx_head = krnl.read_register(tx_head_offset);
    *tx_tail = krnl.read_register(tx_tail_offset);

    std::cout << "Rx head: " << *rx_head << std::endl;
    std::cout << "Rx tail: " << *rx_tail << std::endl;
    std::cout << "Tx head: " << *tx_head << std::endl;
    std::cout << "Tx tail: " << *tx_tail << std::endl;

    int i = 0;
    while (i < 3) {
        i++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Polling..." << std::endl;

        *rx_tail = krnl.read_register(rx_tail_offset);
        std::cout << "Rx head: " << *rx_head << std::endl;
        std::cout << "Rx tail: " << *rx_tail << std::endl;

        while (*rx_head != *rx_tail) {
            rxq_addresses.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
            auto rxq_mapped = rxq_addresses.map<rx_packet*>();

            rx_packet* rx_packet_data = &rxq_mapped[(*rx_head % QUEUE_SIZE)];
            std::cout << "Rx head: " << *rx_head;
            std::cout << " Received packet: " << rx_packet_data->session_id << " " << rx_packet_data->length << std::endl;
            *rx_head = (*rx_head + 1) % QUEUE_SIZE;
        }
        // krnl.write_register(rx_head_offset, *rx_head);
        // krnl.write_register(tx_tail_offset, *tx_tail);
    }

    free(rx_head);
    free(rx_tail);
    free(tx_head);
    free(tx_tail);

    return 0;
}
