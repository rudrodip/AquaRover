#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include "debugger.h"
#include "ServoEasing.hpp"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
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

// dht11 pin
#define DHTPIN 15
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);

// tds
#define tdsPin 4
// turbidity
#define turbidityPin 34

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
    handle_command(message);
    if (debugMode) Serial.println(message);
    received();
    if (message == "debug"){
      debugMode = true;
    }
    if (message == "!debug"){
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

DynamicJsonDocument doc(1024);
double tds;
double turbidity;

void setup()
{
  Serial.begin(115200);

  // Initialize device.
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;

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

  Serial.println("Initiated connection successfully :)");
  setupNotifier();
}

void loop()
{
  checkToReconnect();
  doc.clear();

  sensors_event_t event;
  tds = analogRead(tdsPin);
  turbidity = analogRead(turbidityPin);

  // temperature
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println(F("Error reading temperature!"));
  }
  else
  {
    // Serial.print("Temp: ");
    // Serial.println(event.temperature);
    doc["temperature"] = event.temperature;
  }

  // humidity
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    Serial.println(F("Error reading humidity!"));
  }
  else
  {
    // Serial.print("Humid: ");
    // Serial.println(event.relative_humidity);
    doc["humidity"] = event.relative_humidity;
  }

  // Serial.print("TDS: ");
  // Serial.println(tds);
  doc["tds"] = tds;
  doc["turbidity"] = turbidity;

  // Convert the JSON to a string
  String jsonString;
  serializeJson(doc, jsonString);
  if (debugMode) Serial.println(jsonString);

  if (deviceConnected)
  {
    message_characteristic->notify(); // its to receive message
    message_characteristic->setValue(jsonString.c_str());
  }

  delay(100);
}