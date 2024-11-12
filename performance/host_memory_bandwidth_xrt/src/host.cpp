/**
* Copyright (C) 2019-2021 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/
// Read -> Write Stream.


#include "xcl2.hpp"
#include "cmdlineparser.h"
#include <cstring>
#include <iostream>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"


#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iosfwd>
#include <unistd.h>

#define QUEUE_SIZE (4096)

int main(int argc, char* argv[]) { 
    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
    parser.addSwitch("--device_id", "-d", "device index", "0");
    parser.addSwitch("--file_path", "-p", "file path string", "");
    parser.addSwitch("--input_file", "-f", "input file string", "");
    parser.parse(argc, argv);    

    // Read settings
    std::string binaryFile = parser.value("xclbin_file");
    int device_index = stoi(parser.value("device_id"));
    std::string filepath = parser.value("file_path");
    std::string filename;

    if (argc < 5) {
        parser.printHelp();
        return EXIT_FAILURE;
    }
   
    std::cout << "Open the device" << device_index << std::endl;
    auto device = xrt::device(device_index);

    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);

    auto krnl = xrt::kernel(device, uuid, "bandwidth");
    auto krnl_read = xrt::kernel(device, uuid, "read_bandwidth");
    auto krnl_write = xrt::kernel(device, uuid, "write_bandwidth");

    xrt::bo::flags flags = xrt::bo::flags::host_only;
	
    if (filepath.empty()) {
        std::cout << "\nWARNING: As file path is not provided using -p option, going with -f option which is local "
                     "file testing. Please use -p option, if looking for actual p2p operation on NVMe drive.\n";
        filename = parser.value("input_file");
    } else {
        std::cout << "\nWARNING: Ignoring -f option when -p options is set. -p has high precedence over -f.\n";
        filename = filepath;
    }

    int nvmeFd = -1;

    // Get access to the NVMe SSD.
    nvmeFd = open(filename.c_str(), O_RDWR | O_DIRECT);

    if (nvmeFd < 0) {
        std::cerr << "ERROR: open " << filename << "failed: " << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "INFO: Successfully opened NVME SSD " << filename << std::endl;

    xrt::bo::flags p2p_flags = xrt::bo::flags::p2p;

    double concurrent_max = 0;
    
    for (size_t i = 4 * 1024; i <= 32 * 1024 * 1024; i *= 2) {
        size_t iter = (64 * 1024 * 1024) / i;
        size_t bufsize = i;

        if (xcl::is_emulation()) {
            iter = 2;
            if (bufsize > 8 * 1024) break;
        }

    
        auto hostonly_bo_in = xrt::bo(device, bufsize, p2p_flags, krnl_read.group_id(0));
        auto hostonly_bo_out = xrt::bo(device, bufsize, flags, krnl_write.group_id(1));

        std::string size_str = xcl::convert_size(bufsize);

        // Map the contents of the buffer object into host memory
        auto bo_in_map = hostonly_bo_in.map<char*>();
        auto bo_out_map = hostonly_bo_out.map<char*>();

        std::fill(bo_in_map, bo_in_map + bufsize, 0);
        std::fill(bo_out_map, bo_out_map + bufsize, 0);

        auto start = std::chrono::high_resolution_clock::now();
        // Read From NVMe SSD to Device Buffers
        if (pread(nvmeFd, (void*)bo_in_map, bufsize, 0) <= 0) {
            std::cerr << "ERR: pread failed: "
                    << " error: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        double msduration = duration;
        double dsduration = msduration / ((double)1000000);
        double bpersec = (bufsize / dsduration);
        double gbpersec = (bpersec) / ((double)1024 * 1024 * 1024); // For Concurrent Read and Write

        std::cout << "Pread throughput " << gbpersec << " (GB/sec) for buffer size " << size_str
		<< std::endl;

        int64_t index_start = 0; // Start
        int64_t index_end = bufsize / 64; // Full Size
        int64_t memcpy_start = (index_start) * 64; 
        int64_t memcpy_size = (index_end - index_start) * 64; 

        start = std::chrono::high_resolution_clock::now();
        auto run = xrt::run(krnl_read);
        auto run2 = xrt::run(krnl_write);

        run.set_arg(0, hostonly_bo_in);
        run.set_arg(2, bufsize);
        run.set_arg(3, iter);
        run.set_arg(4, index_start);
        run.set_arg(5, index_end);

        run2.set_arg(1, hostonly_bo_out);
        run2.set_arg(2, bufsize);
        run2.set_arg(3, iter);
        run2.set_arg(4, index_start);
        run2.set_arg(5, index_end);


        run.start();
        run2.start();        

        run.wait();
        run2.wait();        

        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        msduration = duration / iter;
        dsduration = msduration / ((double)1000000);
        bpersec = (memcpy_size / dsduration);
        gbpersec = (bpersec) / ((double)1024 * 1024 * 1024); // For Concurrent Read and Write

        std::cout << "Host -> FPGA -> Host throughput " << 2 * gbpersec << " (GB/sec) for buffer size " << size_str
		<< std::endl;

        // Validate our results
        if (std::memcmp(bo_out_map + memcpy_start, bo_in_map + memcpy_start, memcpy_size))
           throw std::runtime_error("Value read back does not match reference");
	
        if (2 * gbpersec > concurrent_max) {
            concurrent_max = 2 * gbpersec;

        }
    }

    std::cout << "Maximum bandwidth achieved :\n";
    std::cout << "Concurrent Read and Write Throughput = " << concurrent_max << " (GB/sec) \n";
    std::cout << "TEST PASSED\n";
    return EXIT_SUCCESS;
}
 
