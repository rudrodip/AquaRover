#include "drive.h"
#include "ServoEasing.hpp"

// motor driver pins
#define AIN1 25
#define BIN1 27

#define AIN2 33
#define BIN2 14

#define PWMA 32
#define PWMB 13
#define STBY 26

// servo pins
#define directionServo 23
int directionServoAngle = 90;
ServoEasing DirectionServoMotor;

#define pi 3.1415926

const int maxPWM = 255;

Motor thruster = Motor(AIN1, AIN2, PWMA, 1, STBY);

void manageServo(int angle){
  DirectionServoMotor.startEaseTo(angle, 100);
}

void setupServo(){
  DirectionServoMotor.attach(directionServo, directionServoAngle);
}

void drive(double dis, double angle){
  double delta;
  int thrusterPWM = int(dis * maxPWM);

  if (angle < 0) thrusterPWM = thrusterPWM * (-1);

  int absoluteAngle = int(angle) % 360;  // take modulo 360 to get absolute angle
  if (absoluteAngle < 0) {
    absoluteAngle += 360;  // if negative, add 360 to get positive absolute angle
  }
  
  thruster.drive(thrusterPWM);
}