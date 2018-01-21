#!/bin/bash
sudo modprobe can
sudo modprobe vcan
sudo ip link add dev vcan2 type vcan 
sudo ip link set up vcan2
