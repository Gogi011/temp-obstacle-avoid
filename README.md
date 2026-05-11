# temp-obstacle-avoid

## Quick overview (your new simple setup)

- **Controller:** Arduino Uno
- **Drive:** 2 DC motors + 1 caster wheel (2WD)
- **Motor driver:** 1 × L298N
- **Sensors:** 3 × HC-SR04 (left, center, right in C-shape front coverage)
- **Power:** 2 × Li-ion in series (nominal 7.4V)
- **Target behavior:** go straight by default, react only when obstacle is detected (15 cm threshold), avoid getting stuck near 90° corners.

This repo includes a **minimal, reliable baseline** Arduino sketch for that exact setup.

## Why this setup works for your track

- Indoor static obstacles + raised boundaries + one incline are handled well with a tuned 2WD differential drive.
- 2WD gives tighter turns and simpler control than 4WD for this timeline.
- 3 front-facing ultrasonics improve corner detection vs single sensor.
- PWM motor control keeps speed manageable so sensor response can keep up.

## Wiring (Arduino Uno + 1 L298N + 3 HC-SR04)

### L298N to Arduino

- **ENA** → D5 (PWM)
- **IN1** → D8
- **IN2** → D9
- **IN3** → D10
- **IN4** → D11
- **ENB** → D6 (PWM)
- **GND** (L298N) ↔ **GND** (Arduino)

### Motors

- Left motor → L298N **OUT1/OUT2**
- Right motor → L298N **OUT3/OUT4**

### HC-SR04 pins

- Left sensor: **TRIG A0**, **ECHO A1**
- Center sensor: **TRIG A2**, **ECHO A3**
- Right sensor: **TRIG A4**, **ECHO A5**

### Power

- Motor supply (L298N `+12V`/`VIN`) from 2S Li-ion pack (7.4V nominal, 8.4V full).
- Keep **all grounds common** (battery/L298N/Arduino/sensors).
- Power Arduino through a safe regulated path (USB or proper 5V/regulator path), not raw high voltage on 5V pin.

## Notes

- Yes, you can reduce/control N20 motor speed in code using PWM (`analogWrite` on ENA/ENB).
- Start with conservative speed values in the sketch, then tune on track.

## Sketch

Use: [`obstacle_avoidance_3x_hcsr04.ino`](./obstacle_avoidance_3x_hcsr04.ino)
