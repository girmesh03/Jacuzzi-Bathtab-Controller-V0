// ============================================================================
// MENU.CPP - Menu System Management Implementation
// ============================================================================
// Implements menu navigation, structure, and user interaction logic
//
// Target Platform: ESP8266 (ESP-12E/ESP-12F)
// Phase 4: Basic Menu System
// ============================================================================

#include "Menu.h"
#include "Constants.h"
#include "Bitmaps.h"

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
// MENU STRINGS (PROGMEM) - For status text only
// ============================================================================
const char STR_CIRCULATION[] PROGMEM = "Circulation";
const char STR_MASSAGE[] PROGMEM = "Massage";
const char STR_JET[] PROGMEM = "Jet";
const char STR_HEATER[] PROGMEM = "Heater";
const char STR_OZONE[] PROGMEM = "Ozone";
const char STR_SPEAKER[] PROGMEM = "Speaker";
const char STR_LIGHT[] PROGMEM = "Light";
const char STR_SETTINGS[] PROGMEM = "Settings";
const char STR_TARGET_TEMP[] PROGMEM = "Target Temp";
const char STR_IDLE_TIMEOUT[] PROGMEM = "Idle Timeout";
const char STR_ABOUT[] PROGMEM = "About";
const char STR_ON[] PROGMEM = " ON";
const char STR_OFF[] PROGMEM = " OFF";

// ============================================================================
// MAIN MENU DATA (PROGMEM) - With bitmaps
// ============================================================================
const MenuItem mainMenuItems[] PROGMEM = {
    {STR_CIRCULATION, circulation_bitmap, MENU_IDLE, ACTUATOR_CIRCULATION_PUMP},
    {STR_MASSAGE, massage_bitmap, MENU_IDLE, ACTUATOR_MASSAGE_PUMP},
    {STR_JET, jet_bitmap, MENU_IDLE, ACTUATOR_JET_PUMP},
    {STR_HEATER, heater_bitmap, MENU_IDLE, ACTUATOR_HEATER},
    {STR_OZONE, ozone_bitmap, MENU_IDLE, ACTUATOR_OZONE},
    {STR_SPEAKER, speaker_bitmap, MENU_IDLE, ACTUATOR_SPEAKER},
    {STR_LIGHT, light_bulb_bitmap, MENU_IDLE, ACTUATOR_LIGHT},
    {STR_SETTINGS, settings_bitmap, MENU_SETTINGS, -1}
};
const uint8_t mainMenuCount = 8;

// ============================================================================
// SETTINGS MENU DATA (PROGMEM) - With bitmaps
// ============================================================================
const MenuItem settingsMenuItems[] PROGMEM = {
    {STR_TARGET_TEMP, thermometer_bitmap, MENU_SETTINGS_TEMP, -1},
    {STR_IDLE_TIMEOUT, settings_bitmap, MENU_SETTINGS_TIMEOUT, -1},
    {STR_ABOUT, settings_bitmap, MENU_SETTINGS_ABOUT, -1}
};
const uint8_t settingsMenuCount = 3;

// ============================================================================
// MENU MANAGER IMPLEMENTATION
// ============================================================================

/**
 * @brief Initialize menu manager
 * 
 * Sets initial menu state to idle screen with no selection.
 * Initializes interaction timestamp for idle timeout tracking.
 */
void MenuManager::init()
{
    DEBUG_PRINTLN("Initializing menu manager...");
    
    // Initialize menu state
    currentMenu = MENU_IDLE;
    selectedIndex = 0;
    lastInteractionTime = millis();
    
    DEBUG_PRINTLN("Menu manager initialized successfully");
}

/**
 * @brief Get menu item count for a given menu
 * 
 * @param menu Menu ID to get count for
 * @return Number of items in the menu
 */
uint8_t MenuManager::getMenuItemCount(MenuId menu)
{
    switch (menu)
    {
        case MENU_MAIN:
            return mainMenuCount;
        case MENU_SETTINGS:
            return settingsMenuCount;
        default:
            return 0;
    }
}

/**
 * @brief Handle encoder rotation for menu navigation
 * 
 * Updates menu selection with wrapping (first ↔ last).
 * Resets idle timeout on interaction.
 * 
 * @param clockwise True for clockwise rotation, false for counter-clockwise
 * 
 * Requirements: 12.2, 12.5
 */
void MenuManager::handleRotation(bool clockwise)
{
    // Update interaction time
    lastInteractionTime = millis();
    
    // Get current menu item count
    uint8_t itemCount = getMenuItemCount(currentMenu);
    
    // If no items in current menu, do nothing
    if (itemCount == 0)
    {
        return;
    }
    
    // Update selection with wrapping
    if (clockwise)
    {
        selectedIndex++;
        if (selectedIndex >= itemCount)
        {
            selectedIndex = 0;  // Wrap to first item
        }
        DEBUG_PRINT("Menu selection: ");
        DEBUG_PRINT(selectedIndex);
        DEBUG_PRINTLN(" (CW)");
    }
    else
    {
        if (selectedIndex == 0)
        {
            selectedIndex = itemCount - 1;  // Wrap to last item
        }
        else
        {
            selectedIndex--;
        }
        DEBUG_PRINT("Menu selection: ");
        DEBUG_PRINT(selectedIndex);
        DEBUG_PRINTLN(" (CCW)");
    }
}

