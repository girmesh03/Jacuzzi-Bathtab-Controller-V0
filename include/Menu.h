// ============================================================================
// MENU.H - Menu System Management Module
// ============================================================================
// Handles menu navigation, structure, and user interaction for the Jacuzzi
// Controller System. Implements hierarchical menu with idle timeout.
//
// Target Platform: ESP8266 (ESP-12E/ESP-12F)
// Phase 4: Basic Menu System
// ============================================================================

#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "Constants.h"

// ============================================================================
// MENU ID ENUMERATION
// ============================================================================
enum MenuId {
    MENU_IDLE,                  // Idle screen (default)
    MENU_MAIN,                  // Main menu (actuators + settings)
    MENU_SETTINGS,              // Settings menu
    MENU_SETTINGS_TEMP,         // Target temperature setting
    MENU_SETTINGS_TIMEOUT,      // Idle timeout setting
    MENU_SETTINGS_ABOUT         // About screen
    // NOTE: Actuator control screens removed - toggle directly from main menu
};

// ============================================================================
// MENU ITEM STRUCTURE
// ============================================================================
struct MenuItem {
    const char* label;              // Menu item label (stored in PROGMEM)
    const unsigned char* bitmap;    // Pointer to bitmap in PROGMEM
    MenuId submenu;                 // Submenu to open, or MENU_IDLE if none
    int8_t actuatorId;              // Actuator index (0-7), or -1 if not an actuator
};

// ============================================================================
// MENU MANAGER CLASS
// ============================================================================
class MenuManager {
public:
    void init();
    void update();  // Non-blocking, call every loop
    
    void handleRotation(bool clockwise);
    void handlePress();
    
    MenuId getCurrentMenu();
    uint8_t getSelectedIndex();
    bool isIdle();
    int8_t getSelectedActuatorId();
    
    // Reset idle timeout (called on actuator interaction)
    void resetIdleTimeout();
    
private:
    MenuId currentMenu;
    uint8_t selectedIndex;
    uint32_t lastInteractionTime;
    
    void returnToIdle();
    void navigateToSubmenu(MenuId submenu);
    uint8_t getMenuItemCount(MenuId menu);
};

// ============================================================================
// MENU DATA DECLARATIONS (defined in Menu.cpp)
// ============================================================================
// Main menu items
extern const MenuItem mainMenuItems[] PROGMEM;
extern const uint8_t mainMenuCount;

// Settings menu items
extern const MenuItem settingsMenuItems[] PROGMEM;
extern const uint8_t settingsMenuCount;

// Status strings
extern const char STR_ON[] PROGMEM;
extern const char STR_OFF[] PROGMEM;

#endif // MENU_H
