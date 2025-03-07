#!/bin/bash

XRT_PATH=/home/csl/heejae/XRT/build/Debug/opt/xilinx/xrt/bin

echo -n "0000:d8:00.1" | sudo tee /sys/bus/pci/drivers/xocl/unbind
rmmod xocl

pushd /home/csl/heejae/XRT/build/Debug/usr/src/xrt-2.13.0/driver/xocl/userpf
insmod xocl.ko

sudo $XRT_PATH/xbutil configure --host-mem disable --device d8:00.1
sudo $XRT_PATH/xbutil configure --host-mem enable --device d8:00.1 --size 8M

source /opt/xilinx/xrt/setup.sh > /dev/null

popd

pushd /home/csl/heejae/XRT/build/Debug/opt/xilinx/xrt

source /home/csl/heejae/XRT/build/Debug/opt/xilinx/xrt/setup.sh

popd

echo "0000:18:00.0" | sudo tee /sys/bus/pci/drivers/nvme/unbind
echo "0000:af:00.0" | sudo tee /sys/bus/pci/drivers/nvme/unbind

sudo rmmod nvme

sudo setpci -s 0000:18:00.0 COMMAND=0x0007
sudo setpci -s 0000:af:00.0 COMMAND=0x0007

echo $XILINX_XRT

sudo insmod /home/csl/heejae/huge_page_alloc/huge_page_alloc.ko

make host

./hello_world_xrt -x build_dir.hw.xilinx_u55c_gen3x16_xdma_3_202210_1/nvme_driver_top.xclbin

#./reset.sh
