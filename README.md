# Autonomous Line-Following Robot

This project features a differential-drive robot that autonomously follows a line using visual input from an OV7670 camera. All hardware and software were developed from scratch using a Raspberry Pi Pico microcontroller and custom-designed embedded firmware.

## System Overview

- **Microcontroller:** Raspberry Pi Pico 2 (RP2350)
- **Sensor:** OV7670 camera module (80×60 RGB565 resolution)
- **Actuation:** Dual DC motors with PWM-based speed control
- **Control Loop:**
  - Image processed at 20 Hz
  - Positional error extracted from binarized frame
  - PID controller computes motor speed differential

## Performance

- Achieved **<5% mean lateral tracking error** on a 40-foot track  
- Real-time PWM adjustments at 10 Hz  
- Robust response to curves and variable lighting

## Repository Structure

- `Final Configuration/`: Complete working firmware for autonomous line-following
- `Line Following Manual Control/`: Version with manual direction input for testing
- `MotorControl/`: Low-level motor testing and PWM tuning
- `Robot CAD / Pictures/`: Photos of the final build and CAD design files
- `README.md`: This file

## Highlights

- Cascaded PID control in C using fixed-point arithmetic  
- Real-time motor control synchronized to camera frame rate  
- Custom logic for frame parsing, noise rejection, and error computation

## Outcome

Built and tuned a fully autonomous robot capable of tracking visual lines in real time, with performance validated through repeatable trajectory tests and quantitative error metrics.
