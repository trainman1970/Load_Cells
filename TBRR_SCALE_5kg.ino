/* TBRR Railroad - Thunder Hole Mine Tipple loading Car Weighing System
 *  
 *  I2C LCD 20x4 Display for Sign on TBRR

  Took library and samples coding from:
  https://www.youtube.com/watch?v=xVC0X_PE_XE&list=LLUtoezERLgkGYxIu25yF9TQ&index=8

  Library Source:
  https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home#!downloading-and-installation

  I2C Device Scanner:  I2C_Scanner.ino
  http://playground.arduino.cc/Main/I2cScanner

  Added Load Beams to measure car weight after Tipple

  DW Bolt 1-10-2018

*/
#include <elapsedMillis.h>
elapsedMillis timeElapsed; //declare global if you don't want it reset every time loop runs

#include <Wire.h>
#include <LiquidCrystal_I2C.h>     //  Library for I2C LCD Display  (Analog A4 A5 I2C)

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  //  The Display I2C address 0x3F found by scanner

#include "Arduino.h"
#include "HX711.h"        //  Library to use HX711 strain gage converter


#define DOUT1  3        //  Load Cell A pin connections
#define CLK1  2

#define DOUT2  5        //  Load Cell B pin connections
#define CLK2  4

#define sensorA 7       //  IR sensor pin input connection

// Create our LoadCell object
HX711 scale1(DOUT1, CLK1);
HX711 scale2(DOUT2, CLK2);

float seed_1 = -380;    //  Calibration for Load Cell A
float seed_2 = -363;    //  Calibration for Load Cell B

float weight_1 = 0.0;             //  weight of car from scale A
float weight_2 = 0.0;             //  weight of car from scale B
float car_weight[20];             //  sets up array for all car weights
float total_car_weight;           //  variable for total car weight - sum of both load cells
float tonnage = 0.315261;         //  conversion variable from grams to tons (453.59237 - 143 tons)

unsigned int carnum = 0;          //  car number files
unsigned int totalweight = 0;     //  total train weight
unsigned int lightweight = 32;    //  lightweight of coal car in tons
int numloco = 3;                  //  number of locomotives
float locolength = 78.75;         //  length of loco in feet
float locohp = 12000.;            //  total loco horsepower

boolean report = true;            //  boolean variable for knowing when to print individual car report

int total_car_numbers = 0;        //  variable to keep track of total number of loaded cras



// --------------------------------------------------------------------------------------
// SET UP  SET UP   SET UP   SET UP   SET UP   SET UP   SET UP   SET UP   SET UP
// --------------------------------------------------------------------------------------

void setup() {

  pinMode(sensorA, INPUT);   //  Define IR module input - LOW is ON, HIGH is OFF

  lcd.begin(20, 4);      //  Start lcd library

  scale1.set_gain(128);               //  this established channel A load cell
  scale1.set_scale();
  scale1.tare();  //Reset the scale to 0

  weight_1 = scale1.get_units(10);
  //Adjust to this calibration factor
  scale1.set_scale(seed_1);

  scale2.set_gain(128);               //  this established channel B load cell
  scale2.set_scale();
  scale2.tare();  //Reset the scale to 0

  weight_2 = scale2.get_units(10);
  //Adjust to this calibration factor
  scale2.set_scale(seed_2);

// ------------------------------
//     Display Welcome Screen
// ------------------------------

  lcd.clear();
  lcd.setCursor(0, 0);     //  Set cursor to Start position on top row
  lcd.print("Welcome to TBRR");
  lcd.setCursor(0, 1);     //  Set cursor to second row first position
  lcd.print("Sweetwater City");
  lcd.setCursor(0, 2);     //  Set cursor to Start position on top row
  lcd.print("ThunderHole Mine");
  lcd.setCursor(0, 3);     //  Set cursor to second row first position
  lcd.print("Tipple Load Station");

  delay(5000);
}

// --------------------------------------------------------------------------------------
// LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP     LOOP
// --------------------------------------------------------------------------------------

