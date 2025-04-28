open_project nvmet_tcp_top
set_top nvme_tcp_driver_top

add_files "nvme_tcp/nvme_tcp_driver_top.cpp nvme_tcp/nvme_tcp_admin.cpp" \
  -cflags "-Wno-unknown-pragmas" -csimflags "-Wno-unknown-pragmas"

add_files -tb "nvme_tcp/test_nvme_tcp.cpp" 

open_solution "solution1"
set_part {xcu55c-fsvh2892-2L-e}
create_clock -period 6.66 -name default

#csim
csim_design -clean -O
exit

