#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
String pushbulletAPIKey = "your_PUSHBULLET_API_KEY";
String pushbulletURL = "https://api.pushbullet.com/v2/pushes";

WiFiClientSecure client;

void setup() {
  Serial.begin(9600);
  dht.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  client.setInsecure();
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  if (!isnan(temperature) && !isnan(humidity)) {
    String message = "{\"type\": \"note\", \"title\": \"DHT11 Notification\", \"body\": \"Temp: " + String(temperature) + "°C, Humidity: " + String(humidity) + "%\"}";
    sendPushbulletNotification(message);
  }
  delay(10000);
}

void sendPushbulletNotification(String message) {
  if (client.connect("api.pushbullet.com", 443)) {
    client.println("POST /v2/pushes HTTP/1.1");
    client.println("Host: api.pushbullet.com");
    client.println("Authorization: Bearer " + pushbulletAPIKey);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(message.length());
    client.println();
    client.println(message);
  }
}
