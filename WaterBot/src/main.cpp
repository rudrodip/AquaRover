#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include "debugger.h"
#include <SparkFun_TB6612.h>
#include "ServoEasing.hpp"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoJson.h>

// BLE SECTION
BLEServer *pServer = NULL;

BLECharacteristic *message_characteristic = NULL;
BLECharacteristic *box_characteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define MESSAGE_CHARACTERISTIC_UUID "6d68efe5-04b6-4a85-abc4-c2670b7bf7fd"

// dht11 pin
#define DHTPIN 15
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);

// motor driver pins
#define AIN1 25
#define BIN1 27

#define AIN2 33
#define BIN2 14
#define PWMA 32
#define PWMB 13
#define STBY 26

// motor speed
#define turningSpeed 200

// tds
#define tdsPin 23

Motor motor1 = Motor(AIN1, AIN2, PWMA, 1, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, 1, STBY);

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
// tds value
int tds = 0;
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

  // temperature
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println(F("Error reading temperature!"));
  }
  else
  {
    Serial.println(event.temperature);
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
    Serial.println(event.relative_humidity);
    doc["humidity"] = event.relative_humidity;
  }

  Serial.println(tds);
  doc["tds"] = tds;

  // Convert the JSON to a string
  String jsonString;
  serializeJson(doc, jsonString);

  if (deviceConnected)
  {
    message_characteristic->notify(); // its to receive message
    message_characteristic->setValue(jsonString.c_str());
  }

  delay(100);
}