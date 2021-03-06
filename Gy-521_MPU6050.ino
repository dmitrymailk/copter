#include <Wire.h>
#include "Kalman.h"

Kalman kalmanX;
Kalman kalmanY;
 
uint8_t IMUAddress = 0x68;
 
/* IMU Data */
int16_t accX;
int16_t accY;
int16_t accZ;
int16_t tempRaw;
int16_t gyroX;
int16_t gyroY;
int16_t gyroZ;

// controller

int target_roll;
double roll;
double error;
double error_prev;
double error_sum;
int P, D, I;
int left, right;
int trottle = 50;
int force;

int M_1 = 3;
//controller
 
double accXangle; // Angle calculate using the accelerometer
double accYangle;
double temp;
double gyroXangle = 180; // Angle calculate using the gyro
double gyroYangle = 180;
double compAngleX = 180; // Calculate the angle using a Kalman filter
double compAngleY = 180;
double kalAngleX; // Calculate the angle using a Kalman filter
double kalAngleY;
 
uint32_t timer;
 
void setup() {
	//callibrate

	trottle = 100;

	//P = 0;
  P = 1;
	I = 0;
	//D = 0.004;
  D = 1;


  pinMode(M_1, OUTPUT);
  
	//callibrate

  Wire.begin();
  Serial.begin(9600);
    
 
  i2cWrite(0x6B,0x00); // Disable sleep mode      
  kalmanX.setAngle(180); // Set starting angle
  kalmanY.setAngle(180);
  timer = micros();
}
 
void loop() {

  //callibrate

  trottle = 100;

  /* Update all the values */
  uint8_t* data = i2cRead(0x3B,14);
  accX = ((data[0] << 8) | data[1]);
  accY = ((data[2] << 8) | data[3]);
  accZ = ((data[4] << 8) | data[5]);
  tempRaw = ((data[6] << 8) | data[7]);
  gyroX = ((data[8] << 8) | data[9]);
  gyroY = ((data[10] << 8) | data[11]);
  gyroZ = ((data[12] << 8) | data[13]);
 
  /* Calculate the angls based on the different sensors and algorithm */
  accYangle = (atan2(accX,accZ)+PI)*RAD_TO_DEG;
  accXangle = (atan2(accY,accZ)+PI)*RAD_TO_DEG;  
 
  double gyroXrate = (double)gyroX/131.0;
  double gyroYrate = -((double)gyroY/131.0);
  gyroXangle += kalmanX.getRate()*((double)(micros()-timer)/1000000); // Calculate gyro angle using the unbiased rate
  gyroYangle += kalmanY.getRate()*((double)(micros()-timer)/1000000);
 
  kalAngleX = kalmanX.getAngle(accXangle, gyroXrate, (double)(micros()-timer)/1000000); // Calculate the angle using a Kalman filter
  kalAngleY = kalmanY.getAngle(accYangle, gyroYrate, (double)(micros()-timer)/1000000);
  timer = micros();

  kalAngleX = abs(kalAngleX - 180);
  kalAngleY = abs(kalAngleY - 180);
 


  // The accelerometer's maximum samples rate is 1kHz

	//callibrate

	roll = kalAngleX;
  trottle = 10;
  

	error_prev = error;
	error = roll - target_roll;
	error_sum += error;

	force = P * error + I * error_sum + D * (error - error_prev);

	force = abs(round(force));

	left = trottle + force;

  analogWrite(M_1, left);

	//callibrate

	Serial.println();
    Serial.print("X:");
    Serial.print(kalAngleX,0);
    Serial.print(" ");
  
    Serial.print("Y:");
    Serial.print(kalAngleY,0);
    Serial.println(" ");

		Serial.print("left:");
    Serial.print(left);
    Serial.println(" ");

		Serial.print("trottle:");
    Serial.print(trottle);
    Serial.println(" ");

    Serial.print("force:");
    Serial.print(force);
    Serial.println(" ");
}

void i2cWrite(uint8_t registerAddress, uint8_t data){
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  Wire.write(data);
  Wire.endTransmission(); // Send stop
}
uint8_t* i2cRead(uint8_t registerAddress, uint8_t nbytes) {
  uint8_t data[nbytes];
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  Wire.endTransmission(false); // Don't release the bus
  Wire.requestFrom(IMUAddress, nbytes); // Send a repeated start and then release the bus after reading
  for(uint8_t i = 0; i < nbytes; i++)
    data [i]= Wire.read();
  return data;
}
