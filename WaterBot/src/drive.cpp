#include "drive.h"

// motor driver pins
#define AIN1 25
#define BIN1 27

#define AIN2 33
#define BIN2 14
#define PWMA 32
#define PWMB 13
#define STBY 26

#define pi 3.1415926

const double maxDistance = 1.0;
const double maxAngle = 360.0;
const int maxPWM = 255;

Motor leftMotor = Motor(AIN1, AIN2, PWMA, 1, STBY);
Motor rightMotor = Motor(BIN1, BIN2, PWMB, 1, STBY);

void drive(double dis, double angle){
   // Convert the angle to radians
  double radians = angle * (pi / 180);

  // Calculate the PWM values for each motor based on the distance and angle
  double delta;
  int leftMotorPWM, rightMotorPWM;

  delta = dis * maxPWM * cos(radians);

  if (angle <= 90 && angle >= -90){
    rightMotorPWM = maxPWM - delta;
  } else {
    leftMotorPWM = maxPWM + delta;
  }

  leftMotor.drive(leftMotorPWM);
  rightMotor.drive(rightMotorPWM);
}