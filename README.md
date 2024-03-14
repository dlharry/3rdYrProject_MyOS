# 3rdYrProject_MyOS
the repo for my third year project, an bare metal operating system that runs on raspberry pi 4

to compile the project(Compiled on Ubuntu 22.04), required environment is below:

software:  
`aarch64-linux-gnu`
`make`

hardware:  
`raspberry pi 4b`
`usb-to-ttl serial cable`
`micro-hdmi-to-hdmi cable`
`a monitor`
`sd card and sd card reader`

Run `make clean` `make` under project root, will copy the essential files(`config.txt``armstub-new.bin``kernel8.img`) to the sd card, you might wanna change the location of sd card in `Makefile` to suit your environment.

If not interested in compiling the project yourself, the compiled files are in the project folder, copy them according to Makefile and make the essential name changes, recommend using Makefile since that is simplier.

Insert sd card. Connect the GPIO pins 6, 8, 10 with GND, TXD, RXD. Plug monitor. Run `screen /dev/ttyUSB0 115200,cs8,ixoff,-istrip` and power the raspberry pi.

You can press any key to run the game when the information on the screen stops to update.
