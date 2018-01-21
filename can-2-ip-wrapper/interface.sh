#!/bin/bash
modprobe can
modprobe vcan
ip link add dev vcan1 type vcan 
ip link set up vcan1
