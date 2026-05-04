// ============================================================================
// DISPLAY.CPP - OLED Display Management Implementation
// ============================================================================

#include "Display.h"
#include "Sensors.h"
#include "Menu.h"
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
// PHASE 4: MENU SYSTEM DISPLAY METHODS
// ============================================================================

/**
 * @brief Draw header with temperature and water level (compact)
 * 
 * Displays temperature and water level status in top 12 pixels.
 * Used in menu screens to maintain status visibility.
 * 
 * @param sensors Pointer to SensorManager for current readings
 */
void DisplayManager::drawHeader(SensorManager* sensors)
{
    display->setTextSize(TEXT_SIZE_NORMAL);
    display->setCursor(0, 0);
    
    // Display temperature (compact format)
    if (sensors->isTemperatureValid())
    {
        display->print(sensors->getTemperature(), 1);
        display->print(F("C "));
    }
    else
    {
        display->print(F("ERR "));
    }
    
    // Display water level status
    if (sensors->isWaterLevelOK())
    {
        display->print(F("H2O:OK"));
    }
    else
    {
        display->print(F("LOW H2O"));
    }
}

/**
 * @brief Draw a single menu item
 * 
 * Renders menu item with selection highlight (inverse video).
 * 
 * @param label Menu item label (from PROGMEM)
 * @param y Y position for menu item
 * @param selected True if this item is selected
 */
void DisplayManager::drawMenuItem(const char* label, uint8_t y, bool selected)
{
    display->setTextSize(TEXT_SIZE_NORMAL);
    
    // Set colors based on selection
    if (selected)
    {
        display->setTextColor(SH110X_BLACK, SH110X_WHITE);  // Inverse video
    }
    else
    {
        display->setTextColor(SH110X_WHITE);
    }
    
    // Draw menu item
    display->setCursor(4, y);
    
    // Load string from PROGMEM
    char buffer[32];
    strcpy_P(buffer, label);
    display->println(buffer);
    
    // Reset text color
    display->setTextColor(SH110X_WHITE);
}

/**
 * @brief Display idle screen with temperature and water level
 * 
 * Shows large temperature display, water level status, and placeholder
 * for actuator status indicators (Phase 5).
 * 
 * @param sensors Pointer to SensorManager for current readings
 * 
 * Requirements: 13.1-13.4, 24.2, 24.3
 */
void DisplayManager::showIdleScreen(SensorManager* sensors)
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
 * @brief Display main menu with scrolling and selection highlight
 * 
 * Shows menu title, temperature/water level header, and 3 visible menu items.
 * Selected item is highlighted with inverse video.
 * Scroll indicators shown if more items above/below.
 * 
 * @param menu Pointer to MenuManager for current menu state
 * @param sensors Pointer to SensorManager for header display
 * 
 * Requirements: 11.3, 11.4, 11.5, 11.6
 */
void DisplayManager::showMainMenu(MenuManager* menu, SensorManager* sensors)
{
    display->clearDisplay();
    
    // ========================================================================
    // DRAW HEADER (TOP 12 PIXELS)
    // ========================================================================
    drawHeader(sensors);
    
    // ========================================================================
    // DRAW MENU TITLE
    // ========================================================================
    display->setTextSize(TEXT_SIZE_NORMAL);
    display->setCursor(30, 14);
    display->println(F("Main Menu"));
    
    // ========================================================================
    // DRAW MENU ITEMS (3 VISIBLE)
    // ========================================================================
    uint8_t selectedIndex = menu->getSelectedIndex();
    uint8_t itemCount = mainMenuCount;
    
    // Calculate visible range (3 items, selected in middle when possible)
    int8_t startIndex = selectedIndex - 1;
    if (startIndex < 0) startIndex = 0;
    if (startIndex + 3 > itemCount) startIndex = itemCount - 3;
    if (startIndex < 0) startIndex = 0;
    
    // Draw up to 3 visible items
    for (uint8_t i = 0; i < 3 && (startIndex + i) < itemCount; i++)
    {
        uint8_t itemIndex = startIndex + i;
        uint8_t yPos = 26 + (i * 12);
        
        // Get menu item from PROGMEM
        MenuItem item;
        memcpy_P(&item, &mainMenuItems[itemIndex], sizeof(MenuItem));
        
        // Draw menu item
        drawMenuItem(item.label, yPos, itemIndex == selectedIndex);
    }
    
    // ========================================================================
    // DRAW SCROLL INDICATORS
    // ========================================================================
    display->setTextSize(TEXT_SIZE_NORMAL);
    
    // Up arrow if more items above
    if (startIndex > 0)
    {
        display->setCursor(120, 26);
        display->print(F("^"));
    }
    
    // Down arrow if more items below
    if (startIndex + 3 < itemCount)
    {
        display->setCursor(120, 50);
        display->print(F("v"));
    }
    
    // ========================================================================
    // UPDATE DISPLAY
    // ========================================================================
    display->display();
}

