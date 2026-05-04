// ============================================================================
// DISPLAY.CPP - OLED Display Management Implementation
// ============================================================================

#include "Display.h"
#include <Wire.h>

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
// CONSTRUCTOR
// ============================================================================
DisplayManager::DisplayManager()
{
    // Create display object on heap
    display = new Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
}

// ============================================================================
// INITIALIZE DISPLAY
// ============================================================================
bool DisplayManager::init()
{
    DEBUG_PRINT("Initializing OLED display at address 0x");
    DEBUG_PRINTLN(String(I2C_OLED_ADDR, HEX));

    // Initialize display with I2C address
    if (!display->begin(I2C_OLED_ADDR, true))
    {
        DEBUG_PRINTLN("ERROR: OLED display initialization failed!");
        return false;
    }

    DEBUG_PRINTLN("OLED display initialized successfully");

    // Configure display settings
    display->clearDisplay();
    display->setTextSize(TEXT_SIZE_LARGE); // Larger text size for better visibility
    display->setTextColor(SH110X_WHITE);
    display->setTextWrap(false); // Prevent text wrapping

    DEBUG_PRINTLN("Display configured");

    return true;
}

// ============================================================================
// SHOW SPLASH SCREEN
// ============================================================================
void DisplayManager::showSplashScreen()
{
    DEBUG_PRINTLN("Displaying splash screen");

    // Clear display
    display->clearDisplay();

    // Set text size to large for better visibility
    display->setTextSize(TEXT_SIZE_LARGE);

    // Display title - centered
    // "Jacuzzi" on first line
    display->setCursor(SPLASH_TITLE_LINE1_X, SPLASH_TITLE_LINE1_Y);
    display->println(F("Jacuzzi"));

    // "Controller" on second line
    display->setCursor(SPLASH_TITLE_LINE2_X, SPLASH_TITLE_LINE2_Y);
    display->println(F("Controller"));

    // Display version - smaller text, centered
    display->setTextSize(TEXT_SIZE_NORMAL);
    display->setCursor(SPLASH_VERSION_X, SPLASH_VERSION_Y);
    display->println(F("v1.0"));

    // Update display to show content
    display->display();

    DEBUG_PRINTLN("Splash screen displayed");

    // Non-blocking delay for SPLASH_SCREEN_DURATION
    uint32_t splashStart = millis();
    while (millis() - splashStart < SPLASH_SCREEN_DURATION)
    {
        yield(); // Allow ESP8266 background tasks
    }

    DEBUG_PRINTLN("Splash screen timeout complete");
}

// ============================================================================
// SHOW READY MESSAGE
// ============================================================================
void DisplayManager::showReadyMessage()
{
    DEBUG_PRINTLN("Displaying READY message");

    // Clear display
    display->clearDisplay();

    // Set text size to large for better visibility
    display->setTextSize(TEXT_SIZE_LARGE);

    // Display "READY" - centered
    display->setCursor(READY_MSG_X, READY_MSG_Y);
    display->println(F("READY"));

    // Update display to show content
    display->display();

    DEBUG_PRINTLN("System Ready");
}

// ============================================================================
// SHOW ERROR MESSAGE
// ============================================================================
void DisplayManager::showError(const char *message)
{
    DEBUG_PRINT("Displaying error: ");
    DEBUG_PRINTLN(message);

    // Clear display
    display->clearDisplay();

    // Set text size to normal for error messages (more text fits)
    display->setTextSize(TEXT_SIZE_NORMAL);

    // Display "ERROR:" header
    display->setCursor(0, 0);
    display->println(F("ERROR:"));

    // Display error message
    display->setCursor(0, 16);
    display->println(message);

    // Update display to show content
    display->display();
}

// ============================================================================
// GET DISPLAY OBJECT REFERENCE
// ============================================================================
Adafruit_SH1106G &DisplayManager::getDisplay()
{
    return *display;
}

// ============================================================================
// SHOW SENSOR DATA
// ============================================================================
/**
 * @brief Display temperature and water level on OLED
 * 
 * Displays current temperature (or "TEMP ERROR" if invalid)
 * Displays water level status ("Water: OK" or "LOW WATER" flashing)
 * 
 * @param temperature Current temperature reading in °C
 * @param tempValid True if temperature reading is valid
 * @param waterLevelOK True if water level is OK
 * 
 * Requirements: 4.3, 5.5, 23.1-23.4, 24.1-24.4
 */
void DisplayManager::showSensorData(float temperature, bool tempValid, bool waterLevelOK)
{
    // Clear display
    display->clearDisplay();
    
    // ========================================================================
    // DISPLAY TEMPERATURE
    // ========================================================================
    display->setTextSize(TEXT_SIZE_LARGE);
    display->setCursor(TEMP_DISPLAY_X, TEMP_DISPLAY_Y);
    
    if (tempValid)
    {
        // Display temperature with 0.1°C precision (XX.X°C format)
        display->print(temperature, 1);  // 1 decimal place
        display->println(F(" C"));  // Degree symbol not available in default font
    }
    else
    {
        // Display error message using PROGMEM string
        display->println(F("TEMP ERROR"));
    }
    
    // ========================================================================
    // DISPLAY WATER LEVEL
    // ========================================================================
    display->setTextSize(TEXT_SIZE_NORMAL);
    display->setCursor(WATER_STATUS_X, WATER_STATUS_Y);
    
    if (waterLevelOK)
    {
        // Water level OK - display normal message
        display->println(F("Water: OK"));
    }
    else
    {
        // Water level LOW - display flashing warning
        // Flash at 1 Hz (on for WATER_LEVEL_FLASH_INTERVAL ms, off for WATER_LEVEL_FLASH_INTERVAL ms)
        static uint32_t lastFlashTime = 0;
        static bool flashState = false;
        
        if (millis() - lastFlashTime >= WATER_LEVEL_FLASH_INTERVAL)
        {
            flashState = !flashState;
            lastFlashTime = millis();
        }
        
        if (flashState)
        {
            display->println(F("LOW WATER"));
        }
    }
    
    // ========================================================================
    // UPDATE DISPLAY
    // ========================================================================
    display->display();
}
