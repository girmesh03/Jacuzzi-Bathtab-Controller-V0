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

    // Error queue management
    void addError(const char *message, bool critical = true);  // Add error (critical or dismissible)
    void removeError(const char *message);    // Remove specific error from queue
    void clearAllErrors();                    // Clear all errors
    bool hasErrors();                         // Check if any errors exist
    bool hasCriticalErrors();                 // Check if any critical errors exist
    const char* getCurrentError();            // Get current error message
    bool isCurrentErrorCritical();            // Check if current error is critical
    void nextError();                         // Navigate to next error
    void prevError();                         // Navigate to previous error
    void dismissCurrentError();               // Dismiss current error (if not critical)
    uint8_t getErrorCount();                  // Get total error count
    
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
    
    // Water level flash state (centralized)
    uint32_t waterLevelFlashTime;
    bool waterLevelFlashState;
    
    // Error queue system (max 8 errors)
    static const uint8_t MAX_ERRORS = 8;
    struct ErrorEntry {
        char message[32];
        bool critical;  // true = blocks everything, false = dismissible
    };
    ErrorEntry errorQueue[MAX_ERRORS];
    uint8_t errorCount;
    uint8_t currentErrorIndex;
    
    // Screen rendering methods
    void showIdleScreen(SensorManager* sensors, ActuatorManager* actuators);
    void showMainMenu(MenuManager* menu, SensorManager* sensors, ActuatorManager* actuators);
    void showSettingsMenu(MenuManager* menu);
    void showErrorScreen();  // Display current error from queue
    
    // Helper methods
    void drawHeader(SensorManager* sensors);
    void drawCenteredBitmap(const unsigned char* bitmap, uint8_t width, uint8_t height);
    void drawCenteredText(const char* text, uint8_t y);
    bool getWaterLevelFlashState();  // Centralized flash logic
};

#endif // DISPLAY_H
