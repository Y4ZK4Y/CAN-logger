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

resources:
https://yaseen.ly/writing-data-to-sdcards-without-a-filesystem-spi/
https://elm-chan.org/docs/mmc/mmc_e.html
https://satoshinm.github.io/blog/180108_usb_power_usb_breakout_boards_for_supplying_power_to_your_projects.html
