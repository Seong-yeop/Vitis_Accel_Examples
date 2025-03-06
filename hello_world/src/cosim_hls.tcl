open_project nvme_driver_top
set_top mgmt_table

add_files "nvme_io_mgmt_table.cpp" \
  -cflags "-I/home/Xilinx/Vitis_HLS/2022.1/lnx64/tools/fir_v7_0 \
           -I/home/Xilinx/Vitis_HLS/2022.1/lnx64/tools/fpo_v7_0 \
           -L/home/Xilinx/Vitis_HLS/2022.1/lnx64/tools/fir_v7_0 \
           -L/home/Xilinx/Vitis_HLS/2022.1/lnx64/tools/fpo_v7_0"

add_files -tb "test_nvme_io_mgmt_table.cpp" \
  -cflags "-I/home/Xilinx/Vitis_HLS/2022.1/lnx64/tools/fir_v7_0 \
           -I/home/Xilinx/Vitis_HLS/2022.1/lnx64/tools/fpo_v7_0 \
           -L/home/Xilinx/Vitis_HLS/2022.1/lnx64/tools/fir_v7_0 \
           -L/home/Xilinx/Vitis_HLS/2022.1/lnx64/tools/fpo_v7_0"


open_solution "solution1"
set_part {xcu55c-fsvh2892-2L-e}
create_clock -period 6.66 -name default

csynth_design 
cosim_design -trace_level all -O
exit

csim_design -clean -O 
exit
