sudo modprobe nvme

if ! sudo nvme list | grep -q "nvme1n1"; then
    echo "0000:af:00.0" | sudo tee /sys/bus/pci/drivers/nvme/bind
    sleep 1
fi

sudo nvme read /dev/nvme1n1 -s $1 -z 4096 -c 1
