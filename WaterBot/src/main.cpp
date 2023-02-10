#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "time.h"

#define WIFI_SSID "Sumit"
#define WIFI_PASSWORD "sumit625"

#define USER_EMAIL "rudrodip625@gmail.com"
#define USER_PASSWORD "sumit625"

#define API_KEY "AIzaSyBi68cA2xxkLUH3BHAWdo9RqUYV4KMoeeY"
#define DATABASE_URL "https://waterbot-33d6f-default-rtdb.asia-southeast1.firebasedatabase.app/"

#define TIMEDELAY 5000

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String tempPath = "/temp";
String humPath = "/humid";
String ph = "/ph";

// Parent Node (to be updated in every loop)
String parentPath;
String uid;

const char* ntpServer = "pool.ntp.org";
int timestamp;
FirebaseJson json;

unsigned long sendDataPrevMillis = 0;

// Initialize WiFi
void initWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  // endless loop untils connects
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  // after connecting successfully
  Serial.println("Connected Successfully :)");
  Serial.println();
}

double getPh()
{
  return 5.6;
}

double getTemp()
{
  return 25.6;
}

double getHumid()
{
  return 85.6;
}

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void setup()
{
  Serial.begin(115200);
  // Initialize WiFi
  initWiFi();
  configTime(0, 0, ntpServer);

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);
  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "")
  {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";
}

void loop()
{
  if (Firebase.ready() && (millis() - sendDataPrevMillis > TIMEDELAY || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    //Get current timestamp
    timestamp = getTime();
    Serial.print ("time: ");
    Serial.println (timestamp);
    
    parentPath= databasePath + "/" + String(timestamp);

    json.set(tempPath.c_str(), String(getTemp()));
    json.set(humPath.c_str(), String(getHumid()));
    json.set(ph.c_str(), String(getPh()));

    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }
}