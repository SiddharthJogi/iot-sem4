#include <ESP8266WiFi.h>
#include <DHT.h>
#include "ThingSpeak.h"

#define DHTPIN D3
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
WiFiClient client;
unsigned long myChannelNumber = YOUR_CHANNEL_ID;
const char* myWriteAPIKey = "YOUR_API_KEY";

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  ThingSpeak.begin(client);
  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!isnan(h) && !isnan(t)) {
    ThingSpeak.setField(1, t);
    ThingSpeak.setField(2, h);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  }
  delay(20000);
}
