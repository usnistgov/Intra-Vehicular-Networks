#!/bin/bash
sudo modprobe can
sudo modprobe vcan
sudo ip link add dev vcan3 type vcan 
sudo ip link set up vcan3
