modprobe can
ip link add dev vcan0 type vcan
ip link set up vcan0
ifconfig vcan0 up
./IPtoCanwrapper &
