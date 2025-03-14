#!/bin/bash

XRT_PATH=/home/csl/heejae/XRT/build/Debug/opt/xilinx/xrt/bin

source /opt/xilinx/xrt/setup.sh > /dev/null


pushd /home/csl/yeop/XRT/build/Debug/opt/xilinx/xrt
source /home/csl/yeop/XRT/build/Debug/opt/xilinx/xrt/setup.sh
popd

echo $XILINX_XRT

pwd

make all -j 16

