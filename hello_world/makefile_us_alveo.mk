############################## Help Section ##############################
ifneq ($(findstring Makefile, $(MAKEFILE_LIST)), Makefile)
help:
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all TARGET=<hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to generate the design for specified Target and Shell."
	$(ECHO) ""
	$(ECHO) "  make run TARGET=<hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to run application in emulation."
	$(ECHO) ""
	$(ECHO) "  make build TARGET=<hw_emu/hw> PLATFORM=<FPGA platform>"
	$(ECHO) "      Command to build xclbin application."
	$(ECHO) ""
	$(ECHO) "  make host"
	$(ECHO) "      Command to build host application."
	$(ECHO) ""
	$(ECHO) "  make csim"
	$(ECHO) "      Command to run C simulation (CSIM) for testbench validation."
	$(ECHO) ""
	$(ECHO) "  make clean "
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""
	$(ECHO) "  make cleanall"
	$(ECHO) "      Command to remove all the generated files."
	$(ECHO) ""

endif

############################## Setting up Project Variables ##############################
TARGET := hw
VPP_LDFLAGS :=
include ./utils.mk

TEMP_DIR := ./_x.$(TARGET).$(XSA)
BUILD_DIR := ./build_dir.$(TARGET).$(XSA)

LINK_OUTPUT := $(BUILD_DIR)/nvme_driver_top.link.xclbin
PACKAGE_OUT = ./package.$(TARGET)

VPP_PFLAGS := 
CMD_ARGS = -x $(BUILD_DIR)/nvme_driver_top.xclbin
CXXFLAGS += -I$(XILINX_XRT)/include -I$(XILINX_VIVADO)/include -Wall -O0 -g -std=c++1y
LDFLAGS += -L$(XILINX_XRT)/lib -pthread -lOpenCL

########################## Checking if PLATFORM in allowlist #######################
PLATFORM_BLOCKLIST += nodma 
############################## Setting up Host Variables ##############################
#Include Required Host Source Files
CXXFLAGS += -I$(XF_PROJ_ROOT)/common/includes/cmdparser
CXXFLAGS += -I$(XF_PROJ_ROOT)/common/includes/logger
HOST_SRCS += $(XF_PROJ_ROOT)/common/includes/cmdparser/cmdlineparser.cpp $(XF_PROJ_ROOT)/common/includes/logger/logger.cpp ./src/host.cpp 
# Host compiler global settings
CXXFLAGS += -fmessage-length=0
LDFLAGS += -lrt -lstdc++ 
LDFLAGS += -luuid -lxrt_coreutil

############################## Setting up Kernel Variables ##############################
# Kernel compiler global settings
VPP_FLAGS += --save-temps --config ./mailbox_auto_restart.cfg

EXECUTABLE = ./hello_world_xrt
EMCONFIG_DIR = $(TEMP_DIR)

############################## Setting up CSIM Variables ##############################
CSIM_SRCS = ./src/nvme_driver_top.cpp ./src/testbench.cpp
CSIM_EXEC = ./csim_executable

CSIM_FLAGS = -I$(XILINX_HLS)/include -I$(XILINX_XRT)/include -I$(XILINX_VIVADO)/include -std=c++11 -Wall -O0 -g
CSIM_LDFLAGS = -lrt -lstdc++

############################## Setting Targets ##############################
.PHONY: all clean cleanall docs emconfig csim
all: check-platform check-device check-vitis $(EXECUTABLE) $(BUILD_DIR)/nvme_driver_top.xclbin emconfig

.PHONY: host
host: $(EXECUTABLE)

.PHONY: build
build: check-vitis check-device $(BUILD_DIR)/nvme_driver_top.xclbin

.PHONY: xclbin
xclbin: build

############################## CSIM Target ##############################
# Compile and run the testbench for C simulation
csim: $(CSIM_EXEC)
	@echo "Running C Simulation (CSIM)..."
	./$(CSIM_EXEC)

# Rule to build the CSIM executable
$(CSIM_EXEC): $(CSIM_SRCS)
	$(CXX) -o $@ $^ $(CSIM_FLAGS) $(CSIM_LDFLAGS)

############################## Setting Rules for Binary Containers (Building Kernels) ##############################
# Compile kernel
$(TEMP_DIR)/nvme_driver_top.xo: src/nvme_driver_top.cpp src/nvme_submit_cmd.cpp # src/nvme_process_cpl.cpp 
	mkdir -p $(TEMP_DIR)
	v++ -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) \
		-k nvme_driver_top --temp_dir $(TEMP_DIR) -I'$(<D)' \
		-o'$@' $^


$(TEMP_DIR)/packet_generator.xo: src/packet_generator.cpp
	mkdir -p $(TEMP_DIR)
	v++ -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k packet_generator --temp_dir $(TEMP_DIR) -I'$(<D)' -o'$@' '$<'

$(BUILD_DIR)/nvme_driver_top.xclbin: $(TEMP_DIR)/nvme_driver_top.xo $(TEMP_DIR)/packet_generator.xo
	mkdir -p $(BUILD_DIR)
	v++ -l $(VPP_FLAGS) $(VPP_LDFLAGS) -t $(TARGET) --platform $(PLATFORM) \
		--temp_dir $(TEMP_DIR) -o '$(LINK_OUTPUT)' $^
	v++ -p $(LINK_OUTPUT) $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) \
		--package.out_dir $(PACKAGE_OUT) -o $(BUILD_DIR)/nvme_driver_top.xclbin

############################## Setting Rules for Host (Building Host Executable) ##############################
$(EXECUTABLE): $(HOST_SRCS) | check-xrt
		g++ -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

emconfig:$(EMCONFIG_DIR)/emconfig.json
$(EMCONFIG_DIR)/emconfig.json:
	emconfigutil --platform $(PLATFORM) --od $(EMCONFIG_DIR)

############################## Setting Essential Checks and Running Rules ##############################
run: all
ifeq ($(TARGET),$(filter $(TARGET),hw_emu))
	cp -rf $(EMCONFIG_DIR)/emconfig.json .
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) $(CMD_ARGS)
else
	$(EXECUTABLE) $(CMD_ARGS)
endif
ifneq ($(TARGET),$(findstring $(TARGET), hw hw_emu))
$(error Software Emulation is not supported for the examples and will be deprecated in the tool in 2024.2. Please run hw_emu or hw target.)
endif

.PHONY: test
test: $(EXECUTABLE)
ifeq ($(TARGET),$(filter $(TARGET),hw_emu))
	XCL_EMULATION_MODE=$(TARGET) $(EXECUTABLE) $(CMD_ARGS)
else
	$(EXECUTABLE) $(CMD_ARGS)
endif
ifneq ($(TARGET),$(findstring $(TARGET), hw hw_emu))
$(warning WARNING:Software Emulation is not supported for the examples and will be deprecated in the tool in 2024.2. Please run hw_emu or hw target.)
endif

############################## Cleaning Rules ##############################
# Cleaning stuff
clean:
	-$(RMDIR) $(EXECUTABLE) $(CSIM_EXEC) $(XCLBIN)/{*hw_emu*} 
	-$(RMDIR) profile_* TempConfig system_estimate.xtxt *.rpt *.csv 
	-$(RMDIR) src/*.ll *v++* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
	-$(RMDIR) build_dir*
	-$(RMDIR) package.*
