open_project nvme_driver_top
set_top mgmt_table

add_files "nvme_io_mgmt_table.cpp \
            "

add_files -tb "test_nvme_io_mgmt_table.cpp"

open_solution "solution1"
set_part {xcu55c-fsvh2892-2L-e}
create_clock -period 6.66 -name default

csynth_design
cosim_design -trace_level all -O -jobs 8
exit

csim_design -clean -O -jobs 8
exit
