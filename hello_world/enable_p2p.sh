#!/bin/bash

XRT_PATH=/home/csl/heejae/XRT/build/Debug/opt/xilinx/xrt/bin
sudo $XRT_PATH/xbutil configure --p2p enable --device d8:00.1
#./reset.sh
