#include "config.h"
 // Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

void setup(){
  // Setup
  Serial.begin(115200);
  wifiSetup();
  finalSetup();
}
 
void loop(){
  if ((millis() - lastTime) > timerDelay) {
    notifyClients(getSensorReadings());
    finalDisplay();

    lastTime = millis();
  }
  ws.cleanupClients();
}
