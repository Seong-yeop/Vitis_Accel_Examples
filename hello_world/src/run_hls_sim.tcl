# TCL script for NVMe Driver Top Test
# Usage: vitis_hls -f run_hls_sim.tcl [csim|synth|cosim]

set mode [lindex $argv 0]
if {$mode eq ""} {
    set mode "csim"
}

open_project nvme_driver_test
set_top nvme_driver_top

add_files "nvme_driver_top.cpp \
           nvme_submit_cmd.cpp \
           nvme_process_cpl.cpp \
           nvme_io_mgmt_table.cpp \
           packet_generator.cpp" \
  -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"

add_files -tb "test_nvme_driver_top.cpp" \
  -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"

open_solution "solution1"
set_part {xcu55c-fsvh2892-2L-e}
create_clock -period 4.44 -name default

if {$mode eq "csim"} {
    # C simulation only
    csim_design -clean -O
} elseif {$mode eq "synth"} {
    # Synthesis only
    csynth_design
} elseif {$mode eq "cosim"} {
    # C/RTL co-simulation
    csynth_design
    config_compile -pipeline_style frp
    cosim_design -trace_level all -O
}

exit