void loop() {

  timeElapsed = 0;      //  set timer to zero for time weighing car

  while (digitalRead(sensorA) == HIGH)   // sensor imput indicates no car present - car is on scale track
  {
    report = true;                       // set up report indicator for the car weight

    //  capture Load Cell A reading
    weight_1 = scale1.get_units(10);     // take 10 measurements and get average
    scale1.set_scale(seed_1);            // Adjust to this calibration factor

    //  capture Load Cell B reading
    weight_2 = scale2.get_units(10);     // take 10 measurements and get average
    scale2.set_scale(seed_2);            //Adjust to this calibration factor

    car_weight[carnum] = weight_1 + weight_2;   //  place total car weight into cae_weight array
    total_car_weight = weight_1 + weight_2;     //  use variable for total car weight

    if (timeElapsed >= 10000)   //  check for no more cars
    {
      report = false;                    //  set report indicator to false so car report section does not run
      total_car_numbers = carnum + 1;    // Calculate total number of cars loaded
      break;    //  Exit from While loop for weighing cars
    }

  }  //  END READING CAR WEIGHT

  //    SEND INDIVIDUAL CAR REPORT TO LCD SCREEN

  if (report == true)          //  Report indicator will be true if car just weighed
  {
    char result_1[8]; // Buffer big enough for 7-character float
    dtostrf(weight_1, 6, 1, result_1); // Leave room for too large numbers!   //  converts decimal to string

    lcd.clear();
    lcd.setCursor(0, 0);     //  Set cursor to Start position on top row
    lcd.print("Tipple Hole Mine");
    lcd.setCursor(0, 1);     //  Set cursor to Start position on third row
    lcd.print("Load Cell A");
    lcd.setCursor(0, 2);     //  Set cursor to second row first position
    lcd.print("WEIGHT = ");
    lcd.setCursor(9, 2);     //  Set cursor to second row 10 position position
    lcd.print(result_1);
    delay(2000);

    char result_2[8]; // Buffer big enough for 7-character float
    dtostrf(weight_2, 6, 1, result_2); // Leave room for too large numbers!   //  converts decimal to string

    lcd.clear();
    lcd.setCursor(0, 0);     //  Set cursor to Start position on top row
    lcd.print("Tipple Hole Mine");
    lcd.setCursor(0, 1);     //  Set cursor to Start position on third row
    lcd.print("Load Cell B");
    lcd.setCursor(0, 2);     //  Set cursor to second row first position
    lcd.print("WEIGHT = ");
    lcd.setCursor(9, 2);     //  Set cursor to second row 10 position position
    lcd.print(result_2);
    delay(2000);

    char result_3[8]; // Buffer big enough for 7-character float
    dtostrf(total_car_weight, 6, 1, result_3); // Leave room for too large numbers!   //  converts decimal to string

    lcd.clear();
    lcd.setCursor(0, 0);     //  Set cursor to Start position on top row
    lcd.print("Car Number: ");
    lcd.setCursor(13, 0);
    lcd.print(carnum + 1);
    lcd.setCursor(0, 1);     //  Set cursor to second row first position
    lcd.print("WEIGHT = ");
    lcd.setCursor(9, 1);     //  Set cursor to second row 10 position position
    lcd.print(result_3);
    lcd.setCursor(13, 1);     //  Set cursor to second row 10 position position
    lcd.print("Grams");
    delay(3000);

    carnum = carnum + 1;   // increment car number for next car loading measurement

    report = false;        //  set report indicator false, so no more reports for this car
    
  }   //  END OF REPORT



  // -----------------------------------
  // Print Final report for loaded Train
  // -----------------------------------

  if (timeElapsed >= 10000)  //  if time elapsed > 10 seconds, there are no more cars to weigh
  {

    for (int carnext = 0; carnext <= total_car_numbers - 1; carnext++)  // run loop to get total train weight of loaded cars
    {
      totalweight = car_weight[carnext] + totalweight;
    }

    unsigned int trainweight = totalweight * tonnage;  // calculation to convert grams to tons
    unsigned int coal_tonnage;
    coal_tonnage = trainweight - (total_car_numbers * 32);
    int trainlength = ((carnum - 1) * 51.4) + (numloco * locolength);
    float hptt = locohp / trainweight;

    lcd.clear();
    lcd.setCursor(0, 0);     //  Set cursor to Start position on top row
    lcd.print("# of Cars     = ");
    lcd.setCursor(16, 0);    //  Set cursor to first row 14 position
    lcd.print(total_car_numbers);
    lcd.setCursor(0, 1);     //  Set cursor to second row first position
    lcd.print("Trailing Tons = ");
    lcd.setCursor(16, 1);    //  Set cursor to second row 12 position
    lcd.print(trainweight);
    lcd.setCursor(0, 2);     //  Set cursor to Start position on top row
    lcd.print("Length = ");
    lcd.setCursor(9, 2);     //  Set cursor to first row 9 position
    lcd.print(trainlength);
    lcd.setCursor(14, 2);   //  Set cursor to first row 14 position
    lcd.print("Ft");
    lcd.setCursor(0, 3);     //  Set cursor to second row first position
    lcd.print("HP/TT  = ");
    lcd.setCursor(9, 3);    //  Set cursor to second row 9 position
    lcd.print(hptt);
    delay(8000);

    lcd.clear();
    lcd.setCursor(0, 0);     //  Set cursor to Start position on top row
    lcd.print("CUSTOMER REPORT");
    lcd.setCursor(0, 1);     //  Set cursor to second row first position
    lcd.print("Coal Tons = ");
    lcd.setCursor(12, 1);    //  Set cursor to second row 12 position
    lcd.print(coal_tonnage);
    delay(5000);

  }

}     // END OF VOID LOOP
