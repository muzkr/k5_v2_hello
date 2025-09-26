#!/usr/bin/env python3


import argparse
import serial
import signal
from time import sleep

import _prog as pp


def load_image(file: str) -> bytes:

    a = bytearray()
    with open(file, "rb") as fd:
        buf = bytearray(512)
        while True:
            len1 = fd.readinto(buf)
            if len1 > 0:
                a.extend(memoryview(buf)[:len1])
            if len1 < len(buf):
                break

    return a


def main():

    # Usage: serialtool.py --port /dev/ttyUSB0 file
    ap = argparse.ArgumentParser(description="UV-K5 V2 serial tool")
    ap.add_argument(
        "--port", "-p", help="serial port, eg., '/dev/ttyUSB0'", required=True
    )
    ap.add_argument("file", help="firmware image file")
    args = ap.parse_args()

    port: str = args.port
    fw_file: str = args.file

    try:
        fw_image = load_image(fw_file)
        if 0 == len(fw_image):
            print("Invalid firmware image: {}: empty file".format(fw_file))
            return
    except Exception as e:
        print("Cannot load firmware image '{}': {}".format(fw_file, e))
        return

    try:
        ser = serial.Serial(port, baudrate=38400, timeout=0.0001, write_timeout=None)
    except Exception as e:
        print("Cannot open port '{}': {}".format(port, e))
        return

    print("UV-K5 V2 serial tool")
    print("Press Ctrl-C to quit")
    print("Firmware image loaded: {}, size = {}".format(fw_file, len(fw_image)))

    quit_flag = False

    def quit_handler(sig, frame):
        nonlocal quit_flag
        quit_flag = True

    signal.signal(signal.SIGINT, quit_handler)

    prog = pp.Programmer(ser, fw_image)

    while not quit_flag:
        prog.loop()
        sleep(0)

    print("Quit")
    ser.close()


if __name__ == "__main__":
    main()
