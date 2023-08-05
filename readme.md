# Arduino Learnings

## Tool Installation

Install the AVR development tools for Ubuntu using the commands below.

``` bash
sudo apt-get install gcc-avr binutils-avr avr-libc
sudo apt-get install avrdude
```
## WSL2

For WSL2 Arduino programming you will need access to the USB serial port on the Arduino. This is accomplished from WSL2 through Windows using [usbipd_win](https://github.com/dorssel/usbipd-win).

