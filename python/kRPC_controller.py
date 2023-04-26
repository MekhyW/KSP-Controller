import krpc
import serial
import argparse
import time
import logging

with serial.Serial('COM3', 115200, timeout=1) as ser:
    while True:
        data = ser.read_until(b'\xff')
        while len(data) < 2:
            data += ser.read_until(b'\xff')
        print(data)