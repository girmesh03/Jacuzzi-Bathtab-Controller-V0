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
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================
bool initPCF8574();

// ============================================================================
// PCF8574 INITIALIZATION
// ============================================================================
/**
 * Initialize PCF8574 I2C I/O expander with all outputs OFF
 * Returns: true if successful, false on I2C communication failure
 */
bool initPCF8574()
{
  DEBUG_PRINT("Initializing PCF8574 at address 0x");
  DEBUG_PRINTLN(String(I2C_PCF8574_ADDR, HEX));

  // Begin I2C transmission to PCF8574
  Wire.beginTransmission(I2C_PCF8574_ADDR);

  // Write 0x00 to set all outputs LOW (relays OFF)
  Wire.write(0x00);

  // End transmission and check result
  uint8_t result = Wire.endTransmission();

  if (result == 0)
  {
    DEBUG_PRINTLN("PCF8574 initialized successfully - all relays OFF");
    return true;
  }
  else
  {
    DEBUG_PRINT("ERROR: PCF8574 initialization failed! I2C error code: ");
    DEBUG_PRINTLN(String(result));
    return false;
  }
}

// ============================================================================
// SETUP FUNCTION
// ============================================================================
void setup()
{
  // Initialize Serial for debug output
  Serial.begin(115200);
  delay(100); // Allow serial to stabilize

  DEBUG_PRINTLN("Jacuzzi Controller Starting...");

  // Initialize I2C with explicit ESP8266 pins
  Wire.begin(PIN_SDA, PIN_SCL);
  DEBUG_PRINTLN("I2C initialized");

  // Initialize OLED display
  DEBUG_PRINT("Initializing OLED display at address 0x");
  DEBUG_PRINTLN(String(I2C_OLED_ADDR, HEX));

  if (!display.begin(I2C_OLED_ADDR, true))
  {
    DEBUG_PRINTLN("ERROR: OLED display initialization failed!");
    DEBUG_PRINTLN("System halted. Check I2C connections.");
    // Enter infinite loop on failure
    while (1)
    {
      yield(); // Allow ESP8266 background tasks
    }
  }

  DEBUG_PRINTLN("OLED display initialized successfully");

  // Configure display settings
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  DEBUG_PRINTLN("Display configured");

  // Initialize PCF8574 I/O expander
  if (!initPCF8574())
  {
    DEBUG_PRINTLN("ERROR: PCF8574 initialization failed!");
    DEBUG_PRINTLN("System halted. Check I2C connections and address.");

    // Display error on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("PCF8574 ERROR");
    display.println("Check I2C");
    display.display();

    // Enter infinite loop on failure
    while (1)
    {
      yield(); // Allow ESP8266 background tasks
    }
  }
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop()
{
  // Empty for now - will be populated in later phases
  yield(); // Allow ESP8266 background tasks
}
