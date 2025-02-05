#!/usr/bin/env python3

import serial
import sys
from datetime import datetime


if len(sys.argv) < 3:
    print("Usage: ./log-serial.py <file_path> <serial_port>")
    sys.exit(1)

file_path = sys.argv[1]
serial_port = sys.argv[2]

serport = serial.Serial(baudrate=19200)
serport.setPort(serial_port)
serport.open()

# Don't reset the MMC but drive the P2.10 pin to HIGH, so it can be reset
# externally and execute the application code, not the ROM bootloader
serport.setRTS(0)  # set RTS line to 3.3V
serport.setDTR(0)  # set DTR line to 3.3V


with open(file_path, "a") as log_file:
    while True:

        try:
            message = serport.readline().decode("utf-8", errors="replace").strip()
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f %z")
            log_entry = f"[{timestamp}] {message}\n"
            log_file.write(log_entry)
            print(log_entry, flush=True)
            log_file.flush()  # Always flush when a new log is written

        except serial.SerialException:
            print("Serial port disconnected. Shutting down...")
