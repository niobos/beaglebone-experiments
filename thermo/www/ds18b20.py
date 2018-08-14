from w1thermsensor import W1ThermSensor

for sensor in W1ThermSensor.get_available_sensors():
    print("{} (0x{:x}) {}: {}".format(
        sensor.type_name, sensor.type,
        sensor.id,
        sensor.get_temperature(),
    ))
