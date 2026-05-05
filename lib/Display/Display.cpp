// ============================================================================
// DISPLAY.CPP - OLED Display Management Implementation
// ============================================================================

#include "Display.h"
#include "Sensors.h"
#include "Menu.h"
#include "Actuators.h"
#include "Bitmaps.h"
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
    
    // Initialize frame rate limiting
    lastFrameTime = 0;
    needsRedraw = true;
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
// SHOW SPLASH SCREEN WITH WATER DROP BITMAP
// ============================================================================
void DisplayManager::showSplashScreen()
{
    DEBUG_PRINTLN("Displaying splash screen with water drop bitmap");

    // Clear display
    display->clearDisplay();

    // Draw water drop bitmap centered (128x64)
    display->drawBitmap(0, 0, water_drop_bitmap, WATER_DROP_BMPWIDTH, WATER_DROP_BMPHEIGHT, SH110X_WHITE);

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
// SHOW ERROR MESSAGE WITH ERROR BITMAP
// ============================================================================
void DisplayManager::showError(const char *message)
{
    DEBUG_PRINT("Displaying error: ");
    DEBUG_PRINTLN(message);

    // Clear display
    display->clearDisplay();

    // Draw error bitmap centered (using high temperature error bitmap as generic error)
    display->drawBitmap(0, 0, high_temperature_error_bitmap, HIGH_TEMPERATURE_ERROR_BMPWIDTH, HIGH_TEMPERATURE_ERROR_BMPHEIGHT, SH110X_WHITE);

    // Display error message at bottom center
    display->setTextSize(TEXT_SIZE_NORMAL);
    
    // Calculate text width for centering
    int16_t x1, y1;
    uint16_t w, h;
    display->getTextBounds(message, 0, 0, &x1, &y1, &w, &h);
    uint8_t x = (SCREEN_WIDTH - w) / 2;
    
    display->setCursor(x, 54);
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

// ============================================================================
// SHOW SENSOR DATA WITH COUNTER (Phase 3)
// ============================================================================
/**
 * @brief Display temperature, water level, and test counter on OLED
 * 
 * Displays current temperature (or "TEMP ERROR" if invalid)
 * Displays water level status ("Water: OK" or "LOW WATER" flashing)
 * Displays test counter value at bottom of screen
 * 
 * @param temperature Current temperature reading in °C
 * @param tempValid True if temperature reading is valid
 * @param waterLevelOK True if water level is OK
 * @param counter Test counter value
 * 
 * Requirements: 4.3, 5.5, 23.1-23.4, 24.1-24.4, Phase 3 integration
 */
void DisplayManager::showSensorDataWithCounter(float temperature, bool tempValid, bool waterLevelOK, int16_t counter)
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
    // DISPLAY TEST COUNTER (Phase 3)
    // ========================================================================
    display->setTextSize(TEXT_SIZE_NORMAL);
    display->setCursor(COUNTER_DISPLAY_X, COUNTER_DISPLAY_Y);
    display->print(F("Counter: "));
    display->println(counter);
    
    // ========================================================================
    // UPDATE DISPLAY
    // ========================================================================
    display->display();
}

// ============================================================================
// HELPER METHODS
// ============================================================================

/**
 * @brief Draw bitmap centered on screen
 * 
 * @param bitmap Pointer to bitmap data in PROGMEM
 * @param width Bitmap width in pixels
 * @param height Bitmap height in pixels
 */
void DisplayManager::drawCenteredBitmap(const unsigned char* bitmap, uint8_t width, uint8_t height)
{
    // Calculate centered position
    uint8_t x = (SCREEN_WIDTH - width) / 2;
    uint8_t y = (SCREEN_HEIGHT - height) / 2;
    
    // Draw bitmap
    display->drawBitmap(x, y, bitmap, width, height, SH110X_WHITE);
}

/**
 * @brief Draw text centered horizontally at specified Y position
 * 
 * @param text Text to draw (from PROGMEM or RAM)
 * @param y Y position for text
 */
void DisplayManager::drawCenteredText(const char* text, uint8_t y)
{
    // Get text bounds for centering
    int16_t x1, y1;
    uint16_t w, h;
    display->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    
    // Calculate centered X position
    uint8_t x = (SCREEN_WIDTH - w) / 2;
    
    // Draw text
    display->setCursor(x, y);
    display->print(text);
}

// ============================================================================
// PHASE 4: MENU SYSTEM DISPLAY METHODS
// ============================================================================

/**
 * @brief Display idle screen with temperature and water level
 * 
 * Shows large temperature display, water level status, and placeholder
 * for actuator status indicators (Phase 5).
 * 
 * @param sensors Pointer to SensorManager for current readings
 * @param actuators Pointer to ActuatorManager for actuator states
 * 
 * Requirements: 13.1-13.4, 24.2, 24.3
 */
void DisplayManager::showIdleScreen(SensorManager* sensors, ActuatorManager* actuators)
{
    display->clearDisplay();
    
    // ========================================================================
    // DISPLAY TEMPERATURE (LARGE, PROMINENT)
    // ========================================================================
    display->setTextSize(TEXT_SIZE_LARGE);
    display->setCursor(10, 10);
    
    if (sensors->isTemperatureValid())
    {
        display->print(sensors->getTemperature(), 1);
        display->println(F(" C"));
    }
    else
    {
        display->println(F("TEMP ERR"));
    }
    
    // ========================================================================
    // DISPLAY WATER LEVEL STATUS
    // ========================================================================
    display->setTextSize(TEXT_SIZE_NORMAL);
    display->setCursor(10, 40);
    
    if (sensors->isWaterLevelOK())
    {
        display->println(F("Water: OK"));
    }
    else
    {
        // Flash warning at 1 Hz
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
    // ACTUATOR STATUS INDICATORS (PLACEHOLDER FOR PHASE 5)
    // ========================================================================
    display->setCursor(0, 56);
    display->setTextSize(TEXT_SIZE_NORMAL);
    display->print(F("Status: Idle"));
    
    // ========================================================================
    // UPDATE DISPLAY
    // ========================================================================
    display->display();
}

/**
 * @brief Display main menu with bitmap-based rendering
 * 
 * Shows selected menu item's bitmap centered on screen with label at bottom center.
 * No scrolling - one bitmap at a time.
 * 
 * @param menu Pointer to MenuManager for current menu state
 * @param sensors Pointer to SensorManager for header display
 * 
 * Requirements: 11.3, 11.4, 11.5, 11.6
 */
void DisplayManager::showMainMenu(MenuManager* menu, SensorManager* sensors)
{
    display->clearDisplay();
    
    // Get selected menu item from PROGMEM
    uint8_t selectedIndex = menu->getSelectedIndex();
    MenuItem item;
    memcpy_P(&item, &mainMenuItems[selectedIndex], sizeof(MenuItem));
    
    // Draw bitmap shifted up to leave space for text at bottom
    const unsigned char* bitmap = (const unsigned char*)pgm_read_ptr(&item.bitmap);
    display->drawBitmap(0, -5, bitmap, 128, 64, SH110X_WHITE);
    
    // Draw label at bottom center
    display->setTextSize(TEXT_SIZE_NORMAL);
    char label[32];
    strcpy_P(label, item.label);
    drawCenteredText(label, 56);
    
    // Update display
    display->display();
}

/**
 * @brief Display settings menu with bitmap-based rendering
 * 
 * Similar to main menu but for settings items.
 * 
 * @param menu Pointer to MenuManager for current menu state
 * 
 * Requirements: 11.3, 11.4, 11.5
 */
void DisplayManager::showSettingsMenu(MenuManager* menu)
{
    display->clearDisplay();
    
    // Get selected menu item from PROGMEM
    uint8_t selectedIndex = menu->getSelectedIndex();
    MenuItem item;
    memcpy_P(&item, &settingsMenuItems[selectedIndex], sizeof(MenuItem));
    
    // Draw bitmap shifted up to leave space for text at bottom
    const unsigned char* bitmap = (const unsigned char*)pgm_read_ptr(&item.bitmap);
    display->drawBitmap(0, -5, bitmap, 128, 64, SH110X_WHITE);
    
    // Draw label at bottom center
    display->setTextSize(TEXT_SIZE_NORMAL);
    char label[32];
    strcpy_P(label, item.label);
    drawCenteredText(label, 56);
    
    // Update display
    display->display();
}

/**
 * @brief Display actuator control screen with bitmap and status
 * 
 * Shows actuator bitmap centered with label and ON/OFF status at bottom center.
 * 
 * @param menu Pointer to MenuManager for current menu state
 * @param actuators Pointer to ActuatorManager for actuator states
 * @param actuatorId Actuator ID (0-7)
 */
void DisplayManager::showActuatorControl(MenuManager* menu, ActuatorManager* actuators, int8_t actuatorId)
{
    display->clearDisplay();
    
    // Get current menu item from PROGMEM
    MenuItem item;
    
    // Determine which menu we're in
    MenuId currentMenu = menu->getCurrentMenu();
    if (currentMenu >= MENU_CIRCULATION_PUMP && currentMenu <= MENU_LIGHT)
    {
        // Calculate index in main menu
        uint8_t menuIndex = currentMenu - MENU_CIRCULATION_PUMP;
        memcpy_P(&item, &mainMenuItems[menuIndex], sizeof(MenuItem));
    }
    else
    {
        // Fallback - shouldn't happen
        return;
    }
    
    // Draw bitmap shifted up to leave space for text at bottom
    const unsigned char* bitmap = (const unsigned char*)pgm_read_ptr(&item.bitmap);
    display->drawBitmap(0, -5, bitmap, 128, 64, SH110X_WHITE);
    
    // Get actuator state
    bool state = actuators->getState(actuatorId);
    
    // Build status string: "Label ON" or "Label OFF"
    char label[32];
    strcpy_P(label, item.label);
    
    char status[42];
    strcpy(status, label);
    if (state)
    {
        strcat_P(status, STR_ON);
    }
    else
    {
        strcat_P(status, STR_OFF);
    }
    
    // Draw status at bottom center
    display->setTextSize(TEXT_SIZE_NORMAL);
    drawCenteredText(status, 54);
    
    // Update display
    display->display();
}

/**
 * @brief Update display based on current menu state
 * 
 * Implements frame rate limiting (10 FPS minimum) and routes to appropriate
 * screen rendering method based on current menu.
 * 
 * @param sensors Pointer to SensorManager for sensor data
 * @param menu Pointer to MenuManager for menu state
 * @param actuators Pointer to ActuatorManager for actuator states
 * 
 * Requirements: 11.2, 11.7, 17.1, 17.2
 */
void DisplayManager::update(SensorManager* sensors, MenuManager* menu, ActuatorManager* actuators)
{
    // ========================================================================
    // FRAME RATE LIMITING (10 FPS MINIMUM = 100ms)
    // ========================================================================
    if (millis() - lastFrameTime < DISPLAY_MIN_FRAME_TIME)
    {
        return;  // Skip update if not enough time elapsed
    }
    
    lastFrameTime = millis();
    
    // ========================================================================
    // ROUTE TO APPROPRIATE SCREEN
    // ========================================================================
    MenuId currentMenu = menu->getCurrentMenu();
    
    switch (currentMenu)
    {
        case MENU_IDLE:
            showIdleScreen(sensors, actuators);
            break;
            
        case MENU_MAIN:
            showMainMenu(menu, sensors);
            break;
            
        case MENU_SETTINGS:
            showSettingsMenu(menu);
            break;
            
        case MENU_CIRCULATION_PUMP:
        case MENU_MASSAGE_PUMP:
        case MENU_JET_PUMP:
        case MENU_HEATER:
        case MENU_OZONE:
        case MENU_SPEAKER:
        case MENU_LIGHT:
            // Actuator control screens
            showActuatorControl(menu, actuators, menu->getSelectedActuatorId());
            break;
            
        case MENU_SETTINGS_TEMP:
        case MENU_SETTINGS_TIMEOUT:
        case MENU_SETTINGS_ABOUT:
            // Settings submenus (placeholder for Phase 7)
            display->clearDisplay();
            display->setTextSize(TEXT_SIZE_NORMAL);
            display->setCursor(0, 20);
            display->println(F("Settings submenu"));
            display->println(F("(Phase 7)"));
            display->display();
            break;
            
        default:
            // Unknown menu - display error
            display->clearDisplay();
            display->setTextSize(TEXT_SIZE_NORMAL);
            display->setCursor(0, 20);
            display->println(F("Unknown menu"));
            display->display();
            break;
    }
}
