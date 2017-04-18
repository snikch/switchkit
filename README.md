# SwitchKit 🎛

Home Automation for your existing switches, with a little bit of smarts.

SwitchKit provides a simple firmware for ESP8266 devices to control up to three smart or dumb bulbs, via physical switches. It works perfectly with Sonoff, Electrodragon and any ESP8266 based device. It is designed to be placed in wall boxes and wire in to existing switches. Lights are then controlled by a combination of relays and MQTT messages, although all functionality is preserved if your wifi etc. goes down. This firmware is built using the [homie-esp8266](https://github.com/marvinroger/homie-esp8266) framework.

# Features

- [x] Control normal bulbs via MQTT
- [x] Control smart bulbs via physical switches
- [x] Avoid disgraceful degradation if wifi or mqtt connectivity fails
- [x] OTA updates (via homie-esp8266)
- [x] Handles up to three bulbs of either type
- [x] Remotely configurable via MQTT

## Dumb Lights vs Smart Lights

In dumb mode, a bulb is toggled on or off via the physical switch, or via an MQTT command (for home automation). This allows existing bulbs to be made "smart".

In smart mode, the bulb itself must be a "smart bulb" which is capable of being turned off via a home automation system. SwitchKit handles turning the bulb on and off by emitting MQTT commands when online, or by falling back to "dumb mode" when there is no network or MQTT broker connection. For this mode to work, it needs to be in conjunction with home automation software such as [Home Assistant](https://home-assistant.io).


# Motivation

We have a house full of both Lifx and dumb bulbs. While the lifx bulbs are great to turn off with an app, it means that toggling the switch at the wall turns them off… again, and toggling them off at the wall means you can't turn them back on with the app.

SwitchKit is designed to provide toggleable bulbs for the entire home, regardless of whether they are smart or not. Dumb bulbs become smart, and smart bulbs can be controlled via physical and digital means, without ending up in weird states.

# Installation

It is recommended to install via [PlatformIO](platformio.org). If you can not use PlatformIO, then please see the platformio.ini file for the required dependencies.

# Future Features

- [ ] Enable dimming of smart bulbs (perhaps via a combination of toggles)

# Flow Chart

![flow chart](https://cdn.rawgit.com/snikch/switchkit/master/docs/flowchart.svg)

## buttonState

```
press:
  - increment count
  - start timer

timeout:
  count == 1:
    emit: single_press
  count >1 < 5:
    emit: unknown_press
  count: 5:
    emit: manual_toggle
```