/**
 * @brief Handle encoder button press for menu selection
 * 
 * Navigates between menus based on current state:
 * - From idle: enter main menu
 * - From main menu: if Settings, enter settings submenu; if actuator, toggle handled in main.cpp
 * - From settings menu: enter settings submenu
 * - From settings submenu: return to settings menu
 * 
 * NOTE: Actuator toggle is handled in main.cpp, not here.
 * 
 * Resets idle timeout on interaction.
 * 
 * Requirements: 12.2, 12.4
 */
void MenuManager::handlePress()
{
    // Update interaction time
    lastInteractionTime = millis();
    
    // Handle press based on current menu
    if (currentMenu == MENU_IDLE)
    {
        // Enter main menu from idle
        currentMenu = MENU_MAIN;
        selectedIndex = 0;
        DEBUG_PRINTLN("Entered main menu from idle");
    }
    else if (currentMenu == MENU_MAIN)
    {
        // Get selected menu item from PROGMEM
        MenuItem item;
        memcpy_P(&item, &mainMenuItems[selectedIndex], sizeof(MenuItem));
        
        // Only navigate to Settings submenu (actuator toggle handled in main.cpp)
        if (item.submenu == MENU_SETTINGS)
        {
            currentMenu = MENU_SETTINGS;
            selectedIndex = 0;
            DEBUG_PRINTLN("Navigated to Settings menu");
        }
        // For actuators (submenu == MENU_IDLE), do nothing here - toggle handled in main.cpp
    }
    else if (currentMenu == MENU_SETTINGS)
    {
        // Get selected menu item from PROGMEM
        MenuItem item;
        memcpy_P(&item, &settingsMenuItems[selectedIndex], sizeof(MenuItem));
        
        // Navigate to settings submenu
        if (item.submenu != MENU_IDLE)
        {
            currentMenu = item.submenu;
            selectedIndex = 0;
            DEBUG_PRINT("Navigated to settings submenu: ");
            DEBUG_PRINTLN((int)item.submenu);
        }
    }
    else if (currentMenu >= MENU_SETTINGS_TEMP && currentMenu <= MENU_SETTINGS_ABOUT)
    {
        // Settings submenus return to settings menu on button press
        currentMenu = MENU_SETTINGS;
        selectedIndex = 0;
        DEBUG_PRINTLN("Returned to settings menu from submenu");
    }
    else if (currentMenu == MENU_SETTINGS)
    {
        // Settings menu returns to main menu on button press (Phase 4 temporary)
        currentMenu = MENU_MAIN;
        selectedIndex = 0;
        DEBUG_PRINTLN("Returned to main menu from settings");
    }
}

/**
 * @brief Return to idle screen
 * 
 * Resets menu state to idle with no selection.
 * Called when idle timeout expires.
 * 
 * Requirements: 12.3, 37.3
 */
void MenuManager::returnToIdle()
{
    currentMenu = MENU_IDLE;
    selectedIndex = 0;
    DEBUG_PRINTLN("Idle timeout - returned to idle screen");
}

/**
 * @brief Update menu state (non-blocking)
 * 
 * Checks for idle timeout and returns to idle screen if expired.
 * Call this every loop iteration.
 * 
 * Requirements: 12.3, 37.1, 37.3
 */
void MenuManager::update()
{
    // Check idle timeout only if not already idle
    if (currentMenu != MENU_IDLE)
    {
        if (millis() - lastInteractionTime > IDLE_TIMEOUT_DEFAULT)
        {
            returnToIdle();
        }
    }
}

/**
 * @brief Get current menu ID
 * @return Current menu ID
 */
MenuId MenuManager::getCurrentMenu()
{
    return currentMenu;
}

/**
 * @brief Get current selected index
 * @return Current selected index
 */
uint8_t MenuManager::getSelectedIndex()
{
    return selectedIndex;
}

/**
 * @brief Check if currently on idle screen
 * @return True if on idle screen, false otherwise
 */
bool MenuManager::isIdle()
{
    return currentMenu == MENU_IDLE;
}

/**
 * @brief Get actuator ID for current menu selection in MENU_MAIN
 * 
 * Returns the actuator ID if current menu is MENU_MAIN and selected item
 * is an actuator, otherwise returns -1.
 * 
 * @return Actuator ID (0-7) or -1 if not an actuator menu item
 */
int8_t MenuManager::getSelectedActuatorId()
{
    // Only works in MENU_MAIN
    if (currentMenu != MENU_MAIN)
    {
        return -1;
    }
    
    // Get selected menu item from PROGMEM
    MenuItem item;
    memcpy_P(&item, &mainMenuItems[selectedIndex], sizeof(MenuItem));
    
    // Return actuator ID (will be -1 for Settings)
    return item.actuatorId;
}

/**
 * @brief Reset idle timeout
 * 
 * Called when user interacts with actuators to prevent timeout
 * during active use.
 */
void MenuManager::resetIdleTimeout()
{
    lastInteractionTime = millis();
}
