
 #define BLYNK_PRINT Serial

// Necesaary Libraries
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "HardwareSerial.h"
#include <ArduinoJson.h>

char auth[] = "Your BLYNK API_KEY";

#define LED_BUILTIN 5
HardwareSerial MySerial(2);

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Your home Wifi Router LogID";
char pass[] = "Your home Wifi Router Password";

int pin_status;

/*
BLYNK_WRITE(V1)
{
  pin_status = param.asInt();
}
*/
BlynkTimer timer;

void setup() {
  //Serial Begin at 9600 Baud 
  //MySerial.begin(9600,); // For sending data to another ESP32
  WiFi.begin(ssid,pass);
  Serial.begin(9600);
  MySerial.begin(9600, SERIAL_8N1, 0, 2); //RX, TX
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  //Blynk.begin(auth); // Establishing Communication with Blynk Server
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000, handleIndex);
}

void loop() {
  Blynk.run(); // Handeling Blynk Services
  timer.run();
};

void handleIndex()
{
  // Send a JSON-formatted request with key "type" and value "request"
  // then parse the JSON-formatted response with keys "gas" and "distance"
  DynamicJsonDocument doc(1024);
  double gas = 0, distance = 0;
  // Sending the request
  doc["type"] = "request";
  serializeJson(doc,MySerial);
  // Reading the response
  boolean messageReady = false;
  String message = "";
  while(messageReady == false) { // blocking but that's ok
    if(MySerial.available()) {
      message = MySerial.readString();
      Serial.println(message);
      messageReady = true;
    }
  }
  // Attempt to deserialize the JSON-formatted message
  DeserializationError error = deserializeJson(doc,message);
  if(error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  distance = doc["distance"];
  gas = doc["gas"];
  Blynk.virtualWrite(V2,distance);
  Blynk.virtualWrite(V3,gas);
}
