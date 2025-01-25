#include "config.h"
int mVperAmp = 185;           // this the 5A version of the ACS712 -use 100 for 20A Module and 66 for 30A Module
int Watt = 0;
double VoltageRMS = 0;
double VRMS = 0;
double AmpsRMS = 0;

void sensorSetup() {

  pinMode(solarVSensor, INPUT);
  pinMode(solarCSensor, INPUT);

}



void finalSetup() { 

  pinMode(solarVSensor, INPUT);
  pinMode(solarCSensor, INPUT);
  // Challenge #3 Code 
  Wire.begin(SDA_PIN, SCL_PIN);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  Serial.println("Sensors & OLED setup...");
}

void finalDisplay() { 
  float battVRaw = analogRead(battVSensor);
  float solarVRaw = analogRead(solarVSensor);
  float solarCRaw = analogRead(solarCSensor);

  const int battVR1 = 10; const int battVR2 = 20; 
  const int solarVR1 = 10; const int solarVR2 = 20;
  const int solarCR1 = 10; const int solarCR2 = 20;

  // Convert the analog reading (which goes from 0 - 4096) to a voltage (0 - 3V3):
  mySensorData.vBatt = (battVRaw / 4096 ) * battDivider;
  mySensorData.vSolar = (solarVRaw /4096 ) * solarDivider;
  mySensorData.aSolar= solarCRaw * (3.3/ 4096);


  Serial.println("----------------------------------");
  Serial.print("Raw Batt & Raw Solar V: ");
  Serial.print(battVRaw);
  Serial.print(" & ");
  Serial.println(solarVRaw);
  Serial.print("Batt Voltage: ");
  Serial.println(mySensorData.vBatt);

  Serial.print("Solar Voltage: ");
  Serial.println(mySensorData.vSolar);

  Serial.print("Solar Current: ");
  Serial.println(mySensorData.aSolar);

  displayReadings(mySensorData.vBatt , mySensorData.vSolar , mySensorData.aSolar );
}


void displayReadings(float battV, float solarV, float solarC) {
  // Clear display buffer
  display.clearDisplay();
  
  // Set text size and color
  display.setTextSize(1); // Smallest font
  display.setTextColor(SSD1306_WHITE);

  // Print battery voltage
  display.setCursor(0, 0);
  display.print("Batt Voltage: ");
  display.print(battV, 2); // 2 decimal places

  // Print solar voltage
  display.setCursor(0, 16);
  display.print("Solar Voltage: ");
  display.print(solarV, 2); // 2 decimal places

  // Print solar current
  display.setCursor(0, 32);
  display.print("Solar Current: ");
  display.print(solarC, 2); // 2 decimal places

  // Display buffer
  display.display();
}