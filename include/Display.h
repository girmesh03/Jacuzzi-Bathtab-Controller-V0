// ============================================================================
// DISPLAY.H - OLED Display Management Module
// ============================================================================
// Handles all OLED display operations including initialization, rendering,
// and screen management for the Jacuzzi Controller System.
//
// Target Platform: ESP8266 (ESP-12E/ESP-12F)
// Display: SH1106 128x64 OLED at I2C address 0x3C
// Phase 4: Basic Menu System with Bitmap Support
// ============================================================================

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SH110X.h>
#include "Constants.h"

// Forward declarations
class SensorManager;
class MenuManager;
class ActuatorManager;

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

    // Display splash screen with water drop bitmap (non-blocking)
    void showSplashScreen();

    // Display READY message after initialization
    void showReadyMessage();

    // Display error message with error bitmap
    void showError(const char *message);
    
    // Display sensor data (temperature and water level)
    void showSensorData(float temperature, bool tempValid, bool waterLevelOK);
    
    // Display sensor data with counter (Phase 3)
    void showSensorDataWithCounter(float temperature, bool tempValid, bool waterLevelOK, int16_t counter);
    
    // Update display based on current menu state (Phase 4)
    void update(SensorManager* sensors, MenuManager* menu, ActuatorManager* actuators);

    // Get display object reference (for advanced usage)
    Adafruit_SH1106G &getDisplay();

private:
    Adafruit_SH1106G *display; // Pointer to display object
    uint32_t lastFrameTime;    // Frame rate limiting
    bool needsRedraw;          // Dirty flag for optimization
    
    // Screen rendering methods
    void showIdleScreen(SensorManager* sensors, ActuatorManager* actuators);
    void showMainMenu(MenuManager* menu, SensorManager* sensors);
    void showSettingsMenu(MenuManager* menu);
    void showActuatorControl(MenuManager* menu, ActuatorManager* actuators, int8_t actuatorId);
    
    // Helper methods
    void drawHeader(SensorManager* sensors);
    void drawCenteredBitmap(const unsigned char* bitmap, uint8_t width, uint8_t height);
    void drawCenteredText(const char* text, uint8_t y);
};

#endif // DISPLAY_H
