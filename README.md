# RPi and STM32 Camera Projects

## Overview

This repository contains two similar projects focused on camera systems:
## Projects

### Raspberry Pi - Following camera with Face Detection & Facial Recognition

- **Description**: Implements a following camera with face detection and facial recognition using a Raspberry Pi. Uses Buildroot to implement own image, OpenCV, Qt for UI, and custom drivers, daemons, cthreads, etc. 
- **Features**:
  - Real-time face detection
  - Facial recognition with a pre-trained model.
  - Following controll motors.
  - Own custom Linux image, and drivers, daemons, cthreads etc.
  - Graphic Interface
- **Requirements**: Raspberry Pi, Camera module, OpenCV, Qt, Buildroot, Servo-motors


### STM32 - Remote Controlled Sonar Camera

- **Description**: Utilizes STM32 and a ESP8266 for controlling a camera over the network with sonar capabilities.
- **Features**:
  - Remote camera control
  - Sonar-based distance measurement
- **Requirements**: STM32 microcontroller, Camera module, Ultrasonic sensor

![Pan Tilt](https://github.com/Phedro-Shousa/RPi-and-STM32-Camera-Projects/blob/master/Pimoroni_pan-tilt-HAT-anim.gif))
