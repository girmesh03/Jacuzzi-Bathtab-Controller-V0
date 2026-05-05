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
    
    // Initialize frame rate limiting and state
    lastFrameTime = 0;
    needsRedraw = true;
    waterLevelFlashTime = 0;
    waterLevelFlashState = false;
    
    // Initialize error queue
    errorCount = 0;
    currentErrorIndex = 0;
    for (uint8_t i = 0; i < MAX_ERRORS; i++)
    {
        errorQueue[i].message[0] = '\0';
        errorQueue[i].critical = true;
    }
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
// ERROR QUEUE MANAGEMENT
// ============================================================================

/**
 * @brief Add error to queue
 * 
 * Adds error message to queue if not already present and queue not full.
 * 
 * @param message Error message to add
 * @param critical True = blocks everything (safety), False = dismissible (user action)
 */
void DisplayManager::addError(const char *message, bool critical)
{
    // Check if error already exists in queue
    for (uint8_t i = 0; i < errorCount; i++)
    {
        if (strcmp(errorQueue[i].message, message) == 0)
        {
            // Already in queue - don't log again
            return;
        }
    }
    
    // Check if queue is full
    if (errorCount >= MAX_ERRORS)
    {
        DEBUG_PRINTLN("ERROR: Error queue full!");
        return;
    }
    
    // Add error to queue
    strncpy(errorQueue[errorCount].message, message, 31);
    errorQueue[errorCount].message[31] = '\0';
    errorQueue[errorCount].critical = critical;
    errorCount++;
    
    // Log only when actually adding
    DEBUG_PRINT("Error added: ");
    DEBUG_PRINT(message);
    DEBUG_PRINT(critical ? " (CRITICAL)" : " (DISMISSIBLE)");
    DEBUG_PRINT(" - Total: ");
    DEBUG_PRINTLN(errorCount);
}

/**
 * @brief Remove specific error from queue
 * 
 * Removes error message from queue if present.
 * 
 * @param message Error message to remove
 */
void DisplayManager::removeError(const char *message)
{
    // Find error in queue
    for (uint8_t i = 0; i < errorCount; i++)
    {
        if (strcmp(errorQueue[i].message, message) == 0)
        {
            // Found - shift remaining errors down
            for (uint8_t j = i; j < errorCount - 1; j++)
            {
                strcpy(errorQueue[j].message, errorQueue[j + 1].message);
                errorQueue[j].critical = errorQueue[j + 1].critical;
            }
            errorCount--;
            
            // Adjust current index if needed
            if (currentErrorIndex >= errorCount && errorCount > 0)
            {
                currentErrorIndex = errorCount - 1;
            }
            
            // Log only when actually removing
            DEBUG_PRINT("Error removed: ");
            DEBUG_PRINT(message);
            DEBUG_PRINT(" - Remaining: ");
            DEBUG_PRINTLN(errorCount);
            return;
        }
    }
    
    // Not found - don't log
}

/**
 * @brief Clear all errors from queue
 */
void DisplayManager::clearAllErrors()
{
    errorCount = 0;
    currentErrorIndex = 0;
    DEBUG_PRINTLN("All errors cleared");
}

/**
 * @brief Check if any errors exist in queue
 * 
 * @return true if errors exist, false otherwise
 */
bool DisplayManager::hasErrors()
{
    return errorCount > 0;
}

/**
 * @brief Check if any critical errors exist in queue
 * 
 * @return true if critical errors exist, false otherwise
 */
