#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "";
const char* password = "";

#define LED_PIN 2      // GPIO2 - Onboard LED
#define IN1_PIN 5      // GPIO5 (D1)
#define IN2_PIN 4      // GPIO4 (D2)
#define IN3_PIN 0      // GPIO0 (D3)
#define IN4_PIN 14     // GPIO14 (D5) - moved to avoid LED conflict

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html { font-family: Arial; display: inline-block; text-align: center; }
    h2 { font-size: 2.5rem; }
    .button {
      background-color: #4CAF50; border: none; color: white;
      padding: 16px 20px; margin: 4px 2px; font-size: 16px;
      cursor: pointer;
    }
    .button-off { background-color: #f44336; }
  </style>
</head>
<body>
  <h2>NodeMCU Web Control</h2>
  <p>
    <button id="ledButton" class="button" onclick="toggleLED()">Toggle LED</button><br><br>
    <button id="in1Button" class="button" onclick="toggleIN('IN1')">Toggle IN1</button>
    <button id="in2Button" class="button" onclick="toggleIN('IN2')">Toggle IN2</button>
    <button id="in3Button" class="button" onclick="toggleIN('IN3')">Toggle IN3</button>
    <button id="in4Button" class="button" onclick="toggleIN('IN4')">Toggle IN4</button>
  </p>
  <script>
    function toggleLED() {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/toggleLED", true);
      xhttp.send();
    }
    function toggleIN(inName) {
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/" + inName + "toggle", true);
      xhttp.send();
    }
    function updateButton(buttonId, state) {
      var btn = document.getElementById(buttonId);
      btn.innerHTML = "Toggle " + buttonId.replace("Button", "");
      if (state) btn.classList.remove("button-off");
      else btn.classList.add("button-off");
    }
    setInterval(function() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          var states = this.responseText.split(",");
          updateButton("ledButton", states[0] == "0");
          updateButton("in1Button", states[1] == "0");
          updateButton("in2Button", states[2] == "0");
          updateButton("in3Button", states[3] == "0");
          updateButton("in4Button", states[4] == "0");
        }
      };
      xhttp.open("GET", "/getState", true);
      xhttp.send();
    }, 1000);
  </script>
</body>
</html>)rawliteral";

bool ledState = false;
bool in1State = false;
bool in2State = false;
bool in3State = false;
bool in4State = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/toggleLED", HTTP_GET, [](AsyncWebServerRequest *request){
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? LOW : HIGH);  // LOW = ON
    request->send(200, "text/plain", "LED Toggled");
  });

  server.on("/IN1toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    in1State = !in1State;
    digitalWrite(IN1_PIN, in1State ? LOW : HIGH);
    request->send(200, "text/plain", "IN1 Toggled");
  });

  server.on("/IN2toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    in2State = !in2State;
    digitalWrite(IN2_PIN, in2State ? LOW : HIGH);
    request->send(200, "text/plain", "IN2 Toggled");
  });

  server.on("/IN3toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    in3State = !in3State;
    digitalWrite(IN3_PIN, in3State ? LOW : HIGH);
    request->send(200, "text/plain", "IN3 Toggled");
  });

  server.on("/IN4toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    in4State = !in4State;
    digitalWrite(IN4_PIN, in4State ? LOW : HIGH);
    request->send(200, "text/plain", "IN4 Toggled");
  });

  server.on("/getState", HTTP_GET, [](AsyncWebServerRequest *request){
    String state = String(digitalRead(LED_PIN)) + "," +
                   String(digitalRead(IN1_PIN)) + "," +
                   String(digitalRead(IN2_PIN)) + "," +
                   String(digitalRead(IN3_PIN)) + "," +
                   String(digitalRead(IN4_PIN));
    request->send(200, "text/plain", state);
  });

  server.begin();
}

void loop() {}