/**
 * @brief Display settings menu with scrolling and selection highlight
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
    
    // ========================================================================
    // DRAW MENU TITLE
    // ========================================================================
    display->setTextSize(TEXT_SIZE_NORMAL);
    display->setCursor(30, 0);
    display->println(F("Settings"));
    
    // ========================================================================
    // DRAW MENU ITEMS (3 VISIBLE)
    // ========================================================================
    uint8_t selectedIndex = menu->getSelectedIndex();
    uint8_t itemCount = settingsMenuCount;
    
    // Calculate visible range
    int8_t startIndex = selectedIndex - 1;
    if (startIndex < 0) startIndex = 0;
    if (startIndex + 3 > itemCount) startIndex = itemCount - 3;
    if (startIndex < 0) startIndex = 0;
    
    // Draw up to 3 visible items
    for (uint8_t i = 0; i < 3 && (startIndex + i) < itemCount; i++)
    {
        uint8_t itemIndex = startIndex + i;
        uint8_t yPos = 14 + (i * 12);
        
        // Get menu item from PROGMEM
        MenuItem item;
        memcpy_P(&item, &settingsMenuItems[itemIndex], sizeof(MenuItem));
        
        // Draw menu item
        drawMenuItem(item.label, yPos, itemIndex == selectedIndex);
    }
    
    // ========================================================================
    // DRAW SCROLL INDICATORS
    // ========================================================================
    display->setTextSize(TEXT_SIZE_NORMAL);
    
    // Up arrow if more items above
    if (startIndex > 0)
    {
        display->setCursor(120, 14);
        display->print(F("^"));
    }
    
    // Down arrow if more items below
    if (startIndex + 3 < itemCount)
    {
        display->setCursor(120, 38);
        display->print(F("v"));
    }
    
    // ========================================================================
    // UPDATE DISPLAY
    // ========================================================================
    display->display();
}

/**
 * @brief Display actuator control screen (placeholder for Phase 5)
 * 
 * Shows actuator name and status. Full implementation in Phase 5.
 * 
 * @param menu Pointer to MenuManager for current menu state
 * @param actuatorId Actuator ID (0-7)
 */
void DisplayManager::showActuatorControl(MenuManager* menu, int8_t actuatorId)
{
    display->clearDisplay();
    
    // ========================================================================
    // DISPLAY ACTUATOR NAME
    // ========================================================================
    display->setTextSize(TEXT_SIZE_NORMAL);
    display->setCursor(0, 0);
    display->print(F("Actuator "));
    display->println(actuatorId);
    
    // ========================================================================
    // PLACEHOLDER FOR PHASE 5
    // ========================================================================
    display->setCursor(0, 20);
    display->println(F("Control screen"));
    display->println(F("(Phase 5)"));
    
    // ========================================================================
    // UPDATE DISPLAY
    // ========================================================================
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
 * 
 * Requirements: 11.2, 11.7, 17.1, 17.2
 */
void DisplayManager::update(SensorManager* sensors, MenuManager* menu)
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
            showIdleScreen(sensors);
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
            // Actuator control screens (placeholder for Phase 5)
            showActuatorControl(menu, menu->getSelectedActuatorId());
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
