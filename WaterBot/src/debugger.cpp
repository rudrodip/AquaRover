#include "debugger.h"

#define debugLED 2

void connectionSuccessful(){
  for (int i = 0; i < 3; i++){
    digitalWrite(debugLED, HIGH);
    delay(500);
    digitalWrite(debugLED, LOW);
    delay(500);
  }
}

void disConnected(){
  for (int i = 0; i < 2; i++){
    digitalWrite(debugLED, HIGH);
    delay(500);
    digitalWrite(debugLED, LOW);
    delay(500);
  }
}

void received(){
  for (int i = 0; i < 3; i++){
    digitalWrite(debugLED, HIGH);
    delay(50);
    digitalWrite(debugLED, LOW);
    delay(50);
  }
}

void sent(){
  for (int i = 0; i < 3; i++){
    digitalWrite(debugLED, HIGH);
    delay(50);
    digitalWrite(debugLED, LOW);
    delay(50);
  }
}