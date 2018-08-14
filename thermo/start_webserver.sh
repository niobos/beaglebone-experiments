#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

SLOTS="/sys/devices/platform/bone_capemgr/slots"
RP1_STATE="/sys/class/remoteproc/remoteproc1/state"


# Ensure DS18B20 overlay is loaded
if ! grep -q "DS18B20-IO" "${SLOTS}"; then
    echo "Loading DS18B20 overlay"
    echo "DS18B20-IO" > "${SLOTS}"
fi


# Ensure DHT22-PRU overlay is loaded
if ! grep -q "DHT22-PRU-IO" "${SLOTS}"; then
    echo "Loading DHT22 overlay"
    echo "DHT22-PRU-IO" > "${SLOTS}"
fi

wait_for() {
    i=5
    while [ "${i}" -gt 0 ]; do
        if [ -e "$1" ]; then
            return
        fi
        sleep 1
        i=$(( $i - 1 ))
    done
}

wait_for "${RP1_STATE}"

# (re)start PRU
if [ "$(< "${RP1_STATE}" )" == "running" ]; then
    echo "Stopping PRU0"
    echo stop > "${RP1_STATE}"
    sleep 1
fi
echo "Starting PRU0"
echo start > "${RP1_STATE}"


sleep 2

cd "${DIR}/www"
exec ../venv/bin/python webserver.py /dev/rpmsg_pru0
