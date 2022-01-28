![Main branch building](https://github.com/Simpit-team/KerbalSimpitNavball/actions/workflows/ci-arduino.yml/badge.svg)


Navball library for Arduino, using KerbalSimpit to connect KSP to the Arduino

The goal of this library is to be able to draw a navball on a screen connected to an Arduino, in order to integrate it in a custom KSP controller.

# How to install

Just copy the content into the libraries folder of Arduino. After restarting the Arduino IDE, you should see the KerbalNavball examples.

See the Arduino IDE documentation on [how to install a library](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries).

# Requirements

This library use the [Simpit library](https://github.com/Simpit-team/KerbalSimpitRevamped) to connect to KSP. Version 2.1.0 or higher of Simpit is needed.

This library is (mostly) tested with an Arduino Due with [this screen](https://www.adafruit.com/product/2050), based on a HXD8357D with a resolution of 320x480. It is wired using a parallel 8-bit mode, as decribed in this [Adafruit tutorial](https://learn.adafruit.com/adafruit-2-8-and-3-2-color-tft-touchscreen-breakout-v2/pinouts)

# Performance

The following table gives some values of the time needed to compute a frame in different scenario with an Arduino Due with an HXD8357D. It is wired using a parallel 8-bit mode.

| Example                                          | Size     | Time                     |
| ------------------------------------------------ | -------- | ------------------------ |
| KerbalSimpitNavballStandalone                    | 300x300  | between 650ms and 700ms  |
| KerbalSimpitNavballStandalone                    | 200x200  | between 450ms and 500ms  |
