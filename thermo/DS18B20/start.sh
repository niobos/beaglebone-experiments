#!/usr/bin/env bash
#!/bin/bash

echo "DS18B20-IO" > /sys/devices/platform/bone_capemgr/slots

sleep 2

cat /sys/bus/w1/devices/28-*/w1_slave
