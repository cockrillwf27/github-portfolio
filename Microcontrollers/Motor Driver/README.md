# DC Motor Driver Control System (Register-Level Implementation)

This project focused on designing and implementing a complete DC motor control system using **register-level programming** on an Arduino-based microcontroller. The system integrates hardware interfacing, real-time interrupts, PWM motor control, and a finite state machine (FSM), all built from the ground up **without Arduino libraries**.

The goal was to create a safe, responsive, and electrically isolated platform that allows the user to control motor direction, speed, and braking while providing clear visual feedback.

---

## System Capabilities

The final system supports:

- On/Off motor control  
- Direction selection (CW ↔ CCW)  
- PWM speed control via potentiometer  
- Emergency stop / quick brake  
- Active LED indicators  
  - Direction  
  - Movement  
  - Emergency stop  
- Full electrical isolation using 3 optocouplers  
- Interrupt-driven button inputs (no polling)  

All logic—state transitions, interrupts, PWM setup, and I/O configuration—was implemented at the register level using AVR registers such as **DDRx, PORTx, PINx, EIMSK, EICRA, TCCRn, OCRn**, etc.

---

## Design Overview

Development was divided into four main components:

1. **I/O Allocation & Hardware Planning**  
2. **Interrupt Design**  
3. **Finite State Machine Implementation**  
4. **PWM and Motor Driver Integration**

Below are the I/O tables implemented in the final system.

---

## I/O Allocation

### **Table 1 — Arduino I/O Allocation**

| Port (Pin)  | Mode            | Component       | Function                     |
|-------------|-----------------|-----------------|------------------------------|
| PD2 (pin 2) | Input           | Button 1        | User input (interrupt)       |
| PD3 (pin 3) | Input           | Button 2        | User input (interrupt)       |
| PD4 (pin 4) | Input           | Button 3        | User input (interrupt)       |
| PD6 (pin 6) | Analog Output   | Optocoupler 3   | PWM control                  |
| PB0 (pin 8) | Output          | LED1            | Direction indicator          |
| PB1 (pin 9) | Output          | LED2            | Movement indicator           |
| PB2 (pin 10)| Output          | LED3            | Emergency stop indicator     |
| PB4 (pin 12)| Output          | Optocoupler 1   | AIN1 – direction control     |
| PB5 (pin 13)| Output          | Optocoupler 2   | AIN2 – direction control     |
| PC0 (A0)    | Analog Input    | Potentiometer   | Speed control                |

---

## LED Indicators

### **Table 2 — LED Allocation**

| LED  | Function                                  |
|------|--------------------------------------------|
| LED1 | Direction indicator (ON = CCW, OFF = CW)   |
| LED2 | Movement indicator                         |
| LED3 | Emergency stop indicator                   |

---

## Button Inputs

### **Table 3 — Button Allocation**

| Button    | Function               |
|-----------|-------------------------|
| Button 1  | Select direction        |
| Button 2  | On/Off movement         |
| Button 3  | Emergency stop          |

---

## Motor Driver Connections

### **Table 4 — Motor Driver Pin Allocation**

| Pin  | Function         | Connection                     |
|------|------------------|--------------------------------|
| VM   | Motor voltage    | Power                          |
| VCC  | Logic voltage    | Power                          |
| GND  | Ground           | Motor ground only              |
| STBY | Standby          | Power                          |
| AIN1 | Channel A In 1   | Optocoupler → PB4              |
| AIN2 | Channel A In 2   | Optocoupler → PB5              |
| PWMA | PWM input        | Optocoupler → PD6              |
| A01  | Motor output +   | Motor terminal                 |
| A02  | Motor output –   | Motor terminal                 |

---

## Finite State Machine (FSM)

A **Moore-type FSM** was implemented using register-level control.  
It operates across five states with transitions driven by **external interrupts**.

### **Table 5 — FSM State Table**

| State | Next State (B1 / B2 / B3) | Outputs                       |
|--------|----------------------------|-------------------------------|
| S0 – No movement (CW)  | S1 / S2 / S4 | AIN1, AIN2                   |
| S1 – No movement (CCW) | S0 / S3 / S4 | LED1, AIN1, AIN2             |
| S2 – Moving (CW)       | S3 / S0 / S4 | LED2, AIN2                   |
| S3 – Moving (CCW)      | S2 / S1 / S4 | LED1, LED2, AIN1             |
| S4 – Emergency stop    | S0 / S0 / S4 | LED3                          |

External interrupts were configured through **EIMSK** and **EICRA** registers.

---

## Motor Driver + Optocoupler Integration

The system uses optocouplers for **full isolation** between the microcontroller and the motor driver.  
Pull-up resistors on the motor side and current-limiting resistors on the MCU side ensured proper logic levels.

### **Table 6 — Motor Driver Truth Table**

| AIN1 | AIN2 | Mode              |
|------|------|-------------------|
| High | Low  | Short brake       |
| High | High | Counter-clockwise |
| Low  | Low  | Clockwise         |
| Low  | High | Stop              |

PWM on **PWMA** controls motor speed based on the potentiometer input.

---

## Verification & Testing

Testing proceeded in stages:

1. Verified I/O using temporary Arduino code  
2. Converted to pure register-level control (DDRx, PORTx, PINx)  
3. Built and validated FSM with LEDs  
4. Tested motor driver independently  
5. Integrated optocouplers  
6. Verified interrupts + PWM  
7. Full end-to-end system testing  

The final system demonstrated:

- Accurate direction control  
- Smooth PWM speed adjustment  
- Instant emergency braking  
- Reliable interrupt-driven state changes  
- Clear LED feedback  

---

## Final Results & Reflections

The system successfully met all project requirements:

- All inputs were interrupt-driven  
- PWM speed control was smooth and stable  
- Optocouplers ensured electrical safety and isolation  
- FSM transitions operated cleanly  
- LED indicators provided intuitive feedback  


---

