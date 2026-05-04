// ============================================================================
// MAIN.CPP - Jacuzzi/Bathtub Controller System
// ============================================================================
// ESP8266-based embedded control system for jacuzzi/bathtub
// Target Platform: ESP8266 (ESP-12E/ESP-12F)
//
// Phase 1: Basic Hardware Initialization
// - I2C communication with OLED display and PCF8574
// - Splash screen and READY message display
// - All relays initialized to OFF state
//
// Phase 2: Sensor Integration
// - DS18B20 temperature sensor reading
// - Water level sensor monitoring
// - Sensor data display on OLED
// ============================================================================

#include <Arduino.h>
#include <Wire.h>
#include "Constants.h"
#include "Display.h"
#include "Actuators.h"
#include "Sensors.h"

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
DisplayManager displayManager;
ActuatorManager actuatorManager;
SensorManager sensors;

// ============================================================================
// SETUP FUNCTION
// ============================================================================
void setup()
{
  // Initialize Serial for debug output
  Serial.begin(SERIAL_BAUD_RATE);
  delay(SERIAL_INIT_DELAY); // Allow serial to stabilize

  DEBUG_PRINTLN("Jacuzzi Controller Starting...");
  DEBUG_PRINTLN("Phase 1: Basic Hardware Initialization");

  // Initialize I2C with explicit ESP8266 pins
  Wire.begin(PIN_SDA, PIN_SCL);
  DEBUG_PRINTLN("I2C initialized on SDA=GPIO4 (D2), SCL=GPIO5 (D1)");

  // Initialize OLED display
  if (!displayManager.init())
  {
    DEBUG_PRINTLN("ERROR: OLED display initialization failed!");
    DEBUG_PRINTLN("System halted. Check I2C connections.");
    // Enter infinite loop on failure
    while (1)
    {
      yield(); // Allow ESP8266 background tasks
    }
  }

  // Initialize PCF8574 I/O expander (all relays OFF)
  if (!actuatorManager.init())
  {
    DEBUG_PRINTLN("ERROR: PCF8574 initialization failed!");
    DEBUG_PRINTLN("System halted. Check I2C connections and address.");

    // Display error on OLED
    displayManager.showError("PCF8574 ERROR\nCheck I2C");

    // Enter infinite loop on failure
    while (1)
    {
      yield(); // Allow ESP8266 background tasks
    }
  }

  // Initialize sensors (DS18B20 and water level)
  sensors.init();
  DEBUG_PRINTLN("Sensors initialized");

  // Display splash screen for SPLASH_SCREEN_DURATION
  displayManager.showSplashScreen();

  // Display READY message
  displayManager.showReadyMessage();

  DEBUG_PRINTLN("Phase 1 initialization complete");
  DEBUG_PRINTLN("Phase 2: Sensor integration active");
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop()
{
  // Update sensors (non-blocking)
  sensors.update();
  
  // Display sensor data with rate limiting (every DISPLAY_UPDATE_INTERVAL ms)
  static uint32_t lastDisplayUpdate = 0;
  
  if (millis() - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL)
  {
    // Get sensor data
    float temperature = sensors.getTemperature();
    bool tempValid = sensors.isTemperatureValid();
    bool waterLevelOK = sensors.isWaterLevelOK();
    
    // Display sensor data through Display module
    displayManager.showSensorData(temperature, tempValid, waterLevelOK);
    
    lastDisplayUpdate = millis();
  }
  
  // Allow ESP8266 background tasks
  yield();
}
