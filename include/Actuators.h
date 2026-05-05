// ============================================================================
// ACTUATORS.H - PCF8574 Relay Control Module
// ============================================================================
// Manages all actuator control through PCF8574 I2C I/O expander.
// Controls 8 relays for pumps, heater, ozone, speaker, light.
//
// RELAY LOGIC: Active-Low
// - Internal state: 0 = OFF, 1 = ON (logical state)
// - PCF8574 output: HIGH = relay OFF, LOW = relay ON (inverted)
// - All relay state is automatically inverted when writing to PCF8574
//
// Target Platform: ESP8266 (ESP-12E/ESP-12F)
// I/O Expander: PCF8574 at I2C address 0x20
// ============================================================================

#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>
#include "Constants.h"

// Forward declaration
class SensorManager;

// ============================================================================
// ACTUATOR MANAGER CLASS
// ============================================================================
class ActuatorManager
{
public:
    // Initialize PCF8574 with all outputs OFF
    bool init();

    // Set actuator state with safety interlock checks (true = ON, false = OFF)
    bool setState(uint8_t actuatorId, bool state, SensorManager* sensors);

    // Get actuator state
    bool getState(uint8_t actuatorId);

    // Get all actuator states as bit field
    uint8_t getAllStates();

    // Emergency shutdown - deactivate all outputs
    void emergencyShutdown();

private:
    uint8_t relayState; // Current relay states (bit field)

    // Write relay state to PCF8574
    bool writeToExpander();
};

#endif // ACTUATORS_H
