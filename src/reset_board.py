#!/bin/env python3

import serial
import time
import argparse

argParser = argparse.ArgumentParser()
argParser.add_argument("-d", "--device", help="serial port device", required=True)
args = argParser.parse_args()

# open port and toggle dtr line to reset device
ser = serial.Serial(args.port, 9600)  # open serial port
ser.dtr = 1
time.sleep(0.1)
ser.dtr = 0
