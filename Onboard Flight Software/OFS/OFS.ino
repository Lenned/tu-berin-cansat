#include <Wire.h> // I2C library
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Servo.h> 

/* Constants */
// MPU-6050
const int MPU_6050_addr = 0x68; // I2C address of the MPU-6050
const byte PWR_MGMT_1 = 0x6B; // PWR_MGMT_1 register address of the MPU-6050
const byte ACCEL_CONFIG = 0x1C; // ACCEL_CONFIG register address of the MPU-6050
const byte ACCEL_XOUT_H = 0x3B; // ACCEL_XOUT_H register address of the MPU-6050
const byte GYRO_CONFIG = 0x1B; // GYRO_CONFIG register address of the MPU-6050
const int lm35_pin = A0;
const int servoPin = 8; 
const int motorPin = 11; 

/* Variables */
// MPU-6050
float MPU_6050_AccX;
float MPU_6050_AccY;
float MPU_6050_AccZ;
float MPU_6050_Temp;
float MPU_6050_GyroX;
float MPU_6050_GyroY;
float MPU_6050_GyroZ;
float extTemp;

// BMP280
Adafruit_BMP280 bme;
float bmeTemp;
float bmePressure;
float bmeAltitude;

/* Slopes */
float AccXSlope;
float AccYSlope;
float AccZSlope;
float GyroXSlope;
float GyroYSlope;
float GyroZSlope;
float pressureSlope;
float altitudeSlope;

float accXCalibration = 0;
float accYCalibration = 0;
float accZCalibration = 0;
float gyroXCalibration = 0;
float gyroYCalibration = 0;
float gyroZCalibration = 0;

unsigned long prevTime;
unsigned long currTime;
float AccXPrev;
float AccYPrev;
float AccZPrev;
float GyroXPrev;
float GyroYPrev;
float GyroZPrev;
float pressurePrev;
float altitudePrev;

Servo Servo1;

int tele = 1;
int motorOn = 0;
int parachuteOut = 0;
int autoChange = 1;
int armed = 0;
unsigned long liveTime = 0;
unsigned long armedTime = 0;
unsigned long missionTime = 0;
int intialAltitude = 0;
unsigned long drivingStartTime = 0;
int firstLanding = 0;

int didReleaseParachute = 0;
int didDrive = 0;

/*
 * 0: Unarmed
 * 1: Pre-Flight
 * 2: Flight
 * 3: Landed
 * 4: Post-Landing
 * 5: Manual Control
 */
int flightState = 0;

void updateState() {
  if(autoChange == 1) {
    switch (flightState) {
    case 0: // Unarmed -> Pre-Flight
      if(armed == 1) {
        intialAltitude = bmeAltitude;
        flightState = 1;
        armedTime = liveTime;
      }
      break;
    case 1: // Pre-Flight -> Flight
      flightState = 2;
      break;
    case 2: // Flight -> Landed
      if(missionTime > 360000) {
        flightState = 3;
      }
      break;
    case 3: // Landed -> Post-Landing
      /*
      if(didReleaseParachute == 1 && didDrive == 1) {
        flightState = 4;
      }
      */
      break;
    case 4: // Post-Landing
      firstLanding = 0;
      break;
    case 5: // Manual Control
      break;
    default:
      break;
    }
  }
}

void calibrateAll() {
  accXCalibration = 0 - MPU_6050_AccX;
  accYCalibration = 0 - MPU_6050_AccY;
  accZCalibration = 0 - MPU_6050_AccZ;
  gyroXCalibration = 0 - MPU_6050_GyroX;
  gyroYCalibration = 0 - MPU_6050_GyroY;
  gyroZCalibration = 0 - MPU_6050_GyroZ;
}

void resetCalibration() {
  accXCalibration = 0;
  accYCalibration = 0;
  accZCalibration = 0;
  gyroXCalibration = 0;
  gyroYCalibration = 0;
  gyroZCalibration = 0;
}

void receiveUplink() {
  if(Serial.available() > 0) {
   char input = Serial.read();
   if(input == 'A') {
    resetCalibration();
   } else if(input == 'B') {
    calibrateAll();
   } else if(input == 'C') {
    flightState = 5;
    autoChange = 0; 
    finishDriving();
   } else if(input == 'D') {
    armed = 1;
   } else if(input == 'E') {
    flightState = 1;
   } else if(input == 'F') {
    flightState = 2;
   } else if(input == 'G') {
    flightState = 3;
   } else if(input == 'H') {
    flightState = 4;
   } else if(input == 'I') {
    releaseParachute();
   } else if(input == 'J') {
    attachParachute(); 
   } else if(input == 'K') {
    beginDriving();
   } else if(input == 'L') {
    finishDriving();
   } else if(input == 'M') {
    Servo1.write(0);
   } else if(input == 'N') {
    Servo1.write(45);
   } else if(input == 'O') {
    Servo1.write(90);
   }
  }
}