bool DisplayManager::hasCriticalErrors()
{
    for (uint8_t i = 0; i < errorCount; i++)
    {
        if (errorQueue[i].critical)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Get current error message
 * 
 * @return Pointer to current error message, or nullptr if no errors
 */
const char* DisplayManager::getCurrentError()
{
    if (errorCount == 0)
    {
        return nullptr;
    }
    
    return errorQueue[currentErrorIndex].message;
}

/**
 * @brief Check if current error is critical
 * 
 * @return true if current error is critical, false otherwise
 */
bool DisplayManager::isCurrentErrorCritical()
{
    if (errorCount == 0)
    {
        return false;
    }
    
    return errorQueue[currentErrorIndex].critical;
}

/**
 * @brief Navigate to next error in queue
 */
void DisplayManager::nextError()
{
    if (errorCount == 0)
    {
        return;
    }
    
    currentErrorIndex++;
    if (currentErrorIndex >= errorCount)
    {
        currentErrorIndex = 0;  // Wrap to first
    }
    
    DEBUG_PRINT("Next error: ");
    DEBUG_PRINTLN(currentErrorIndex);
}

/**
 * @brief Navigate to previous error in queue
 */
void DisplayManager::prevError()
{
    if (errorCount == 0)
    {
        return;
    }
    
    if (currentErrorIndex == 0)
    {
        currentErrorIndex = errorCount - 1;  // Wrap to last
    }
    else
    {
        currentErrorIndex--;
    }
    
    DEBUG_PRINT("Previous error: ");
    DEBUG_PRINTLN(currentErrorIndex);
}

/**
 * @brief Dismiss current error (only if not critical)
 */
void DisplayManager::dismissCurrentError()
{
    if (errorCount == 0)
    {
        return;
    }
    
    // Check if current error is dismissible
    if (!errorQueue[currentErrorIndex].critical)
    {
        DEBUG_PRINT("Dismissing error: ");
        DEBUG_PRINTLN(errorQueue[currentErrorIndex].message);
        
        // Remove current error
        char msgCopy[32];
        strcpy(msgCopy, errorQueue[currentErrorIndex].message);
        removeError(msgCopy);
    }
    else
    {
        DEBUG_PRINTLN("Cannot dismiss critical error");
    }
}

/**
 * @brief Get total error count
 * 
 * @return Number of errors in queue
 */
uint8_t DisplayManager::getErrorCount()
{
    return errorCount;
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

/**
 * @brief Get centralized water level flash state
 * 
 * Provides synchronized flashing across all screens.
 * 
 * @return true if flash should be visible, false otherwise
 */
bool DisplayManager::getWaterLevelFlashState()
{
    if (millis() - waterLevelFlashTime >= WATER_LEVEL_FLASH_INTERVAL)
    {
        waterLevelFlashState = !waterLevelFlashState;
        waterLevelFlashTime = millis();
    }
    return waterLevelFlashState;
}

// ============================================================================
// PHASE 4: MENU SYSTEM DISPLAY METHODS
// ============================================================================

/**
 * @brief Display idle screen with thermometer bitmap and temperature
 * 
 * Shows 2-column layout:
 * - Left column: Centered thermometer bitmap (32x32)
 * - Right column: Centered temperature value
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
    // LEFT COLUMN: THERMOMETER BITMAP (32x32 centered vertically)
    // ========================================================================
    uint8_t bitmapX = 16;  // Center in left half (64/2 - 32/2 = 16)
    uint8_t bitmapY = 16;  // Center vertically (64/2 - 32/2 = 16)
    display->drawBitmap(bitmapX, bitmapY, thermometer_small_bitmap, 
                       THERMOMETER_SMALL_BMPWIDTH, THERMOMETER_SMALL_BMPHEIGHT, SH110X_WHITE);
    
    // ========================================================================
    // RIGHT COLUMN: TEMPERATURE VALUE (centered vertically)
    // ========================================================================
    display->setTextSize(TEXT_SIZE_LARGE);
    display->setTextColor(SH110X_WHITE);
    
    if (sensors->isTemperatureValid())
    {
        // Format temperature as "XX.X C"
        char tempStr[16];
        dtostrf(sensors->getTemperature(), 4, 1, tempStr);  // 4 chars wide, 1 decimal
        strcat(tempStr, " C");
        
        // Calculate position for right column (centered)
        int16_t x1, y1;
        uint16_t w, h;
        display->getTextBounds(tempStr, 0, 0, &x1, &y1, &w, &h);
        
        uint8_t textX = 64 + (64 - w) / 2;  // Center in right half
        uint8_t textY = (64 - h) / 2;       // Center vertically
        
        display->setCursor(textX, textY);
        display->print(tempStr);
    }
    else
    {
        // Display error message
        display->setTextSize(TEXT_SIZE_NORMAL);
        display->setCursor(70, 24);
        display->println(F("TEMP"));
        display->setCursor(70, 34);
        display->println(F("ERROR"));
    }
    
    // ========================================================================
    // BOTTOM: WATER LEVEL WARNING (if low)
    // ========================================================================
    if (!sensors->isWaterLevelOK())
    {
        // Flash warning at 1 Hz
        if (getWaterLevelFlashState())
        {
            display->setTextSize(TEXT_SIZE_NORMAL);
            drawCenteredText("LOW WATER", 54);
        }
    }
    
    // ========================================================================
    // UPDATE DISPLAY
    // ========================================================================
    display->display();
}

/**
 * @brief Display main menu with bitmap-based rendering and actuator states
 * 
 * Shows selected menu item's bitmap centered on screen with label and state at bottom center.
 * For actuators: shows "Label ON" or "Label OFF"
 * For Settings: shows "Settings"
 * Bitmap is positioned to not overlap with text.
 * 
 * @param menu Pointer to MenuManager for current menu state
 * @param sensors Pointer to SensorManager for header display
 * @param actuators Pointer to ActuatorManager for actuator states
 * 
 * Requirements: 11.3, 11.4, 11.5, 11.6
 */
void DisplayManager::showMainMenu(MenuManager* menu, SensorManager* sensors, ActuatorManager* actuators)
{
    display->clearDisplay();
    
    // Get selected menu item from PROGMEM
    uint8_t selectedIndex = menu->getSelectedIndex();
    MenuItem item;
    memcpy_P(&item, &mainMenuItems[selectedIndex], sizeof(MenuItem));
    
    // Draw bitmap centered horizontally, positioned to leave space for text at bottom
    // Bitmap is 128x64, but we shift it up by 10 pixels to leave room for text
    const unsigned char* bitmap = (const unsigned char*)pgm_read_ptr(&item.bitmap);
    display->drawBitmap(0, -10, bitmap, 128, 64, SH110X_WHITE);
    
    // Build status string
    char statusText[42];
    char label[32];
    strcpy_P(label, item.label);
    
    if (item.actuatorId >= 0)
    {
        // Actuator item - show state (ON/OFF)
        bool state = actuators->getState(item.actuatorId);
        strcpy(statusText, label);
        if (state)
        {
            strcat_P(statusText, STR_ON);
        }
        else
        {
            strcat_P(statusText, STR_OFF);
        }
    }
    else
    {
        // Settings item - just show label
        strcpy(statusText, label);
    }
    
    // Draw status at bottom center (text size 1)
    display->setTextSize(TEXT_SIZE_NORMAL);
    display->setTextColor(SH110X_WHITE);
    drawCenteredText(statusText, 56);
    
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
    
    // Draw bitmap centered horizontally, positioned to leave space for text at bottom
    const unsigned char* bitmap = (const unsigned char*)pgm_read_ptr(&item.bitmap);
    display->drawBitmap(0, -10, bitmap, 128, 64, SH110X_WHITE);
    
    // Draw label at bottom center
    display->setTextSize(TEXT_SIZE_NORMAL);
    display->setTextColor(SH110X_WHITE);
    char label[32];
    strcpy_P(label, item.label);
    drawCenteredText(label, 56);
    
    // Update display
    display->display();
}

/**
 * @brief Display error screen with current error from queue
 * 
 * Shows error bitmap centered with error message at bottom center.
 * If multiple errors exist, shows current error with navigation indicators.
 * Shows "Press to dismiss" for non-critical errors.
 * Uses appropriate bitmap based on error type.
 * 
 * Requirements: Error display system
 */
void DisplayManager::showErrorScreen()
{
    display->clearDisplay();
    
    // Get current error message
    const char* errorMsg = getCurrentError();
    if (errorMsg == nullptr)
    {
        return;
    }
    
    // Select appropriate error bitmap based on error type
    const unsigned char* errorBitmap = high_temperature_error_bitmap;
    uint8_t bitmapWidth = HIGH_TEMPERATURE_ERROR_BMPWIDTH;
    uint8_t bitmapHeight = HIGH_TEMPERATURE_ERROR_BMPHEIGHT;
    
    if (strstr(errorMsg, "WATER") != nullptr || strstr(errorMsg, "Water") != nullptr)
    {
        // Water-related error
        errorBitmap = low_water_level_error_bitmap;
        bitmapWidth = LOW_WATER_LEVEL_ERROR_BMPWIDTH;
        bitmapHeight = LOW_WATER_LEVEL_ERROR_BMPHEIGHT;
    }
    else if (strstr(errorMsg, "SENSOR") != nullptr || strstr(errorMsg, "Sensor") != nullptr)
    {
        // Sensor-related error
        errorBitmap = sensor_error_bitmap;
        bitmapWidth = SENSOR_ERROR_BMPWIDTH;
        bitmapHeight = SENSOR_ERROR_BMPHEIGHT;
    }
    else if (strstr(errorMsg, "TEMP") != nullptr || strstr(errorMsg, "Temp") != nullptr)
    {
        // Temperature-related error
        errorBitmap = high_temperature_error_bitmap;
        bitmapWidth = HIGH_TEMPERATURE_ERROR_BMPWIDTH;
        bitmapHeight = HIGH_TEMPERATURE_ERROR_BMPHEIGHT;
    }
    
    // Draw error bitmap centered (128x64)
    display->drawBitmap(0, 0, errorBitmap, bitmapWidth, bitmapHeight, SH110X_WHITE);
    
    // Build error text with count if multiple errors
    char errorText[42];
    if (errorCount > 1)
    {
        // Show "Error X/Y: Message"
        snprintf(errorText, sizeof(errorText), "%d/%d: %s", 
                currentErrorIndex + 1, errorCount, errorMsg);
    }
    else
    {
        // Show just message
        strncpy(errorText, errorMsg, sizeof(errorText) - 1);
        errorText[sizeof(errorText) - 1] = '\0';
    }
    
    // Display error text at bottom center (text size 1)
    display->setTextSize(TEXT_SIZE_NORMAL);
    display->setTextColor(SH110X_WHITE);
    drawCenteredText(errorText, 48);
    
    // Show dismissal hint for non-critical errors
    if (!isCurrentErrorCritical())
    {
        drawCenteredText("Press to exit", 56);
    }
    
    // Update display
    display->display();
}

/**
 * @brief Update display based on current menu state
 * 
 * Implements frame rate limiting (10 FPS minimum) and routes to appropriate
 * screen rendering method based on current menu.
 * 
 * CRITICAL: If critical errors exist, only shows error screen and blocks all other displays.
 * Non-critical errors can be dismissed to continue operation.
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
    // ERROR STATE CHECK (HIGHEST PRIORITY)
    // ========================================================================
    if (hasErrors())
    {
        // Errors exist - show error screen
        if (millis() - lastFrameTime >= DISPLAY_MIN_FRAME_TIME)
        {
            showErrorScreen();
            lastFrameTime = millis();
        }
        return;  // Block all other display updates while errors exist
    }
    
    // ========================================================================
    // FRAME RATE LIMITING (10 FPS MINIMUM = 100ms)
    // ========================================================================
    if (!needsRedraw && (millis() - lastFrameTime < DISPLAY_MIN_FRAME_TIME))
    {
        return;  // Skip update if not enough time elapsed and no forced redraw
    }
    
    lastFrameTime = millis();
    needsRedraw = false;  // Clear dirty flag
    
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
            showMainMenu(menu, sensors, actuators);
            break;
            
        case MENU_SETTINGS:
            showSettingsMenu(menu);
            break;
            
        case MENU_SETTINGS_TEMP:
        case MENU_SETTINGS_TIMEOUT:
        case MENU_SETTINGS_ABOUT:
            // Settings submenus (placeholder for Phase 7)
            display->clearDisplay();
            display->setTextSize(TEXT_SIZE_NORMAL);
            display->setTextColor(SH110X_WHITE);
            drawCenteredText("Settings", 20);
            drawCenteredText("(Phase 7)", 30);
            display->display();
            break;
            
        default:
            // Unknown menu - display error
            display->clearDisplay();
            display->setTextSize(TEXT_SIZE_NORMAL);
            display->setTextColor(SH110X_WHITE);
            drawCenteredText("Unknown menu", 28);
            display->display();
            break;
    }
}
