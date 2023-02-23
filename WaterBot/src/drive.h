#ifndef DRIVER_H
#define DRIVER_H

#include <Arduino.h>
#include <SparkFun_TB6612.h>

void manageServo(int);
void setupServo();
void drive(double, double);

#endif