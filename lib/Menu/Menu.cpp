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
// MENU STRINGS (PROGMEM)
// ============================================================================
const char STR_MAIN_MENU[] PROGMEM = "Main Menu";
const char STR_CIRCULATION[] PROGMEM = "Circulation Pump";
const char STR_MASSAGE[] PROGMEM = "Massage Pump";
const char STR_JET[] PROGMEM = "Jet Pump";
const char STR_HEATER[] PROGMEM = "Heater";
const char STR_OZONE[] PROGMEM = "Ozone Generator";
const char STR_SPEAKER[] PROGMEM = "Speaker";
const char STR_LIGHT[] PROGMEM = "Light";
const char STR_SETTINGS[] PROGMEM = "Settings";
const char STR_TARGET_TEMP[] PROGMEM = "Target Temp";
const char STR_IDLE_TIMEOUT[] PROGMEM = "Idle Timeout";
const char STR_ABOUT[] PROGMEM = "About";

// ============================================================================
// MAIN MENU DATA (PROGMEM)
// ============================================================================
const MenuItem mainMenuItems[] PROGMEM = {
    {STR_CIRCULATION, MENU_CIRCULATION_PUMP, ACTUATOR_CIRCULATION_PUMP},
    {STR_MASSAGE, MENU_MASSAGE_PUMP, ACTUATOR_MASSAGE_PUMP},
    {STR_JET, MENU_JET_PUMP, ACTUATOR_JET_PUMP},
    {STR_HEATER, MENU_HEATER, ACTUATOR_HEATER},
    {STR_OZONE, MENU_OZONE, ACTUATOR_OZONE},
    {STR_SPEAKER, MENU_SPEAKER, ACTUATOR_SPEAKER},
    {STR_LIGHT, MENU_LIGHT, ACTUATOR_LIGHT},
    {STR_SETTINGS, MENU_SETTINGS, -1}
};
const uint8_t mainMenuCount = 8;

// ============================================================================
// SETTINGS MENU DATA (PROGMEM)
// ============================================================================
const MenuItem settingsMenuItems[] PROGMEM = {
    {STR_TARGET_TEMP, MENU_SETTINGS_TEMP, -1},
    {STR_IDLE_TIMEOUT, MENU_SETTINGS_TIMEOUT, -1},
    {STR_ABOUT, MENU_SETTINGS_ABOUT, -1}
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
 * - From main menu: enter submenu or actuator control
 * - From submenu: return to parent menu
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
        
        // Navigate to submenu
        if (item.submenu != MENU_IDLE)
        {
            currentMenu = item.submenu;
            selectedIndex = 0;
            DEBUG_PRINT("Navigated to submenu: ");
            DEBUG_PRINTLN((int)item.submenu);
        }
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
    else
    {
        // Return to parent menu from any other menu
        // Determine parent menu based on current menu
        if (currentMenu >= MENU_CIRCULATION_PUMP && currentMenu <= MENU_LIGHT)
        {
            // Actuator menus return to main menu
            currentMenu = MENU_MAIN;
            selectedIndex = 0;
            DEBUG_PRINTLN("Returned to main menu from actuator control");
        }
        else if (currentMenu >= MENU_SETTINGS_TEMP && currentMenu <= MENU_SETTINGS_ABOUT)
        {
            // Settings submenus return to settings menu
            currentMenu = MENU_SETTINGS;
            selectedIndex = 0;
            DEBUG_PRINTLN("Returned to settings menu from submenu");
        }
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
 * @brief Get actuator ID for current menu selection
 * 
 * Returns the actuator ID if current menu is an actuator control menu,
 * otherwise returns -1.
 * 
 * @return Actuator ID (0-7) or -1 if not an actuator menu
 */
int8_t MenuManager::getSelectedActuatorId()
{
    // Check if current menu is an actuator control menu
    if (currentMenu >= MENU_CIRCULATION_PUMP && currentMenu <= MENU_LIGHT)
    {
        // Calculate actuator ID from menu ID
        // MENU_CIRCULATION_PUMP = 2, ACTUATOR_CIRCULATION_PUMP = 0
        // MENU_MASSAGE_PUMP = 3, ACTUATOR_MASSAGE_PUMP = 1
        // etc.
        return (int8_t)(currentMenu - MENU_CIRCULATION_PUMP);
    }
    
    return -1;  // Not an actuator menu
}
