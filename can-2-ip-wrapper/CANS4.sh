#!/bin/bash
sudo modprobe can
sudo modprobe vcan
sudo ip link add dev vcan4 type vcan 
sudo ip link set up vcan4
