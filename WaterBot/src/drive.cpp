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
Servo DirectionServoMotor;

#define pi 3.1415926

const int maxPWM = 255;

Motor thruster = Motor(AIN1, AIN2, PWMA, 1, STBY);

void manageServo(int angle){
  DirectionServoMotor.write(angle);
}

void setupServo(){
  DirectionServoMotor.attach(directionServo);
}

void drive(double dis, double angle){
  double delta;
  int thrusterPWM = int(dis * maxPWM);
  
  DirectionServoMotor.write(int(angle));
  thruster.drive(thrusterPWM);
}