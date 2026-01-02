# NVMe Driver HLS Implementation

A Vitis HLS implementation of an FPGA-based NVMe driver for hardware-accelerated I/O operations on NVMe SSDs using AMD Alveo accelerator cards.

## Architecture Overview

```
                    +------------------+
  Request Packet -->| nvme_io_cmd_gen  |
                    +--------+---------+
                             |
                             v
                    +------------------+
                    |  nvme_mgmt_prp   |  (PRP List Management)
                    +--------+---------+
                             |
                             v
                    +------------------+
                    |nvme_io_sqe_dbl   |
                    |     _write       |---> Doorbell Write
                    +--------+---------+
                             |
                             v
                    +------------------+
                    |    sq_writer     |---> SQ Entry Write
                    +------------------+

                    +------------------+
  CQ Polling <------|nvme_process_cpl  |<--- CQ Entry Read
                    +--------+---------+
                             |
                             v
                    +------------------+
                    |   mgmt_table     |  (Command Info Management)
                    +------------------+
```

## Module Description

### Top Module
- **nvme_driver_top.cpp**: Top-level module connecting all submodules using DATAFLOW

### Submit Path
| Module | File | Description |
|--------|------|-------------|
| `nvme_io_cmd_gen` | nvme_submit_cmd.cpp | Converts request packets to NVMe I/O commands |
| `nvme_mgmt_prp` | nvme_submit_cmd.cpp | Manages PRP (Physical Region Page) lists |
| `nvme_io_sqe_dbl_write` | nvme_submit_cmd.cpp | Writes commands to SQ and updates doorbell |
| `sq_writer` | nvme_submit_cmd.cpp | SQ memory writer (separated for pipeline optimization) |

### Completion Path
| Module | File | Description |
|--------|------|-------------|
| `nvme_process_cpl` | nvme_process_cpl.cpp | CQ polling and completion processing |
| `mgmt_table` | nvme_io_mgmt_table.cpp | Command ID-based command info table management |

## Data Structures

### NVMe I/O Command (64 bytes)
```cpp
typedef struct __attribute__((packed, aligned(64))) _nvme_io_command {
    uint8_t  opc;      // Opcode (0x01=Write, 0x02=Read)
    uint8_t  fuse;     // Fuse
    uint16_t cid;      // Command ID
    uint32_t nsid;     // Namespace ID
    uint64_t mptr;     // Metadata Pointer
    uint64_t prp1;     // PRP Entry 1
    uint64_t prp2;     // PRP Entry 2
    uint32_t cdw10-15; // Command specific DWORDs
} nvme_io_command_t;
```

### NVMe Completion Queue Entry (16 bytes)
```cpp
typedef struct __attribute__((packed, aligned(64))) _nvme_cqe_t {
    uint32_t dw0;           // Command Specific
    uint32_t dw1;           // Reserved
    uint16_t sq_head;       // SQ Head Pointer
    uint16_t sq_identifier; // SQ Identifier
    uint16_t cid;           // Command ID
    uint32_t phase_tag:1;   // Phase Tag
    uint32_t status_code:8; // Status Code
    ...
} nvme_cqe_t;
```

## HLS Optimization Pragmas

| Pragma | Purpose |
|--------|---------|
| `#pragma HLS DATAFLOW` | Enables pipeline parallel execution between modules |
| `#pragma HLS PIPELINE II=1` | Processes new input every clock cycle |
| `#pragma HLS STREAM depth=512` | FIFO-based data streaming |
| `#pragma HLS DEPENDENCE` | Removes false dependencies |
| `#pragma HLS bind_storage type=RAM_T2P` | Uses True Dual-Port BRAM |

## Interface Configuration

| Port | Interface | Bundle | Description |
|------|-----------|--------|-------------|
| io_cq_base_addr | m_axi | gmem0 | CQ memory access |
| io_sq_base_addr | m_axi | gmem1 | SQ memory access (burst write enabled) |
| dbl_base_address | m_axi | gmem2/3 | Doorbell register access |
| start/done | s_axilite | - | Control registers |

## Build

```bash
# HLS Synthesis
cd nvme_driver
source /path/to/Vitis/settings64.sh
make all TARGET=hw PLATFORM=<platform>
```

## Queue Parameters

| Parameter | Value | Description |
|-----------|-------|-------------|
| `IO_QUEUE_MAX_DEPTH` | 128 | Maximum I/O Queue depth |
| `IO_QUEUE_MAX_NUM` | 4 | Maximum number of queues |
| `MAX_CMD_INFO_TBL_SIZE` | 256 | Command info table size |

## File Structure

```
nvme_driver/
├── src/
│   ├── nvme_driver_top.hpp      # Header and data structure definitions
│   ├── nvme_driver_top.cpp      # Top module
│   ├── nvme_submit_cmd.cpp      # Submit path modules
│   ├── nvme_process_cpl.cpp     # Completion path modules
│   ├── nvme_io_mgmt_table.cpp   # Command info table management
│   ├── packet_generator.cpp     # Test packet generator
│   ├── host.cpp                 # Host application
│   └── test_*.cpp               # Testbench files
├── Makefile
└── README.md
```

## Target Platform

- AMD Alveo U55C (xcu55c-fsvh2892-2L-e)
- Clock: 150 MHz (6.66ns period)
