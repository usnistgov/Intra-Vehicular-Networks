#!/bin/bash
sudo modprobe can
sudo modprobe vcan
sudo ip link add dev vcan1 type vcan 
sudo ip link set up vcan1
