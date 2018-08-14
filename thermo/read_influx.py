import argparse
import requests
from influxdb import InfluxDBClient


parser = argparse.ArgumentParser(description='Thermo reader')
parser.add_argument('--base-url', help="Base URL to query on", default="http://localhost:8080/")
parser.add_argument('--db', help="influx database to write to", default='hvac')

args = parser.parse_args()

points = []

r = requests.get('{b}'.format(
    b=args.base_url,
))
if r.status_code == 200:
    data = r.json()

    try:
        d = data['DHT22'][0]
        points.append({
            'measurement': 'ventilation',
            'tags': {
                'port': 'supply input',
            },
            'fields': {
                'temperature': d['temperature'],
                'relative_humidity': d['humidity'],
                'dewpoint': d['dewpoint'],
            }
        })
    except KeyError:
        pass

    try:
        d = data['DHT22'][2]
        points.append({
            'measurement': 'ventilation',
            'tags': {
                'port': 'supply output',
            },
            'fields': {
                'temperature': d['temperature'],
                'relative_humidity': d['humidity'],
                'dewpoint': d['dewpoint'],
            }
        })
    except KeyError:
        pass

    try:
        d = data['DHT22'][1]
        points.append({
            'measurement': 'ventilation',
            'tags': {
                'port': 'exhaust input',
            },
            'fields': {
                'temperature': d['temperature'],
                'relative_humidity': d['humidity'],
                'dewpoint': d['dewpoint'],
            }
        })
    except KeyError:
        pass

    try:
        d = data['DS18B20']
        d = list(d.values())[0]
        points.append({
            'measurement': 'ventilation',
            'tags': {
                'port': 'exhaust output',
            },
            'fields': {
                'temperature': d,
            }
        })
    except KeyError:
        pass

db_con = InfluxDBClient(database=args.db)
db_con.write_points(points)
