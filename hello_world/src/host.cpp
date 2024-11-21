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

    // Map RXQ addresses to the kernel argument
    // auto rxq_addresses = xrt::bo(device, QUEUE_SIZE*sizeof(struct rx_packet), xrt::bo::flags::host_only, krnl.group_id(0));
    auto rxq_addresses = xrt::bo(device, QUEUE_SIZE*sizeof(struct rx_packet), krnl.group_id(0));
    std::fill(rxq_addresses.map<uint8_t*>(), rxq_addresses.map<uint8_t*>() + QUEUE_SIZE*sizeof(struct rx_packet), 0);
    
    auto rx_buffer = xrt::bo(device, QUEUE_SIZE*BUFFER_SIZE, krnl.group_id(0));

    auto txq_addresses = xrt::bo(device, QUEUE_SIZE*sizeof(struct tx_packet), krnl.group_id(1));
    std::fill(txq_addresses.map<uint8_t*>(), txq_addresses.map<uint8_t*>() + QUEUE_SIZE*sizeof(struct tx_packet), 0);

    auto tx_buffer = xrt::bo(device, QUEUE_SIZE*BUFFER_SIZE, krnl.group_id(1));

    uint32_t rx_head = 0, rx_tail = 0, tx_head = 0, tx_tail = 0;

    auto rx_head_offset = krnl.offset(4);
    auto rx_tail_offset = krnl.offset(5);
    auto tx_head_offset = krnl.offset(6);
    auto tx_tail_offset = krnl.offset(7);

    // std::cout << "rx_head_offset: " << rx_head_offset << std::endl;
    // std::cout << "rx_tail_offset: " << rx_tail_offset << std::endl;
    // std::cout << "tx_head_offset: " << tx_head_offset << std::endl;
    // std::cout << "tx_tail_offset: " << tx_tail_offset << std::endl;

    krnl.write_register(rx_head_offset, rx_head);
    krnl.write_register(rx_tail_offset, rx_tail);
    krnl.write_register(tx_head_offset, tx_head);
    krnl.write_register(tx_tail_offset, tx_tail);

/*
    rxq_addresses.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    std::cout << "Execute the kernel" << std::endl;
    auto run = krnl(rxq_addresses, rx_buffer, txq_addresses, tx_buffer);  
    std::cout << "Kernel is running" << std::endl;
    
    // run.wait();
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
    while (i < 3) {
        i++;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "Polling..." << std::endl;

        rx_tail = krnl.read_register(rx_tail_offset);
        std::cout << "Rx head: " << rx_head << std::endl;
        std::cout << "Rx tail: " << rx_tail << std::endl;

        while (rx_head != rx_tail) {
            rxq_addresses.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
            rx_buffer.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
            auto rxq_mapped = rxq_addresses.map<rx_packet*>();

            rx_packet* rx_packet_data = &rxq_mapped[(rx_head % QUEUE_SIZE)];
            std::cout << "Rx head: " << rx_head;
            std::cout << " Received packet: " << rx_packet_data->session_id << " " << rx_packet_data->length << std::endl;
            for (int i = 0; i < 5; i++) {
                // Copy data from rx_buffer
                std::cout << rx_buffer.map<char*>()[rx_head*BUFFER_SIZE + i];
            }
            std::cout << std::endl;
            rx_head = (rx_head + 1) % QUEUE_SIZE;
        }
        krnl.write_register(rx_head_offset, rx_head);
    }
*/
    krnl.write_register(tx_head_offset, tx_head);
    auto tx_buffer_map = tx_buffer.map<char*>();
    for (uint16_t i = 0; i < 10; i++) {
        tx_packet* tx_packet_data = &txq_addresses.map<tx_packet*>()[i];
        tx_packet_data->session_id = i;
        tx_packet_data->length = 1500;
        tx_tail = (tx_tail + 1) % QUEUE_SIZE;
        // krnl.write_register(tx_tail_offset, tx_tail);

        // copy data to tx_buffer
        for (uint16_t j = 0; j < 1500; j++) {
            tx_buffer_map[i*BUFFER_SIZE + j] = 'A' + i;
        }
    }
    krnl.write_register(tx_tail_offset, tx_tail);

    tx_head = krnl.read_register(tx_head_offset);
    tx_tail = krnl.read_register(tx_tail_offset);
    std::cout << "Tx head: " << tx_head << std::endl;
    std::cout << "Tx tail: " << tx_tail << std::endl;

    
    rxq_addresses.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    rx_buffer.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    tx_buffer.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    txq_addresses.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    
    auto run2 = krnl(rxq_addresses, rx_buffer, txq_addresses, tx_buffer, rx_head, rx_tail, tx_head, tx_tail);  
    // auto run2 = krnl(rxq_addresses, rx_buffer, txq_addresses, tx_buffer);  
    // auto run2 = xrt::run(krnl);
    // run2.set_arg(0, rxq_addresses);
    // run2.set_arg(1, rx_buffer);
    // run2.set_arg(2, txq_addresses);
    // run2.set_arg(3, tx_buffer);
    // run2.set_arg(4, rx_head);
    // run2.set_arg(5, rx_tail);
    // run2.set_arg(6, &tx_head);
    // run2.set_arg(7, &tx_tail);
    // run2.start(); 
    
    // std::this_thread::sleep_for(std::chrono::seconds(10));
    run2.wait();
    // run2 = krnl(rxq_addresses, rx_buffer, txq_addresses, tx_buffer);  
    // run2 = krnl(rxq_addresses, rx_buffer, txq_addresses, tx_buffer);  
    // run2 = krnl(rxq_addresses, rx_buffer, txq_addresses, tx_buffer);  
    txq_addresses.sync(XCL_BO_SYNC_BO_FROM_DEVICE); // 커널 실행 후 동기화
    tx_buffer.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    tx_head = krnl.read_register(tx_head_offset);
    tx_tail = krnl.read_register(tx_tail_offset);
    std::cout << "Tx head: " << tx_head << std::endl;
    std::cout << "Tx tail: " << tx_tail << std::endl;

    return 0;
}
    