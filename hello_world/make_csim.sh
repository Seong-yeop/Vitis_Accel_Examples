source /opt/xilinx/Vivado/2022.1/settings64.sh
source /opt/xilinx/Vitis/2022.1/settings64.sh
rm -rf src/nvmet_tcp_top
cd src

vitis_hls csim_hls.tcl
