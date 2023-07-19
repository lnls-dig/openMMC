#!/bin/sh

firmware="$1"
device="$2"
address="$3"

if [ -z "$firmware" ] || [ -z "$device" ]; then
	printf "Usage: $0 firmware.bin device [address]\n"
	exit 1
fi

if [ -z "$address" ]; then
	address=0x0000
fi

if [ -z "$MCUXPRESSOIDE_BIN" ]; then
	MCUXPRESSOIDE_BIN=/usr/local/mcuxpressoide/ide/binaries/
fi

"${MCUXPRESSOIDE_BIN}/boot_link1"

set -e

"${MCUXPRESSOIDE_BIN}/crt_emu_cm_redlink" -g -flash-load="$firmware" -load-base="$address" -p ${device}
