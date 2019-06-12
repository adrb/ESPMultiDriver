
Alternative firmware for Espressif's ESP8266 MCU based devices.

What distinguishes it from other implementations is that it's event driven
and completly asynchrounous, which means ultra speed and responsiveness.

It's also designed to facilitate implementation of new device drivers,
and provides you with framework for fast and easy development.

You can watch demo with polish subtitles [here](https://www.youtube.com/watch?v=_lr_p6fi5mk)

### Compilation and installation

#### Preparation

- Install Atom IDE and run it
- You also need to install the platform.io IDE for Atom
  - Navigate to Menu: File > Settings > Install
  - Search for platform.io IDE package
  - Install "platformio-ide" package along with "clang"

#### Building and uploading the firmware

- Download source files from Github
- Inside of Atom IDE
  - Navigate to Menu: PlatformIO > PlatformIO Home Tab, then click on "Open Project"
  - Choose directory with downloaded source files. Be sure that the platformio.ini file is there
  - Open platformio.ini file and change "upload_port" to proper COM port
  - Navigate to Menu: PlatformIO > Upload

#### Uploading SPIFFS

If you want to have nice and easy HTTP configuration interface, you can upload
SPIFFS files with jQuery Mobile interface.
It's recommended step, however if your device doesn't have at least 2MB flash
memory, can be safely omitted

Open terminal window, type in command below and hit enter key
```
pio run -t uploadfs
```

### License

This program is licensed under GPL-3.0

## Author:

Adrian Brzezinski <adrian.brzezinski at adrb.pl>
