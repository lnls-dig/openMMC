#!/bin/bash

# Check if a serial port argument is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <serial_port>"
    exit 1
fi

SERIAL_PORT=$1

SCRIPT_PATH="log-serial.py"
SERVICE_TEMPLATE="log-serial@.service"
SERVICE_PATH="/usr/local/lib/systemd/system/log-serial@.service"
BIN_PATH="/usr/local/bin/log-serial.py"

cp "$SCRIPT_PATH" "$BIN_PATH"
cp "$SERVICE_TEMPLATE" "$SERVICE_PATH"

systemctl enable --now "log-serial@${SERIAL_PORT}.service"
