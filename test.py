#!/usr/bin/python

import glob
import serial
import platform

DEBUG_PRINT = False


###############################################################################
# TODO: implement an interactive terminal with getch()?
# https://stackoverflow.com/questions/510357/python-read-a-single-character-from-the-user

def main():
    port = serial_init()
    d = 100

    while True:
        go_to(0, d, port)
        go_to(d, d, port)
        go_to(d, 0, port)
        go_to(0, 0, port)


###############################################################################
def serial_init():
    PLATFORM = platform.system()
    if "Linux" in PLATFORM:
        SERIAL_PATH = "/dev/ttyUSB*"
    elif "Darwin" in PLATFORM:
        SERIAL_PATH = "/dev/tty.usb*"
    else: # Windows
        port = serial.Serial('COM5', 115200)        # TODO ADAPT COM NUMBER !!
        SERIAL_PATH = 'WIN_WORKARAOUND'

    if SERIAL_PATH != 'WIN_WORKARAOUND':
        devices = glob.glob(SERIAL_PATH)
        port = serial.Serial(devices[0], 115200)

    success = port.isOpen()

    if success:
        print("Port open, waiting for calibration...\n")
        line = port.readline()
        while ("start".encode(encoding="utf-8") not in line):
            line = port.readline()
    else:
        print("\n!!! Error: serial device not found !!!")
        exit(-1)
    return port


###############################################################################
def go_to(x, y, port):
    command = "X" + str(x) + " Y" + str(y) + "\n"
    port.write(command.encode(encoding="utf-8"))
    if DEBUG_PRINT: print(command)

    # wait for acknowledgment
    line = port.readline()
    while ("ok".encode(encoding="utf-8") not in line):
        line = port.readline()


###############################################################################
if __name__ == "__main__":
    main()

