/*
  Liquid Crystal & Esmacat Arduino Library - Printing the Latest Button Pressed

  This sketch does the following:
  1) Gets the analog value of the button pressed on the LCD Shield and
     sends it to the EtherCAT master using EASE (EtherCAT Arduino Shield by Esmacat).
  2) Based on the Input from the master, prints the corresponding message on the LCD.

  created 15 Dec 2019
  by Harmonic Bionics Inc. (https://www.harmonicbionics.com/).


*/


/*
    PIN CONFIGURATION
    |-----------------------------------------|
    |  LCD PIN Number  |   Arduino PIN Number |
    |-----------------------------------------|
    |       RS         |      Digital Pin 8   |
    |     ENABLE       |      Digital Pin 9   |
    |       D4         |      Digital Pin 4   |
    |       D5         |      Digital Pin 5   |
    |       D6         |      Digital Pin 6   |
    |       D7         |      Digital Pin 7   |
    |     Buttons      |      Analog Pin A0   |
    |-----------------------------------------|

*/

/*
  Analog Input Values for the Push Buttons
  |------------------------------------------------|
  |    Push Button     |          A0 value         |
  |------------------------------------------------|
  |       SELECT       |   val >= 500 & val <= 750 |
  |        LEFT        |   val >= 300 & val < 500  |
  |         UP         |   val >= 50  & val < 150  |
  |        DOWN        |   val >= 150 & val < 300  |
  |        RIGHT       |   val >= 0   & val < 50   |
  |------------------------------------------------|
*/

#include <LiquidCrystal.h>      // Include LCD Arduino Library
#include <Esmacatshield.h>      //Include the Esmacat Arduino Library

#define ARDUINO_UNO_SLAVE_SELECT 10      // The chip selector pin for Arduino Uno is 10 

LiquidCrystal lcd_display(8, 9, 4, 5, 6, 7);      // Create an object for the Library class
Esmacatshield ease_1(ARDUINO_UNO_SLAVE_SELECT);      // Create a slave object and specify the Chip Selector Pin


int ease_registers[8];      // EASE 8 registers

int analog_value;      // Initialise analog input value variable
String data;      // Initialize the data variable
int button_pressed;      // Encoded value for the buttons
long int counter = 0;

int calibration_reference[5] = {0,0,0,0,0};
int calibration_done = 0;
bool data_ready;

// AGREE Mode
int mode;

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

  lcd_display.print("AGREE Exoskeleton");      // Print a message onto the LCD Display

  ease_1.start_spi();      // Start SPI for EASE

  Serial.begin(9600);      // Initialise the Serial Communication with the specified Baud Rate
}

void loop() {

  // Code to print the button pressed on the LCD to the Serial Monitor
  analog_value = analogRead(A0);

  ease_1.get_ecat_registers(ease_registers);


//  Serial.print(ease_registers[1]);
//  Serial.print(\t);
//  Serial.println(analog_value);

  data_ready = ease_registers[7];
  
  if(data_ready) 
 {  
    calibration_done = ease_registers[6];
    for(int i=0;i<5;i++){
      calibration_reference[i] = ease_registers[i];  // Receive the encode the value for register
  }
 }

  counter++;
  if(counter%50==0)
  {
    Serial.println("INPUT regs >> STORED regs >> Calibrated? ");
    for(int i=0;i<5;i++){
      Serial.print(ease_registers[i]);     //Write register data (register,value, led_on)
      Serial.print(" ");
    }
    Serial.print(" >> ");
    for(int i=0;i<5;i++){
    Serial.print(calibration_reference[i]);     //Write register data (register,value, led_on)
    Serial.print(" ");
    }
    Serial.print(" >> ");
    Serial.print(calibration_done);  
    Serial.println();
  }


  // Set OUTPUT Values
  for(int i=0;i<5;i++){
      ease_1.write_reg_value(i, calibration_reference[i]);     //Write register data (register,value, led_on)
  }
  ease_1.write_reg_value(6, calibration_done);



  
  

//  Serial.println(analog_value);
//
//  for (int i = 0; i < 16; i++)
//  {
//    lcd_display.setCursor(i, 0);
//    lcd_display.print(" ");      // Remove the previous characters (if any)
//  }
//
//  lcd_display.setCursor(0, 0);
//  lcd_display.print("Mode: ");
//  lcd_display.print(mode);
//
//  if ( analog_value < 750)
//  {
//    lcd_display.setCursor(0, 1);
//    lcd_display.print("Button Pressed");
//    ease_1.write_reg_value(0, 0);     //Write register data (register,value, led_on)
//
//  }
//  else {
//    for (int i = 0; i < 16; i++)
//    {
//      lcd_display.setCursor(i, 1);
//      lcd_display.print(" ");      // Remove the previous characters (if any)
//    }
//    ease_1.write_reg_value(0, analog_value);     //Write register data (register,value, led_on)
//
//  }

}
