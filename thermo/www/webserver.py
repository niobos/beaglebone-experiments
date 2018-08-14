#!/usr/bin/env python3
import sys
import os
import asyncio
from signal import signal, SIGINT

import sanic
import sanic.response
from w1thermsensor import W1ThermSensor

from util import rh_to_dewpoint
from dht22 import DHT22

app = sanic.Sanic()

server = app.create_server(host="0.0.0.0", port=8000)
task = asyncio.ensure_future(server)

signal(SIGINT, lambda: asyncio.get_event_loop().stop())

try:
    port = sys.argv[1]
    fd = os.open(port, os.O_RDWR | os.O_EXCL)
    temps = DHT22(fd)
except IndexError:
    print("Usage: {} /dev/tty".format(sys.argv[0]))
    sys.exit(64)


def do_ds_measurements():
    m = {}
    for sensor in W1ThermSensor.get_available_sensors():
        m[sensor.id] = sensor.get_temperature()
    return m


@app.route("/")
async def index(_request):
    ds = asyncio.get_event_loop().run_in_executor(None, do_ds_measurements)

    dht = await temps.current_measurement()
    for measurement in dht:
        try:
            measurement['dewpoint'] = rh_to_dewpoint(
                temperature=measurement['temperature'],
                relative_humidity=measurement['humidity'],
            )
        except KeyError:
            pass

    return sanic.response.json({
        "DHT22": dht,
        "DS18B20": await ds,
    })

try:
    asyncio.get_event_loop().run_forever()
except:
    asyncio.get_event_loop().stop()
