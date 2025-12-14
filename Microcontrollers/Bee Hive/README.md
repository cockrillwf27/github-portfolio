# Artificial Beehive Project Summary

---

## Purpose

This project creates an electronic system to protect fence lines by using elephants’ natural fear of bees, eliminating the need for real beehive maintenance. After detecting elephant proximity or fence disturbance via sensors, the system plays realistic bee sounds, with the volume and pattern of the sounds changing based on detection behavior to effectively scare away elephants.

---

## Design and Performance Objectives

### Design Objectives

- Ability to operate off a battery pack, utilize standby mode, and log events  
- User input for arming and unarming the hive  
- Indicators for standby, uptime, hive armed, and hive in combat states  
- Communication between multiple MCUs  
- Detection of motion outside of the hive  
- Detection of hive movement or free-fall conditions  
- Ability to imitate bees and adapt behavior based on external triggers  

### Performance Objectives

- Base station reliably controls and receives real-time status from substation units up to **100 ft** away  
- Standby/sleep mode current draw between **0.5 mA and 5 mA**

---

## System Highlights and Implementation Overview

### Architecture

One master Arduino-based control station with LCD and SD logging wirelessly commands and monitors a substation unit using an **XBee** radio link.

### Key Components

- Arduino UNO  
- XBee PRO S2B  
- DFPlayer Mini + 3W 4Ω speaker  
- MMA8452Q accelerometer  
- PIR motion sensor  
- 2004A 1.3V I2C LCD  
- MicroSD card  
- 8S (9.6V nominal) nickel-metal hydride battery  

### Innovations

- Adaptive **“angry bee” escalation algorithm** using retrigger counting and timed decay  
- **Single-byte command** and **two-byte status protocol** to minimize wireless traffic  
- Single master button handling wake, arm, disarm, and system-wide reboot  

### Implementation Steps

1. Prototyped a single substation unit with PIR, accelerometer, and DFPlayer audio playback  
2. Added XBee communication, status reporting, and centralized LCD display with event logging  
3. Implemented power-down sleep mode, interrupt-based wake, and adaptive audio escalation logic  

---

## Results and Performance Summary

The completed system reliably wakes from deep sleep, transitions in and out of armed states, detects motion and vibration, escalates bee sound playback appropriately, and communicates over long distances with minimal crashes after extensive testing.

### Measured Data

- **Wireless range:** Reliable two-way communication at over **180 ft**  
- **Current draw:**  
  - 1.04 mA in standby  
  - 9 mA when armed  
  - 17 mA when in combat  

### Comparison to Objectives

All functional design objectives and performance targets were fully met or exceeded during controlled testing.

---

## Organization of Additional Reports

- **High Level Summary Narrative:** Resources for the final integrated system implementation  
- **System Team Report:** Team contributions, schematics, and findings  
- **Detection Team Report:** Detection subsystem contributions, schematics, and findings  
- **Scare Team Report:** Audio and deterrence subsystem contributions, schematics, and findings  

---

## System Image

**Figure 1.** Substation prototype  

---

## Video Demonstration

[Narrated system walkthrough](596EC2B6-D350-42FB-A7E5-70618DAF63EC.MP4)

