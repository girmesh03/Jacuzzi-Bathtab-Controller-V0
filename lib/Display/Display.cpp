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
    display->setTextSize(2); // Larger text size for better visibility
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

    // Set text size to 2 for better visibility
    display->setTextSize(2);

    // Display title - centered
    // "Jacuzzi" on first line
    display->setCursor(10, 10);
    display->println(F("Jacuzzi"));

    // "Controller" on second line
    display->setCursor(4, 30);
    display->println(F("Controller"));

    // Display version - smaller text, centered
    display->setTextSize(1);
    display->setCursor(52, 52);
    display->println(F("v1.0"));

    // Update display to show content
    display->display();

    DEBUG_PRINTLN("Splash screen displayed");

    // Non-blocking delay for 2 seconds
    uint32_t splashStart = millis();
    while (millis() - splashStart < 2000)
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

    // Set text size to 2 for better visibility
    display->setTextSize(2);

    // Display "READY" - centered
    display->setCursor(32, 24);
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

    // Set text size to 1 for error messages (more text fits)
    display->setTextSize(1);

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
