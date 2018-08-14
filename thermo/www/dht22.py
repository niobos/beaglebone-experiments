#!/usr/bin/env python3
import sys
import os
import asyncio


class DHT22:
    def __init__(self, fd: int):
        self.fd = fd
        self.num_sensors = 4

        self.measurement = asyncio.get_event_loop().create_future()
        self.measurement.set_result(None)

        asyncio.get_event_loop().add_reader(self.fd, self._new_measurement_ready)

    def start_new_measurement(self):
        self.measurement = asyncio.get_event_loop().create_future()

        os.write(self.fd, b"\x00")
        # Trigger measurement. Results will come in as soon as they are ready (can take up to 2 seconds)

        return self.measurement

    def _new_measurement_ready(self):
        # Triggered by ready-for-read on self.fd

        buf = os.read(self.fd, 5*self.num_sensors)

        result = []
        measurements = [buf[5*i:5*i+5] for i in range(self.num_sensors)]
        for m in measurements:
            my_checksum = sum([b for b in m[0:4]])
            my_checksum %= 0x100
            if my_checksum != m[4]:
                result.append({
                    "error": "checksum mismatch: my={} got={}".format(
                        my_checksum, m[4]
                    ),
                    "raw data": ' '.join(['{:02x}'.format(b) for b in m]),
                })
                continue

            temp = m[2] * 256 + m[3]
            if temp & 0x8000:
                # 1's complement, according to https://github.com/adafruit/DHT-sensor-library/blob/master/DHT.cpp#L53
                temp = temp & 0x7fff
                temp = -temp
            temp /= 10

            hum = m[0] * 256 + m[1]
            hum /= 10

            result.append({"temperature": temp, "humidity": hum})

        try:
            self.measurement.set_result(result)
        except asyncio.InvalidStateError:
            # future already resolved, ignore new data
            pass

    def current_measurement(self):
        # make sure that multiple simultaneous requests get the same measurement back.
        if self.measurement.done():
            # This is not thread safe. There is a race condition right here.
            # Additional locks needed when running in multithreading
            self.start_new_measurement()
        return self.measurement


if __name__ == "__main__":
    from pprint import pprint
    try:
        port = sys.argv[1]
        fd = os.open(port, os.O_RDWR | os.O_EXCL)

        dht = DHT22(fd)
        m = dht.current_measurement()

        m = asyncio.get_event_loop().run_until_complete(m)
        pprint(m)
    except IndexError:
        print("Usage: {} /dev/tty".format(sys.argv[0]))
