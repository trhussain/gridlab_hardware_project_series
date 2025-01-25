#ifndef CONFIG_H
#define CONFIG_H

// Include Library 
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ACS712.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>


// Current & Voltage sensor pins 
#define solarVSensor 34
#define solarCSensor 3
#define battVSensor 35

#define battDivider 7.17
#define solarDivider 16.95
// Custom SDA/SCL Pins (ensure they match yours)
#define SDA_PIN 4 // DO NOT USE 2             
#define SCL_PIN 15           








// OLED Constants
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };



// ----------- Web Server Variables


// Soft Access Point credentials
const char* ssid = "REPLACE WITH YOUR NAME";
const char* password = "REPLACE WITH YOUR BIRTHDAY";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Json Variable to Hold Sensor Readings
JSONVar readings;



// Auxiliar variables to store the current output state
String output26State = "off";
const int output26 = 26;

struct sensorData { 
  float vBatt; 
  float vSolar;
  float aSolar;
} mySensorData;


#endif // CONFIG_H
