# Self-Balancing Robot

A two-wheeled inverted-pendulum robot that keeps itself upright using a closed-loop PID controller. It was built as a mini summer project in 2025, with an ESP32 for control and a BNO055 for orientation sensing. The gains were tuned wirelessly over Bluetooth, using an empirical Ziegler–Nichols-style procedure.

## Why this project

A two-wheeled robot is an unstable system. Left alone, it falls over, so the controller has to keep correcting the tilt. The project covers the full control loop: sensing, control and actuation. It was also my first hands-on look at how much tuning matters for a real plant.

![Self-balancing](Self_Balancing.png)

## Hardware

| Component | Details |
|---|---|
| Microcontroller | ESP32 WROOM |
| IMU | Bosch BNO055 (9-DOF, on-chip sensor fusion) |
| Motors | NEMA 17 MOTOR |
| Motor driver | DRV8825 Stepper Driver |
| Frame | Aluminium extrusion frame on a flat base plate, designed in CAD (Solidworks) |
| Wireless link | ESP32 Bluetooth [Classic / BLE], used for live PID tuning and telemetry |

## How it works

1. **Sense.** The BNO055 gives the tilt angle of the chassis.
2. **Compute error.** Error = setpoint (upright) − measured tilt.
3. **Control.** A PID controller turns the error into a motor command:
   - **P** reacts to the current tilt.
   - **I** removes steady-state offset.
   - **D** damps oscillation using the rate of change of tilt.
4. **Actuate.** The command drives both wheels in the direction of the lean, which moves the base back under the centre of mass.
5. **Tune live.** Gains can be changed over Bluetooth while the robot is balancing, so there is no reflashing between trials.

## PID tuning

Tuning was done in two stages, and every change was sent over Bluetooth while the robot was running.

**1. Manual hit and trial.** I changed Kp, Ki, and Kd manually and observed the response. This gave a rough working baseline.

**2. Empirical, Ziegler–Nichols-style tuning.** To make this systematic, I tuned one term at a time and then went round again:

1. **Kp first.** With Ki = 0 and Kd = 0, raise Kp until the robot responds firmly and starts to oscillate. Then back off slightly.
2. **Then Ki.** Add just enough integral action to remove the steady lean or drift, without causing slow oscillation.
3. **Then Kd.** Add a derivative action to damp the oscillation and the overshoot after a push.
4. **Repeat.** The terms interact, so I went back through P, I and D until the response was stable.

I used the idea behind Ziegler–Nichols (find the point where P alone starts to oscillate and tune from there), not the table formulas.

## Repository

```
├── self_balancing.ino    # ESP32 firmware
├── motor-test.ino        # test
└── README.md
```

## Getting started

1. Wire the BNO055 to the ESP32 over I²C, and the motor driver to the ESP32 pins defined in the sketch.
2. Open the `.ino` file in the Arduino IDE with ESP32 board support installed.
3. Install the required libraries: `[Adafruit BNO055, Adafruit Unified Sensor, ...]`.
4. Set the initial PID gains at the top of the sketch.
5. Flash it, hold the robot upright, power on and release.
6. Connect over Bluetooth to adjust the gains live.

## Tech stack

`ESP32` · `BNO055` · `PID control` · `Ziegler–Nichols (empirical)` · `Bluetooth` · `C/C++`

## Author

**Dhriti Dhall**, Electronics and Computer Engineering, TIET Patiala
[LinkedIn] · [GitHub]
