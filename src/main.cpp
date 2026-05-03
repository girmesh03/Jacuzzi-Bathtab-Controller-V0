// ============================================================================
// MAIN.CPP - Jacuzzi/Bathtub Controller System
// ============================================================================
// ESP8266-based embedded control system for jacuzzi/bathtub
// Target Platform: ESP8266 (ESP-12E/ESP-12F)
// ============================================================================

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SH110X.h>
#include "constants.h"

// ============================================================================
// DEBUG MACROS
// ============================================================================
#ifdef ENABLE_SERIAL_DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================
Adafruit_SH1106G display(128, 64, &Wire, -1);

// ============================================================================
// SETUP FUNCTION
// ============================================================================
void setup() {
  // Initialize Serial for debug output
  Serial.begin(115200);
  delay(100);  // Allow serial to stabilize
  
  DEBUG_PRINTLN("Jacuzzi Controller Starting...");
  
  // Initialize I2C with explicit ESP8266 pins
  Wire.begin(PIN_SDA, PIN_SCL);
  DEBUG_PRINTLN("I2C initialized");
  
  // Initialize OLED display
  DEBUG_PRINT("Initializing OLED display at address 0x");
  DEBUG_PRINTLN(I2C_OLED_ADDR, HEX);
  
  if (!display.begin(I2C_OLED_ADDR, true)) {
    DEBUG_PRINTLN("ERROR: OLED display initialization failed!");
    DEBUG_PRINTLN("System halted. Check I2C connections.");
    // Enter infinite loop on failure
    while (1) {
      yield();  // Allow ESP8266 background tasks
    }
  }
  
  DEBUG_PRINTLN("OLED display initialized successfully");
  
  // Configure display settings
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  DEBUG_PRINTLN("Display configured");
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  // Empty for now - will be populated in later phases
  yield();  // Allow ESP8266 background tasks
}
