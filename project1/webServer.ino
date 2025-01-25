#include "config.h"

// Get Sensor Readings and return JSON object
String getSensorReadings() {
  readings["vbatt"] = String(mySensorData.vBatt);
  readings["solarbatt"] = String(mySensorData.vSolar);
  readings["solarcurrent"] = String(mySensorData.aSolar);
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



void wifiSetup() { 
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

  // Configure the LED 
  pinMode(output26, OUTPUT);
  digitalWrite(output26, LOW);


}