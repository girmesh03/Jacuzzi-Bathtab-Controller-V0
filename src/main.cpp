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
  Serial.begin(115200);
  delay(100); // Allow serial to stabilize

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

  // Display splash screen for 2 seconds
  displayManager.showSplashScreen();

  // Display READY message
  displayManager.showReadyMessage();

  DEBUG_PRINTLN("Phase 1 initialization complete");
  DEBUG_PRINTLN("Phase 2: Sensor integration active");
}

// ============================================================================
// DISPLAY SENSOR DATA FUNCTION
// ============================================================================
/**
 * @brief Display temperature and water level on OLED
 * 
 * Displays current temperature (or "TEMP ERROR" if invalid)
 * Displays water level status ("Water: OK" or "LOW WATER" flashing)
 * 
 * Requirements: 4.3, 5.5, 23.1-23.4, 24.1-24.4
 */
void displaySensorData()
{
  // Get display object reference
  Adafruit_SH1106G &display = displayManager.getDisplay();
  
  // Clear display
  display.clearDisplay();
  
  // ========================================================================
  // DISPLAY TEMPERATURE
  // ========================================================================
  display.setTextSize(2);
  display.setCursor(0, 0);
  
  if (sensors.isTemperatureValid())
  {
    // Display temperature with 0.1°C precision (XX.X°C format)
    float temp = sensors.getTemperature();
    display.print(temp, 1);  // 1 decimal place
    display.println(F(" C"));  // Degree symbol not available in default font
  }
  else
  {
    // Display error message
    display.println(F("TEMP ERROR"));
  }
  
  // ========================================================================
  // DISPLAY WATER LEVEL
  // ========================================================================
  display.setTextSize(1);
  display.setCursor(0, 30);
  
  if (sensors.isWaterLevelOK())
  {
    // Water level OK - display normal message
    display.println(F("Water: OK"));
  }
  else
  {
    // Water level LOW - display flashing warning
    // Flash at 1 Hz (on for 500ms, off for 500ms)
    static uint32_t lastFlashTime = 0;
    static bool flashState = false;
    
    if (millis() - lastFlashTime >= 500)
    {
      flashState = !flashState;
      lastFlashTime = millis();
    }
    
    if (flashState)
    {
      display.println(F("LOW WATER"));
    }
  }
  
  // ========================================================================
  // UPDATE DISPLAY
  // ========================================================================
  display.display();
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop()
{
  // Update sensors (non-blocking)
  sensors.update();
  
  // Display sensor data with rate limiting (every 500ms)
  static uint32_t lastDisplayUpdate = 0;
  
  if (millis() - lastDisplayUpdate >= 500)
  {
    displaySensorData();
    lastDisplayUpdate = millis();
  }
  
  // Allow ESP8266 background tasks
  yield();
}
