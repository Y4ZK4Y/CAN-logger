# CAN-logger

Attempt at making a stm32 powered board that logs CAN data to a SD card - in progress . . . 

install ARM toolchain (arm-none-eabi-gcc) - openocd - TODO: manage things with nix
git submodule update --init --recursive

cd /Users/$USER$/whatever/CAN-logger/libopencm3
make

cd app/
make

to see UART output:
get board's virtual sertial port (macos) /dev/cu.*
screen PORT 115200
Ctrl+A, then K, then Y to confirm.

TODO: cleanup the build, super messy in app/src/

