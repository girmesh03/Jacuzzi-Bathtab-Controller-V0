// ============================================================================
// ACTUATORS.CPP - PCF8574 Relay Control Implementation
// ============================================================================

#include "Actuators.h"
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
// INITIALIZE PCF8574
// ============================================================================
bool ActuatorManager::init() {
    DEBUG_PRINT("Initializing PCF8574 at address 0x");
    DEBUG_PRINTLN(String(I2C_PCF8574_ADDR, HEX));
    
    // Initialize relay state to all OFF
    // NOTE: Relays are ACTIVE-LOW, so 0xFF = all relays OFF
    relayState = 0x00;  // Internal state: 0 = OFF, 1 = ON
    
    // Write initial state to PCF8574 (inverted for active-low)
    Wire.beginTransmission(I2C_PCF8574_ADDR);
    Wire.write(~relayState);  // Invert: 0xFF sent to PCF8574 = all relays OFF
    uint8_t result = Wire.endTransmission();
    
    if (result == 0) {
        DEBUG_PRINTLN("PCF8574 initialized successfully - all relays OFF (active-low)");
        return true;
    } else {
        DEBUG_PRINT("ERROR: PCF8574 initialization failed! I2C error code: ");
        DEBUG_PRINTLN(String(result));
        return false;
    }
}

// ============================================================================
// SET ACTUATOR STATE
// ============================================================================
bool ActuatorManager::setState(uint8_t actuatorId, bool state) {
    if (actuatorId > 7) {
        DEBUG_PRINTLN("ERROR: Invalid actuator ID");
        return false;
    }
    
    // Update relay state bit field
    if (state) {
        relayState |= (1 << actuatorId);  // Set bit (relay ON)
    } else {
        relayState &= ~(1 << actuatorId); // Clear bit (relay OFF)
    }
    
    // Write to PCF8574
    return writeToExpander();
}

// ============================================================================
// GET ACTUATOR STATE
// ============================================================================
bool ActuatorManager::getState(uint8_t actuatorId) {
    if (actuatorId > 7) {
        return false;
    }
    
    return (relayState & (1 << actuatorId)) != 0;
}

// ============================================================================
// GET ALL ACTUATOR STATES
// ============================================================================
uint8_t ActuatorManager::getAllStates() {
    return relayState;
}

// ============================================================================
// EMERGENCY SHUTDOWN
// ============================================================================
void ActuatorManager::emergencyShutdown() {
    DEBUG_PRINTLN("EMERGENCY SHUTDOWN - Deactivating all relays");
    
    // Set all relays OFF
    relayState = 0x00;
    
    // Write to PCF8574 (ignore return value in emergency)
    writeToExpander();
}

// ============================================================================
// WRITE TO PCF8574
// ============================================================================
bool ActuatorManager::writeToExpander() {
    Wire.beginTransmission(I2C_PCF8574_ADDR);
    
    // Invert relay state for active-low relays
    // Internal: 0 = OFF, 1 = ON
    // PCF8574: HIGH = relay OFF, LOW = relay ON
    Wire.write(~relayState);
    
    uint8_t result = Wire.endTransmission();
    
    if (result == 0) {
        DEBUG_PRINT("PCF8574 write successful - relay state: 0x");
        DEBUG_PRINTLN(String(relayState, HEX));
        DEBUG_PRINT("  (PCF8574 output: 0x");
        DEBUG_PRINT(String((uint8_t)~relayState, HEX));
        DEBUG_PRINTLN(" - active-low)");
        return true;
    } else {
        DEBUG_PRINT("ERROR: PCF8574 write failed! I2C error code: ");
        DEBUG_PRINTLN(String(result));
        return false;
    }
}