void sendTelemetry() {
  if(tele == 1) {
      Serial.println("A," + String(MPU_6050_AccX, 2) + "," + String(MPU_6050_AccY, 2) + "," + String(MPU_6050_AccZ, 2) + "," + String(MPU_6050_GyroX,2 ) + "," + String(MPU_6050_GyroY, 2) + "," + String(MPU_6050_GyroZ, 2) + "," + String(MPU_6050_Temp, 2) + "," + String(bmeTemp, 2) + "," + String(bmePressure, 2) + "," + String(bmeAltitude, 2) + "," + String(extTemp, 2) + "," + String(flightState) + "," + String(liveTime) + "," + String(missionTime));
      // Serial.println("B," + String(motorOn) + "," + String(parachuteOut) + "," + String(autoChange) + "," + String(isDriving) + "," + String(didDrive) + "," + String(drivingTime));
   }
}

void releaseParachute() {
  Servo1.write(0); 
  parachuteOut = 1;
}

void attachParachute() {
  Servo1.write(45);
  parachuteOut = 0;
}

void beginDriving() {
  digitalWrite(motorPin, HIGH);
  motorOn = 1;
}

/*
void drive() {
  didDrive == 1;
  if(isDriving == 1) {
    if(millis() - drivingTime > 20000) {
      isDriving = 0;
      drivingTime = 0;
      finishDriving();
    }
  } else {
    isDriving = 1;
    drivingTime = millis();
    beginDriving();
  }
}
*/

void finishDriving() {
  digitalWrite(motorPin, LOW);
  motorOn = 0;
}

void calculateSlopes() {
  if(currTime - prevTime > 1000) {
    AccXSlope = (MPU_6050_AccX - AccXPrev) / (currTime - prevTime) * 100;
    AccYSlope = (MPU_6050_AccY - AccYPrev) / (currTime - prevTime) * 100;
    AccZSlope = (MPU_6050_AccZ - AccZPrev) / (currTime - prevTime) * 100;
    GyroXSlope = (MPU_6050_GyroX - GyroXPrev) / (currTime - prevTime);
    GyroYSlope = (MPU_6050_GyroY - GyroYPrev) / (currTime - prevTime);
    GyroZSlope = (MPU_6050_GyroZ - GyroZPrev) / (currTime - prevTime);
    pressureSlope = (bmePressure - pressurePrev) / (currTime - prevTime);
    altitudeSlope = (bmeAltitude - altitudePrev) / (currTime - prevTime);
    AccXPrev = MPU_6050_AccX;
    AccYPrev = MPU_6050_AccY;
    AccZPrev = MPU_6050_AccZ;
    GyroXPrev = MPU_6050_GyroX;
    GyroYPrev = MPU_6050_GyroY;
    GyroZPrev = MPU_6050_GyroZ;
    pressurePrev = bmePressure;
    altitudePrev = bmeAltitude;
    prevTime = currTime;
  }
}

