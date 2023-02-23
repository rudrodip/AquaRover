#include "debugger.h"

#define debugLED 2

void setupNotifier(){
  for (int i = 0; i < 5; i++){
    digitalWrite(debugLED, HIGH);
    delay(100);
    digitalWrite(debugLED, LOW);
    delay(100);
  }
}

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
  digitalWrite(debugLED, HIGH);
  delay(25);
  digitalWrite(debugLED, LOW);
  delay(25);
}

void sent(){
  digitalWrite(debugLED, HIGH);
  delay(25);
  digitalWrite(debugLED, LOW);
  delay(25);
}