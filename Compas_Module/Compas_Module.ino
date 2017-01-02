
#include "ledDisplay.h"
#include "config.h"
#include <Wire.h>
#include <HMC5883L.h>
#include <MPU6050.h>

HMC5883L compass;
MPU6050 mpu;

float heading1;
float correct;

void setup() {
  Serial.begin(9600);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);

  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    delay(500);
  }

// Enable bypass mode
  mpu.setI2CMasterModeEnabled(false);
  mpu.setI2CBypassEnabled(true) ;
  mpu.setSleepEnabled(false);

// Initialize Initialize HMC5883L
  while (!compass.begin())
  {
    delay(500);
  }

  // Set measurement range
  compass.setRange(HMC5883L_RANGE_1_3GA);

  // Set measurement mode
  compass.setMeasurementMode(HMC5883L_CONTINOUS);

  // Set data rate
  compass.setDataRate(HMC5883L_DATARATE_30HZ);

  // Set number of samples averaged
  compass.setSamples(HMC5883L_SAMPLES_8);

  // Set calibration offset. See HMC5883L_calibration.ino
  compass.setOffset(0, 0); 


//Showoff led display  
  for (int j = 0; j < 16; j++){
    registerWrite(windRichting[j]);
    delay(100);
  }
  
  registerWrite(255); //all led on
  delay(500);
  registerWrite(0); //all led off
}


void loop() {
  // Read vectors
  Vector mag = compass.readNormalize();
  Vector acc = mpu.readScaledAccel();  

  // Calculate heading1s
  heading1 = tiltCompensate(mag, acc);
  
  heading1 += declinationAngle;
  
  // Correct for heading1 < 0deg and heading1 > 360deg
  heading1 = correctAngle(heading1);

  // Convert to degrees
  heading1 = heading1 * 180/M_PI; 

  // Output
  Serial.println(heading1);
  showLedDisplay(heading1);

  delay(100);
  
  }

     // Tilt compensation
float tiltCompensate(Vector mag, Vector normAccel)
{
  // Pitch & Roll 
  
  float roll;
  float pitch;
  
  roll = asin(normAccel.YAxis);
  pitch = asin(-normAccel.XAxis);

  if (roll > 0.78 || roll < -0.78 || pitch > 0.78 || pitch < -0.78)
  {
    return -1000;
  }
  
    // Some of these are used twice, so rather than computing them twice in the algorithem we precompute them before hand.
  float cosRoll = cos(roll);
  float sinRoll = sin(roll);  
  float cosPitch = cos(pitch);
  float sinPitch = sin(pitch);
  
  // Tilt compensation
  float Xh = mag.XAxis * cosPitch + mag.ZAxis * sinPitch;
  float Yh = mag.XAxis * sinRoll * sinPitch + mag.YAxis * cosRoll - mag.ZAxis * sinRoll * cosPitch;
 
  float heading1 = atan2(Yh, Xh);
    
  return heading1;
}

// Correct angle
float correctAngle(float heading1)
{
  if (heading1 < 0) { heading1 += 2 * PI; }
  if (heading1 > 2 * PI) { heading1 -= 2 * PI; }

  return heading1;
}

