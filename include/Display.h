// ============================================================================
// DISPLAY.H - OLED Display Management Module
// ============================================================================
// Handles all OLED display operations including initialization, rendering,
// and screen management for the Jacuzzi Controller System.
//
// Target Platform: ESP8266 (ESP-12E/ESP-12F)
// Display: SH1106 128x64 OLED at I2C address 0x3C
// ============================================================================

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SH110X.h>
#include "Constants.h"

// ============================================================================
// DISPLAY MANAGER CLASS
// ============================================================================
class DisplayManager
{
public:
    // Constructor
    DisplayManager();

    // Initialize display hardware
    bool init();

    // Display splash screen for 2 seconds (non-blocking)
    void showSplashScreen();

    // Display READY message after initialization
    void showReadyMessage();

    // Display error message
    void showError(const char *message);

    // Get display object reference (for advanced usage)
    Adafruit_SH1106G &getDisplay();

private:
    Adafruit_SH1106G *display; // Pointer to display object
};

#endif // DISPLAY_H
