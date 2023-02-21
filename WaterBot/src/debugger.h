#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <Arduino.h>

void setupNotifier();
void connectionSuccessful();
void disConnected();
void received();
void sent();

#endif
