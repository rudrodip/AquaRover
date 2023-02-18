#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include "debugger.h"

// BLE SECTION
BLEServer *pServer = NULL;

BLECharacteristic *message_characteristic = NULL;
BLECharacteristic *box_characteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define MESSAGE_CHARACTERISTIC_UUID "6d68efe5-04b6-4a85-abc4-c2670b7bf7fd"


class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    Serial.println("Connected");
    deviceConnected = true;
    connectionSuccessful();
  };

  void onDisconnect(BLEServer *pServer)
  {
    Serial.println("Disconnected");
    deviceConnected = false;
    disConnected();
  }
};

class CharacteristicsCallbacks : public BLECharacteristicCallbacks
{
  // on onWrite function, we're handling the command
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    String message = pCharacteristic->getValue().c_str();
    Serial.println(message);
    received();
  }
};

void checkToReconnect() //added
{
  // disconnected so advertise
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("Disconnected: start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connected so reset boolean control
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    Serial.println("Reconnected");
    oldDeviceConnected = deviceConnected;
  }
}

void setup()
{
  Serial.begin(115200);

  // pinMode
  pinMode(2, OUTPUT);

  // Create the BLE Device
  BLEDevice::init("Durbar");
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  delay(100);

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

  Serial.println("Initiated connection successfully :)");
}


void loop()
{
  checkToReconnect();
  message_characteristic->notify(); // its to receive message
  message_characteristic->setValue("hello");

  delay(100);
}