#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

float t = 0.0, h = 0.0;
AsyncWebServer server(80);
unsigned long previousMillis = 0;
const long interval = 10000;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head><meta name="viewport" content="width=device-width, initial-scale=1"></head><body><h2>DHT11 Server</h2><p>Temperature: <span id="temperature">%TEMPERATURE%</span> °C</p><p>Humidity: <span id="humidity">%HUMIDITY%</span> %</p><script>setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){document.getElementById("temperature").innerHTML=this.responseText;}};
xhttp.open("GET","/temperature",true);xhttp.send();},10000);
setInterval(function(){var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){document.getElementById("humidity").innerHTML=this.responseText;}};
xhttp.open("GET","/humidity",true);xhttp.send();},10000);</script></body></html>)rawliteral";

String processor(const String& var) {
  if (var == "TEMPERATURE") return String(t);
  if (var == "HUMIDITY") return String(h);
  return String();
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(1000);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){request->send_P(200, "text/html", index_html, processor);});
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){request->send_P(200, "text/plain", String(t).c_str());});
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){request->send_P(200, "text/plain", String(h).c_str());});
  server.begin();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    float newT = dht.readTemperature();
    float newH = dht.readHumidity();
    if (!isnan(newT)) t = newT;
    if (!isnan(newH)) h = newH;
  }
}
