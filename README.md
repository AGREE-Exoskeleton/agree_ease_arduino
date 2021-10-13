# AGREE Arduino Sketch for Homing Management

Arduino sketch for saving AGREE homing reference values on the Arduino+EASE, according to the robot's side.

  This sketch does the following:
  1) Gets the homing references, robot side, and calibration flag from the EtherCAT master if the "data_ready" boolean is activated.
  2) Stores the values in the memory (according to chosen robot_side)
  3) Sends the homing references, robot side, and calibration flag to the EtherCAT master.

## Requirement

- Requires Arduino IDE
- Requires the <EsmacatShield.h> arduino library (EASE_ArduinoCode in Library Manager)

## Instructions

- Compile the sketch
- Upload the sketch to the Arduino Board
- Set the robot side to "1" for right and "2" for left. "0" resets the homing references.
- Once "data_ready" is set to one, homing references are stored in memory.
- To retrieve homing references set the correct robot side and read corresponding EASE output EtherCAT packets.



