/****************************************************************
 * Example1_Basics.ino
 * ICM 20948 Arduino Library Demo
 * Use the default configuration to stream 9-axis IMU data
 * Owen Lyke @ SparkFun Electronics
 * Original Creation Date: April 17 2019
 *
 * Please see License.md for the license information.
 *
 * Distributed as-is; no warranty is given.
 ***************************************************************/
#include "ICM_20948.h" // Click here to get the library: http://librarymanager/All#SparkFun_ICM_20948_IMU
#include "Wire.h"

#define SERIAL_PORT Serial


#define WIRE_PORT Wire // Your desired Wire port.      Used when "USE_SPI" is not defined
// The value of the last bit of the I2C address.
// On the SparkFun 9DoF IMU breakout the default is 1, and when the ADR jumper is closed the value becomes 0
#define AD0_VAL 1
float adjX_AD01 = 6;
float adjY_AD01 = -13;
float adjZ_AD01 = 28;
float adjX_AD00 = 24;
float adjY_AD00 = -21;
float adjZ_AD00 = -2;
ICM_20948_I2C myICM0; // Otherwise create an ICM_20948_I2C object
ICM_20948_I2C myICM1; // Otherwise create an ICM_20948_I2C object
int rCount;
int lCount;
int bCount;
int fCount;
float prev_accX;
float prev_accY;
float prev_accZ;
void setup()
{
  float prev_accX = 4;
  float prev_accY = 4;
  float prev_accZ = 4;
  int rCount = 0;
  int lCount = 0;
  int bCount = 0;
  int fCount = 0;
  SERIAL_PORT.begin(115200);
  while (!SERIAL_PORT)
  {
  };
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  WIRE_PORT.begin();
  WIRE_PORT.setClock(400000);

  //myICM0.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  bool initialized = false;
  while (!initialized)
  {
    myICM0.begin(WIRE_PORT, AD0_VAL);
    myICM1.begin(WIRE_PORT, !AD0_VAL);
    SERIAL_PORT.print(F("Initialization of the sensor returned: "));
    SERIAL_PORT.println(myICM0.statusString());
    //if (myICM1.status != ICM_20948_Stat_Ok)
    if (myICM0.status != ICM_20948_Stat_Ok || myICM1.status != ICM_20948_Stat_Ok)
    {
      SERIAL_PORT.println("Trying again...");
      delay(500);
    }
    else
    {
      initialized = true;
    }
  }
}

void loop()
{
  if (myICM0.dataReady())
  {
    myICM0.getAGMT();         // The values are only updated when you call 'getAGMT'
                             //    printRawAGMT( myICM0.agmt );     // Uncomment this to see the raw values, taken directly from the agmt structure
    // SERIAL_PORT.println("0: ");
    // printScaledAGMT(&myICM0, 0); // This function takes into account the scale settings from when the measurement was made to calculate the values with units
    delay(30);
  }
  else
  {
    SERIAL_PORT.println("Waiting for data");
    delay(500);
  }
    if (myICM1.dataReady())
  {
    myICM1.getAGMT();         // The values are only updated when you call 'getAGMT'
                             //    printRawAGMT( myICM1.agmt );     // Uncomment this to see the raw values, taken directly from the agmt structure
    // SERIAL_PORT.println("1: ");
    // printScaledAGMT(&myICM1, 1); // This function takes into account the scale settings from when the measurement was made to calculate the values with units
    delay(30);
  }
  else
  {
    SERIAL_PORT.println("Waiting for data");
    delay(500);
  }

  float yz0 = atan2(myICM0.magY() + adjY_AD01, myICM0.magZ() + adjZ_AD01) * 180/PI;
  float yz1 = atan2(myICM1.magY() + adjY_AD00, myICM1.magZ() + adjZ_AD00) * 180/PI;
  float xy0 = atan2(myICM0.magX() + adjX_AD01, myICM0.magY() + adjY_AD01) * 180/PI;
  float xy1 = atan2(myICM1.magX() + adjX_AD00, myICM1.magY() + adjY_AD00) * 180/PI;
  float xz0 = atan2(myICM0.magX() + adjX_AD01, myICM0.magZ() + adjZ_AD01) * 180/PI;
  float xz1 = atan2(myICM1.magX() + adjX_AD00, myICM1.magZ() + adjZ_AD00) * 180/PI;
  float yz = 180-abs(abs(yz0 - yz1) - 180);
  float xy = 180-abs(abs(xy0 - xy1) - 180);
  float xz = 180-abs(abs(xz0 - xz1) - 180);
  SERIAL_PORT.println(yz);
  SERIAL_PORT.println(xz);
  SERIAL_PORT.println(xy);
  float acc_diff = ((prev_accX - myICM0.accX()) * (prev_accX - myICM0.accX()) + (prev_accY - myICM0.accY()) * (prev_accY - myICM0.accY()) + 
                   (prev_accZ - myICM0.accZ()) * (prev_accZ - myICM0.accZ()))/10000;
  SERIAL_PORT.println(acc_diff);
  if (acc_diff > 400) {
    for (int blinks = 0; blinks < 5; blinks++) {
      digitalWrite(7, HIGH);
      digitalWrite(6, HIGH);
      delay(500);
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);
      delay(500);
    }
  } else if (yz >= 45 && yz <= 140 && xz >= 55 && xz <= 135 && xy >= 55 && xy <= 130) {
    rCount++;
    bCount = 0;
    lCount = 0;
    fCount = 0;
    SERIAL_PORT.println("RIGHT");
    if (rCount > 4) {
      if (rCount % 10 >= 5) {
        digitalWrite(7, HIGH);
        digitalWrite(6, LOW);
      } else {
        digitalWrite(7, LOW);
        digitalWrite(6, LOW);
      }
    }
  } else if (yz <= 40 && xz <= 70 && xy <= 70){
    rCount = 0;
    bCount++;
    lCount = 0;
    fCount = 0;
    if (bCount > 4) {
      digitalWrite(7, HIGH);
      digitalWrite(6, HIGH);
    }
    SERIAL_PORT.println("BREAK");
  } else if (yz >= 85 && yz <= 150 && xz >= 75 && xz <= 135 && xy >= 130){
    //yz = 90, xz = 0, xy = 90
    rCount = 0;
    bCount = 0;
    lCount++;
    fCount = 0;
    if (lCount > 4) {
      if (lCount % 10 >= 5) {
        digitalWrite(7, LOW);
        digitalWrite(6, HIGH);
      } else {
        digitalWrite(7, LOW);
        digitalWrite(6, LOW);
      }
    }
    SERIAL_PORT.println("LEFT");
  } else {
    rCount = 0;
    bCount = 0;
    lCount = 0;
    fCount++;
    if (fCount > 2) {
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);
    }
    SERIAL_PORT.println("FALSE");
  }
  prev_accX = myICM0.accX();
  prev_accY = myICM0.accY();
  prev_accZ = myICM0.accZ();
  delay(100);
}