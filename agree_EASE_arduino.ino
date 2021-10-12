/*
  Esmacat Arduino Library - Homing manager for the AGREE Exoskeleton.

  This sketch does the following:
  1) Gets the homing references, robot side, and calibration flag from the EtherCAT master if the "data_ready" boolean is activated.
  2) Stores the values in the memory.
  3) Sends the homing references, robot side, and calibration flag to the EtherCAT master.

  created 10 Oct 2021
  by Stefano Dalla Gasperina for the AGREE Exoskeleton.

*/
#include <LiquidCrystal.h>      // Include LCD Arduino Library
#include <Esmacatshield.h>      //Include the Esmacat Arduino Library

#define ARDUINO_UNO_SLAVE_SELECT 10      // The chip selector pin for Arduino Uno is 10 
#define CALIBRATION_SIDE_INDEX   5
#define CALIBRATION_DONE_INDEX   6
#define CALIBRATION_READY_INDEX  7

LiquidCrystal lcd_display(8, 9, 4, 5, 6, 7);      // Create an object for the Library class
Esmacatshield agree_ease(ARDUINO_UNO_SLAVE_SELECT);      // Create a slave object and specify the Chip Selector Pin


// EASE input registers
int agree_ease_registers[8];  

// Reset counter
long int counter = 0;

// Initialize homing references
int agree_homing_references[5] = {0,0,0,0,0};

// Initialize "calibration_done" flag
int calibration_done = 0;

// Initialize "data_ready" flag
bool data_ready = false;


/*
    -------------------------------------------
    |    Button on LCD     |    Encoded Value  |
    -------------------------------------------
    |         Left         |          1        |
    |          Up          |          2        |
    |         Down         |          3        |
    |         Right        |          4        |
    |        Select        |          5        |
    --------------------------------------------
*/

void setup() {

  lcd_display.begin(16, 2);     //Initialise the number of (coloumns, rows) in the LCD
  lcd_display.print("AGREE-Exoskeleton");      // Print a message onto the LCD Display

  // Start SPI for EASE
  agree_ease.start_spi();      

  // Initialise the Serial Communication with the specified Baud Rate
  Serial.begin(9600);      
}

void loop() {

  // Read EASE registers
  agree_ease.get_ecat_registers(agree_ease_registers);

  // Get "data_ready" flag
  data_ready = agree_ease_registers[CALIBRATION_READY_INDEX];

  // If data is ready (from the EtherCAT master)
  if(data_ready) 
 {  
    // Get "calibration_done" flag
    calibration_done = agree_ease_registers[CALIBRATION_DONE_INDEX];

    // Get "homing_references" values from input registers (5 slaves)
    for(int i=0;i<5;i++){
      agree_homing_references[i] = agree_ease_registers[i];  
  }
 }
  
  // Set "homing references" in the output registers
  for(int i=0;i<5;i++){
      agree_ease.write_reg_value(i, agree_homing_references[i]);   
  }
  
  agree_ease.write_reg_value(CALIBRATION_DONE_INDEX, calibration_done);

  // Increase counter for print feedback
  counter++;
  // Print feedback
  if(counter%50==0)
  {
    Serial.println("INPUT regs >> STORED regs >> DONE? reg ");
    for(int i=0;i<5;i++){
      Serial.print(agree_ease_registers[i]);     
      Serial.print(" ");
    }
    Serial.print(" >> ");
    for(int i=0;i<5;i++){
    Serial.print(agree_homing_references[i]);     
    Serial.print(" ");
    }
    Serial.print(" >> ");
    Serial.print(calibration_done);  
    Serial.println();
  }

}
