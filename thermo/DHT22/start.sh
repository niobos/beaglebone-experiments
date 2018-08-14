#!/bin/bash

echo "DHT22-PRU-IO" > /sys/devices/platform/bone_capemgr/slots
echo start > /sys/class/remoteproc/remoteproc1/state
sleep 2

/opt/src/DHT22/decode.pl /dev/rpmsg_pru0
