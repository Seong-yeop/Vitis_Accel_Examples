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
    // void *data;
};

struct tx_packet {
    uint16_t session_id;
    uint16_t length;
    // void *data;
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
    auto send_krnl = xrt::kernel(device, uuid, "send_kernel:{send_kernel_1}", xrt::kernel::cu_access_mode::exclusive);

    // Map RXQ addresses to the kernel argument
    // auto rxq_addresses = xrt::bo(device, QUEUE_SIZE*sizeof(struct rx_packet), xrt::bo::flags::host_only, krnl.group_id(0));
    auto rxq_addresses = xrt::bo(device, QUEUE_SIZE*sizeof(struct rx_packet), krnl.group_id(0));
    std::fill(rxq_addresses.map<uint8_t*>(), rxq_addresses.map<uint8_t*>() + QUEUE_SIZE*sizeof(struct rx_packet), 0);
    
    auto rx_buffer = xrt::bo(device, QUEUE_SIZE*BUFFER_SIZE, krnl.group_id(0));

    auto txq_addresses = xrt::bo(device, QUEUE_SIZE*sizeof(struct tx_packet), krnl.group_id(1));
    std::fill(txq_addresses.map<uint8_t*>(), txq_addresses.map<uint8_t*>() + QUEUE_SIZE*sizeof(struct tx_packet), 0);

    uint32_t rx_head = 0, rx_tail = 0, tx_head = 0, tx_tail = 0;

    auto rx_head_offset = krnl.offset(3);
    auto rx_tail_offset = krnl.offset(4);
    auto tx_head_offset = krnl.offset(5);
    auto tx_tail_offset = krnl.offset(6);

    printf("rx tail offset: %d\n", rx_tail_offset);

    rxq_addresses.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    std::cout << "Execute the kernel" << std::endl;
    auto run = krnl(rxq_addresses, rx_buffer, txq_addresses);  
    std::cout << "Kernel is running" << std::endl;
    
    run.wait();
    std::cout << "Kernel is done" << std::endl;

    // print rx, tx head and tail
    rx_head = krnl.read_register(rx_head_offset);
    rx_tail = krnl.read_register(rx_tail_offset);
    tx_head = krnl.read_register(tx_head_offset);
    tx_tail = krnl.read_register(tx_tail_offset);

    std::cout << "Rx head: " << rx_head << std::endl;
    std::cout << "Rx tail: " << rx_tail << std::endl;
    std::cout << "Tx head: " << tx_head << std::endl;
    std::cout << "Tx tail: " << tx_tail << std::endl;

    int i = 0;
    while (i < 1) {
        i++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Polling..." << std::endl;

        rx_tail = krnl.read_register(rx_tail_offset);
        std::cout << "Rx head: " << rx_head << std::endl;
        std::cout << "Rx tail: " << rx_tail << std::endl;

        while (rx_head != rx_tail) {
            rxq_addresses.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
            auto rxq_mapped = rxq_addresses.map<rx_packet*>();

            rx_packet* rx_packet_data = &rxq_mapped[(rx_head % QUEUE_SIZE)];
            std::cout << "Rx head: " << rx_head;
            std::cout << " Received packet: " << rx_packet_data->session_id << " " << rx_packet_data->length << std::endl;
            rx_head = (rx_head + 1) % QUEUE_SIZE;
        }
        krnl.write_register(rx_head_offset, rx_head);
        // krnl.write_register(tx_tail_offset, *tx_tail);
    }

    send_krnl.write_register(send_krnl.offset(2), tx_head);
    send_krnl.write_register(send_krnl.offset(3), tx_tail);
    for (int i = 0; i < 10; i++) {
        tx_packet* tx_packet_data = &txq_addresses.map<tx_packet*>()[i];
        tx_packet_data->session_id = i;
        tx_packet_data->length = i;
        tx_tail = (tx_tail + 1) % QUEUE_SIZE;
    }
    send_krnl.write_register(send_krnl.offset(3), tx_tail);
    krnl.write_register(tx_tail_offset, tx_tail);
    txq_addresses.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    char data[1500] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto run2 = send_krnl(txq_addresses, data);
    // run2.wait();

    std::cout << "Send kernel" << std::endl;
    std::cout << send_krnl.read_register(send_krnl.offset(2)) << std::endl;
    std::cout << send_krnl.read_register(send_krnl.offset(3)) << std::endl;

    return 0;
}
    