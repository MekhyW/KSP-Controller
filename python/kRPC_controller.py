import krpc
import serial
import argparse
import time
import logging

with serial.Serial('COM3', 115200,timeout=0.5) as ser:
    time.sleep(2);
    while True:
        data = ser.read(3)
        print(data)
        ser.write(b'\0x1\0x62\0xff')