# Motor Driver Project

# Objectives:

The objective of this lab was to design and implement a system to control a simple DC motor 
using a microcontroller. The system needed to include the following: 

• A user input that allows the motor to be turned on and off. 

• A user input that allows the direction of the motor to be set, whether the motor is on or 
off. 

• A user input that allows the speed of the motor to be varied. 

• A user controllable emergency stop / quick brake feature to quickly stop the system. 

• Indicators that clearly display whether the system is on or off, the direction of the motor, 
and whether the emergency brake has been engaged. 

• To meet reliability requirements, interrupts should be used to detect changes to the user 
inputs and an opto-coupler should be used to separate the motor ground from the MCU 
ground.



# Design process:

The design was divided into four main categories: I/O, FSM, motor control, and upper-level 
components. For the I/O, three input and three output pins were allocated for the FSM, three pins 
for the motor driver, and one pin for the potentiometer. The button inputs were placed at pins 2, 
3, and 4 to utilize the built-in external interrupts implemented later. Pin 6 was reserved for the 
motor outputs, as an analog output was required to override with the PWM. Pin A0 was used as 
the analog input for the potentiometer, and the LEDs were assigned to any remaining available 
pins. 

The finite state machine was implemented next, using a Moore-type configuration with three 
button inputs and three LED outputs; motor control outputs were added later. The machine was 
designed with five states that cycle through different movements and directions. 
The motor control unit was then added to enable outputs from the MCU to drive the motor. This 
process began with wiring the control unit separately from the MCU to determine the motor's 
polarity and functionality. Once verified, opto-couplers for the AIN1 and AIN2 inputs were 
added to allow the FSM outputs to control motor movement. 

Finally, the upper-level components were implemented. This included transitioning to interrupt
based input handling, initializing PWM timers, and incorporating speed control through a 
potentiometer. A dedicated PWM timer was configured for motor control to minimize humming 
and vibration. The potentiometer was connected as an analog input to the Arduino, enabling 
direct control over motor speed. 