void collectData() {
  Wire.beginTransmission(MPU_6050_addr);
  Wire.write(ACCEL_XOUT_H); // ACC_XOUT_H register
  Wire.endTransmission();
  Wire.requestFrom(MPU_6050_addr, 14); // Requesting 14 bytes from the MPU-6050
  MPU_6050_AccX = Wire.read() << 8 | Wire.read();
  MPU_6050_AccY = Wire.read() << 8 | Wire.read();
  MPU_6050_AccZ = Wire.read() << 8 | Wire.read();
  MPU_6050_Temp = Wire.read() << 8 | Wire.read();
  MPU_6050_GyroX = Wire.read() << 8 | Wire.read();
  MPU_6050_GyroY = Wire.read() << 8 | Wire.read();
  MPU_6050_GyroZ = Wire.read() << 8 | Wire.read();
  MPU_6050_AccX = MPU_6050_AccX / 4096;
  MPU_6050_AccY = MPU_6050_AccY / 4096;
  MPU_6050_AccZ = MPU_6050_AccZ / 4096;
  MPU_6050_AccX = MPU_6050_AccX + accXCalibration;
  MPU_6050_AccY = MPU_6050_AccY + accYCalibration;
  MPU_6050_AccZ = MPU_6050_AccZ + accZCalibration;
  MPU_6050_Temp = MPU_6050_Temp / 340 + 36.53;
  MPU_6050_GyroX = MPU_6050_GyroX / 65.5;
  MPU_6050_GyroY = MPU_6050_GyroY / 65.5;
  MPU_6050_GyroZ = MPU_6050_GyroZ / 65.5;
  MPU_6050_GyroX = MPU_6050_GyroX + gyroXCalibration;
  MPU_6050_GyroY = MPU_6050_GyroY + gyroYCalibration;
  MPU_6050_GyroZ = MPU_6050_GyroZ + gyroZCalibration;
  int temp_adc_val;
  float temp_val;
  temp_adc_val = analogRead(lm35_pin);  /* Read Temperature */
  temp_val = (temp_adc_val * 4.88); /* Convert adc value to equivalent voltage */
  temp_val = (temp_val/10); /* LM35 gives output of 10mv/°C */
  extTemp = temp_val;
  bmeTemp = bme.readTemperature();
  bmePressure = bme.readPressure() / 100.0F;
  bmeAltitude = bme.readAltitude(1013.25);
  currTime = millis();

  /*
      Serial.print("MPU-6050 Acc X-Axis  = "); Serial.print(MPU_6050_AccX,2); Serial.println(" [g]");
      Serial.print("MPU-6050 Acc Y-Axis  = "); Serial.print(MPU_6050_AccY,2); Serial.println(" [g]");
      Serial.print("MPU-6050 Acc Z-Axis  = "); Serial.print(MPU_6050_AccZ,2); Serial.println(" [g]");
      Serial.print("MPU-6050 Gyro X-Axis = "); Serial.print(MPU_6050_GyroX,2); Serial.println(" [deg/s]");
      Serial.print("MPU-6050 Gyro Y-Axis = "); Serial.print(MPU_6050_GyroY,2); Serial.println(" [deg/s]");
      Serial.print("MPU-6050 Gyro Z-Axis = "); Serial.print(MPU_6050_GyroZ,2); Serial.println(" [deg/s]");
      Serial.print("MPU-6050 Temperature = "); Serial.print(MPU_6050_Temp,1); Serial.println(" [*C]");
      Serial.println("------");
      Serial.print("BMP280 Temperature   = "); Serial.print(bmeTemp); Serial.println(" [*C]");
      Serial.print("BMP280 Pressure      = "); Serial.print(bmePressure); Serial.println(" [hPa]");
      Serial.print("BMP280 Approx Alt    = "); Serial.print(bmeAltitude); Serial.println(" [m]");
      Serial.println("------");
  */
  
  // calculateSlopes();
}

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_6050_addr);
  Wire.write(PWR_MGMT_1);
  Wire.write(0); // Waking up the MPU-6050
  Wire.endTransmission();
  Wire.beginTransmission(MPU_6050_addr);
  Wire.write(ACCEL_CONFIG);
  Wire.write(0x10); // Setting the acceleration full scale range to plus minus 8g
  Wire.endTransmission();
  Wire.beginTransmission(MPU_6050_addr);
  Wire.write(GYRO_CONFIG);
  Wire.write(0x10); // Setting the gyro full scale range to plus minus 250 degrees per second
  Wire.endTransmission();

  Servo1.attach(servoPin);
  Servo1.write(35); 
  parachuteOut = 0;

  bme.begin();

  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);
  motorOn = 0;

  Serial.begin(9600);
  Serial.println( "Arduino started sending bytes via XBee" );
}

void loop() {
  liveTime = millis();
  if(armedTime != 0) {
    missionTime = liveTime - armedTime;
  }
  
  updateState();

  switch (flightState) {
  case 0: // Unarmed
    break;
  case 1: // Pre-Flight
    break;
  case 2: // Flight
    break;
  case 3: // Landed
    if(firstLanding == 0) {
      delay(2000);
      releaseParachute();
      didReleaseParachute = 1;
      delay(2000);
      drivingStartTime = liveTime;
      beginDriving();
      firstLanding = 1;
    } else {
      /*
      if(liveTime > 30000 + drivingStartTime) {
        finishDriving();
        didDrive = 1;
        delay(2000);
      }
      */
    }
    break;
  case 4: // Post-Landing
    break;
  case 5: // Manual Control
    break;
  default:
    break;
  }

  collectData();
  receiveUplink();
  sendTelemetry();
  delay(50);
}
