/*
  Esmacat Arduino Library - Homing manager for the AGREE Exoskeleton.

  This sketch does the following:
  1) Gets the homing references, robot side, and calibration flag from the EtherCAT master if the "data_ready" boolean is activated.
  2) Stores the values in the memory (according to chosen robot_side)
  3) Sends the homing references, robot side, and calibration flag to the EtherCAT master.

  created 12 Oct 2021
  by Stefano Dalla Gasperina for the AGREE Exoskeleton.

*/
#include <LiquidCrystal.h>      // Include LCD Arduino Library
#include <Esmacatshield.h>      //Include the Esmacat Arduino Library

#define ARDUINO_UNO_SLAVE_SELECT 10      // The chip selector pin for Arduino Uno is 10 
#define CALIBRATION_SIDE_INDEX   5
#define CALIBRATION_DONE_INDEX   6
#define CALIBRATION_READY_INDEX  7
#define N_JOINTS                 5

#define RIGHT_SIDE               1
#define LEFT_SIDE                2
#define DEFAULT_SIDE             0

LiquidCrystal lcd_display(8, 9, 4, 5, 6, 7);      // Create an object for the Library class
Esmacatshield agree_ease(ARDUINO_UNO_SLAVE_SELECT);      // Create a slave object and specify the Chip Selector Pin


// EASE input registers
int agree_ease_registers[8];

// Reset counter
long int counter = 0;

// Initialize homing references
int agree_right_references[N_JOINTS] = {0, 0, 0, 0};
int agree_left_references[N_JOINTS] = {0, 0, 0, 0};

// Initialize "calibration_done" flag
int right_calibration_done = 0;
int left_calibration_done = 0;

// Initialize "data_ready" flag
int data_ready = 0;

// Initialize side
int side = 0;

void setup() {

  //lcd_display.begin(16, 2);     //Initialise the number of (coloumns, rows) in the LCD
  //lcd_display.print("AGREE-Exoskeleton");      // Print a message onto the LCD Display

  // Start SPI for EASE
  agree_ease.start_spi();

  // Initialise the Serial Communication with the specified Baud Rate
  Serial.begin(115200);
}

void loop() {

  // Read EASE registers
  agree_ease.get_ecat_registers(agree_ease_registers);

  // Get "data_ready" flag
  data_ready = agree_ease_registers[CALIBRATION_READY_INDEX];
  // Set "data_ready" flag
  agree_ease.write_reg_value(CALIBRATION_READY_INDEX, data_ready);

  // Get side
  side = agree_ease_registers[CALIBRATION_SIDE_INDEX];


  // If data is ready (from the EtherCAT master)
  if (data_ready)
  {
    // switch robot side

    switch (side) {

      case RIGHT_SIDE:
        // Get "calibration_done" flag for right side
        right_calibration_done = agree_ease_registers[CALIBRATION_DONE_INDEX];
        // Get "homing_references" values from input registers (4 slaves)
        for (int i = 0; i < N_JOINTS; i++) {
          agree_right_references[i] = agree_ease_registers[i];
        }
        break;

      case LEFT_SIDE:
        // Get "calibration_done" flag
        left_calibration_done = agree_ease_registers[CALIBRATION_DONE_INDEX];

        // Get "homing_references" values from input registers (N_JOINTS slaves)
        for (int i = 0; i < N_JOINTS; i++) {
          agree_left_references[i] = agree_ease_registers[i];
        }
        break;

      default:
        // TODO: reset homing parameters
        break;
    }
  }

  // Set side
  agree_ease.write_reg_value(CALIBRATION_SIDE_INDEX, side);

  // switch robot side
  switch (side) {
    case RIGHT_SIDE:
      // Set right "homing references" in the output registers
      for (int i = 0; i < N_JOINTS; i++) {
        agree_ease.write_reg_value(i, agree_right_references[i]);
      }
      agree_ease.write_reg_value(CALIBRATION_DONE_INDEX, right_calibration_done);
      break;

    case LEFT_SIDE:
      // Set left "homing references" in the output registers
      for (int i = 0; i < N_JOINTS; i++) {
        agree_ease.write_reg_value(i, agree_left_references[i]);
      }
      agree_ease.write_reg_value(CALIBRATION_DONE_INDEX, left_calibration_done);
      break;

    default:
      // Set "homing references" to default
      for (int i = 0; i < N_JOINTS; i++) {
        agree_ease.write_reg_value(i, 0);
      }
      agree_ease.write_reg_value(CALIBRATION_DONE_INDEX, 0);
      break;
  }

  // Increase counter for print feedback
  counter++;
  // Print feedback
  if (counter % 50 == 0)
  {
    Serial.println("INPUT regs ");
    for (int i = 0; i < N_JOINTS; i++) {
      Serial.print(agree_ease_registers[i]);
      Serial.print(" ");
    }
    Serial.print(" >> SIDE ");
    Serial.print(agree_ease_registers[CALIBRATION_SIDE_INDEX]);
    Serial.print(" >> DONE ");
    Serial.print(agree_ease_registers[CALIBRATION_DONE_INDEX]);
    Serial.println();

    Serial.println("OUTPUT RIGHT regs ");
    for (int i = 0; i < N_JOINTS; i++) {
      Serial.print(agree_right_references[i]);
      Serial.print(" ");
    }
    Serial.print(" >> ");
    Serial.print(right_calibration_done);
    Serial.println();

    Serial.println("OUTPUT LEFT regs ");
    for (int i = 0; i < N_JOINTS; i++) {
      Serial.print(agree_left_references[i]);
      Serial.print(" ");
    }
    Serial.print(" >> ");
    Serial.print(left_calibration_done);
    Serial.println();
  }
}
