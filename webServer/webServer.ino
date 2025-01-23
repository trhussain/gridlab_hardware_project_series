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
unsigned long timerDelay = 15000;

// Create a sensor object

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

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
String getGPIOStates() {
  readings["output26"] = output26State;
  readings["output27"] = output27State;
  return JSON.stringify(readings);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    // Create a properly null-terminated string from received data
    char message[len + 1]; // Add space for the null terminator
    memcpy(message, data, len);
    message[len] = '\0'; // Null-terminate the string

    Serial.println(message); // Print the clean message for debugging

    // Check if "toggle26" is part of the message
    if (strstr(message, "toggle26") != NULL) {
      output26State = (output26State == "off") ? "on" : "off";
      digitalWrite(output26, (output26State == "on") ? HIGH : LOW);
      Serial.println("Toggled GPIO 26");
      notifyClients(getGPIOStates());
    } 
    // Check if "toggle27" is part of the message
    else if (strstr(message, "toggle27") != NULL) {
      output27State = (output27State == "off") ? "on" : "off";
      digitalWrite(output27, (output27State == "on") ? HIGH : LOW);
      Serial.println("Toggled GPIO 27");
      notifyClients(getGPIOStates());
    } 
    // Handle other messages, like "getReadings"
    else if (strstr(message, "getReadings") != NULL) {
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



    pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);


}

void loop() {


        if ((millis() - lastTime) > timerDelay) {
          notifyClients(getSensorReadings());
          lastTime = millis();
        }
        ws.cleanupClients();

}
