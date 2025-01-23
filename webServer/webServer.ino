#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>

// Soft Access Point credentials
const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Json Variable to Hold Sensor Readings
JSONVar readings;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// Create a sensor object

// Initialize the BME280 sensor


// Get Sensor Readings and return JSON object
String getSensorReadings() {
  readings["temperature"] = String("potato");
  readings["humidity"] = String("234");
  readings["pressure"] = String("456");
  return JSON.stringify(readings);
}

// Initialize LittleFS
void initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}

// Notify all WebSocket clients with sensor readings
void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}

// Handle incoming WebSocket messages
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    String message = (char*)data;
    if (message == "getReadings") {
      notifyClients(getSensorReadings());
    }
  }
}

// WebSocket event handler
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

// Initialize WebSocket
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// Initialize WiFi as a Soft Access Point
void initWiFi() {
  WiFi.softAP(ssid); // No password
  IPAddress IP = WiFi.softAPIP(); // Get the IP address of the AP
  Serial.print("Access Point IP address: ");
  Serial.println(IP);
}

void setup() {
  // Start Serial communication
  Serial.begin(115200);

  // Initialize components
  initLittleFS();
  initWebSocket();
  initWiFi();

  // Serve HTML file from LittleFS
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  // Serve static files (CSS, JS, etc.) from LittleFS
  server.serveStatic("/", LittleFS, "/");

  // Start the server
  server.begin();
}

void loop() {


        if ((millis() - lastTime) > timerDelay) {
          notifyClients(getSensorReadings());
          lastTime = millis();
        }
        ws.cleanupClients();

}
