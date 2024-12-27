#include <iostream>
#include <xrt/xrt_device.h>
#include <xrt/xrt_bo.h>
#include <experimental/xrt_ip.h>
#include <cstring>   // for memset
#include <thread>
#include <chrono>

static const size_t DATA_SIZE = 256; // bytes

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <xclbin_file>\n";
        return 1;
    }
    std::string xclbin_path = argv[1];

    // 1) Open device
    auto device = xrt::device(0);

    // 2) Load xclbin
    std::cout << "Loading xclbin: " << xclbin_path << std::endl;
    auto uuid = device.load_xclbin(xclbin_path);

    // 3) Create a pinned host memory bo
    //    "host_only" means the buffer physically resides in pinned host memory,
    //    accessible via PCIe. The FPGA sees it as an AXI address space range.
    auto bo = xrt::bo(device, DATA_SIZE, xrt::bo::flags::host_only, /*group_id=*/0);
    // auto bo = xrt::bo(device, DATA_SIZE, /*group_id=*/0);

    // 4) Map to host pointer and initialize
    auto host_ptr = bo.map<uint32_t*>();
    std::memset(host_ptr, 0x00, DATA_SIZE); // fill with 0x55
    // No bo.sync needed for "host_only" to device? Usually not. But if needed, do:
    // bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    // 5) Construct IP object
    //    Suppose your block is named "dma_host_mem:{dma_host_mem_1}"
    auto ip = xrt::ip(device, uuid, "process_request:{process_request_1}");

    // 6) Suppose the IP has the following register offsets:
    //    base_addr = (0x10 low, 0x14 high), size_bytes = 0x18
    //    Real offsets come from your HLS or block design.
    constexpr uint32_t BASE_ADDR_LOW  = 0x10;
    constexpr uint32_t BASE_ADDR_HIGH = 0x14;
    constexpr uint32_t SIZE_REG       = 0x1C;

    // 7) Get the device address of the pinned host memory (64-bit)
    uint64_t bo_addr = bo.address(); // or .physical_address()
    uint32_t addr_low  = static_cast<uint32_t>( bo_addr & 0xFFFFFFFF );
    uint32_t addr_high = static_cast<uint32_t>((bo_addr >> 32) & 0xFFFFFFFF );

    std::cout << "bo_addr = 0x" << std::hex << bo_addr << std::dec << "\n";

    // 8) Write the IP registers
    ip.write_register(BASE_ADDR_LOW,  addr_low);
    ip.write_register(BASE_ADDR_HIGH, addr_high);
    ip.write_register(SIZE_REG,       static_cast<uint32_t>(DATA_SIZE));
    ip.write_register(SIZE_REG,      0); 
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    ip.write_register(BASE_ADDR_LOW,  addr_low);
    ip.write_register(BASE_ADDR_HIGH, addr_high);
    ip.write_register(SIZE_REG,       static_cast<uint32_t>(DATA_SIZE));
    ip.write_register(SIZE_REG,      0); 
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // If ap_ctrl_none, the IP might immediately start reading/writing host_ptr data.
    // We'll wait a bit to let it run
    
    // ip.write_register(BASE_ADDR_LOW,  0);
    // ip.write_register(BASE_ADDR_HIGH, 0);

    // 9) Check the result in host_ptr
    //    If the IP inverts each byte, we expect 0xAA in each location now
    // bo.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    for (int i = 0; i < 16; i++) {
        std::cout << "host_ptr[" << i << "] = 0x" 
                  << std::hex << static_cast<int>(host_ptr[i]) << std::dec << "\n";
    }

    std::cout << "Done.\n";
    return 0;
}
