#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include "debugger.h"
#include <ArduinoJson.h>
#include "handle_command.h"

// BLE SECTION
BLEServer *pServer = NULL;

BLECharacteristic *message_characteristic = NULL;
BLECharacteristic *box_characteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;
bool debugMode = false;

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define MESSAGE_CHARACTERISTIC_UUID "6d68efe5-04b6-4a85-abc4-c2670b7bf7fd"

// esp32 to arduino serial communication pin
#define rx 16
#define tx 17
DynamicJsonDocument doc(1024);

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    Serial.println("Connected");
    deviceConnected = true;
    if (debugMode)
      connectionSuccessful();
  };

  void onDisconnect(BLEServer *pServer)
  {
    Serial.println("Disconnected");
    deviceConnected = false;
    if (debugMode)
      disConnected();
  }
};

class CharacteristicsCallbacks : public BLECharacteristicCallbacks
{
  // on onWrite function, we're handling the command
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    String message = pCharacteristic->getValue().c_str();
    handle_command(message);
    if (debugMode)
      Serial.println(message);
    if (debugMode)
      received();
    if (message == "debug")
    {
      debugMode = true;
    }
    if (message == "!debug")
    {
      debugMode = false;
    }
  }
};

void checkToReconnect() // added
{
  // disconnected so advertise
  if (!deviceConnected && oldDeviceConnected)
  {
    delay(500);                  // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("Disconnected: start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connected so reset boolean control
  if (deviceConnected && !oldDeviceConnected)
  {
    // do stuff here on connecting
    Serial.println("Reconnected");
    oldDeviceConnected = deviceConnected;
  }
}

const byte numChars = 64;
char receivedChars[numChars]; // an array to store the received data
boolean newData = false;

void recvWithEndMarker()
{
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial2.available() > 0 && newData == false)
  {
    rc = Serial2.read();

    if (rc != endMarker)
    {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars)
      {
        ndx = numChars - 1;
      }
    }
    else
    {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void sendNewData()
{
  if (newData == true)
  {
    if (debugMode)
      Serial.println(receivedChars);
    if (deviceConnected)
      message_characteristic->setValue(receivedChars);
    newData = false;
  }
}

void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, rx, tx);

  // pinMode
  pinMode(2, OUTPUT);

  // Create the BLE Device
  BLEDevice::init("AquaRover");
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  delay(1000);

  // Create a BLE Characteristic
  message_characteristic = pService->createCharacteristic(
      MESSAGE_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);

  // Start the BLE service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();

  message_characteristic->setValue("Connection Established");
  message_characteristic->setCallbacks(new CharacteristicsCallbacks());

  setupServo();
  Serial.println("Initiated connection successfully :)");
  setupNotifier();
}

void loop()
{
  checkToReconnect();
  recvWithEndMarker();
  sendNewData();

  if (deviceConnected)
  {
    message_characteristic->notify(); // its to receive message
  }

  delay(100);
}