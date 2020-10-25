# libLunaEsp32

A modular library for making IoT LED lights. The modules can be quite freely mixed and matched allowing you to use only what you need.

# Features

## Light types
- PWM - supports arbitrary number of RGBW channels while still being logically a single light source.
- Addressable LED - WS2812 type LEDs

## Modules
- Effects - on device generated effects such as flame, colorful calming lavalamp-like effect, single color.
- Persistency - used to preserve the state of the Effects module after reboot
- Mqtt - used for controlling the Effects module via the MQTT protocol for easy integration with home automation solutions such as https://www.home-assistant.io/
- Update - allows over-the-air updates
- Realtime - allows low latency control using the desktop app https://github.com/wchomik/LunaClient
- Infrared - allows control using commodity IR remotes

## Security

Realtime communication uses DTLS with certificate verification.

# Using the library

Take a look at the examples: https://github.com/romanchom/lunaEsp32Template
