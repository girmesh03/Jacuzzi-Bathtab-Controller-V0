// ============================================================================
// MAIN.CPP - Jacuzzi/Bathtub Controller System
// ============================================================================
// ESP8266-based embedded control system for jacuzzi/bathtub
// Target Platform: ESP8266 (ESP-12E/ESP-12F)
//
// Phase 1: Basic Hardware Initialization
// - I2C communication with OLED display and PCF8574
// - Splash screen and READY message display
// - All relays initialized to OFF state
//
// Phase 2: Sensor Integration
// - DS18B20 temperature sensor reading
// - Water level sensor monitoring
// - Sensor data display on OLED
//
// Phase 3: Rotary Encoder Input
// - KY-040 rotary encoder input with debouncing
// - Button press detection
// - Buzzer feedback for user interactions
// - Test counter display
// ============================================================================

#include <Arduino.h>
#include <Wire.h>
#include "Constants.h"
#include "Display.h"
#include "Actuators.h"
#include "Sensors.h"
#include "Input.h"
#include "Menu.h"

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
// GLOBAL OBJECTS
// ============================================================================
DisplayManager displayManager;
ActuatorManager actuatorManager;
SensorManager sensors;
InputManager input;
MenuManager menu;

// ============================================================================
// SETUP FUNCTION
// ============================================================================
void setup()
{
  // Initialize Serial for debug output
  Serial.begin(SERIAL_BAUD_RATE);
  delay(SERIAL_INIT_DELAY); // Allow serial to stabilize

  DEBUG_PRINTLN("Jacuzzi Controller Starting...");
  DEBUG_PRINTLN("Phase 1: Basic Hardware Initialization");

  // Initialize I2C with explicit ESP8266 pins
  Wire.begin(PIN_SDA, PIN_SCL);
  DEBUG_PRINTLN("I2C initialized on SDA=GPIO4 (D2), SCL=GPIO5 (D1)");

  // Initialize OLED display
  if (!displayManager.init())
  {
    DEBUG_PRINTLN("ERROR: OLED display initialization failed!");
    DEBUG_PRINTLN("System halted. Check I2C connections.");
    // Enter infinite loop on failure
    while (1)
    {
      yield(); // Allow ESP8266 background tasks
    }
  }

  // Initialize PCF8574 I/O expander (all relays OFF)
  if (!actuatorManager.init())
  {
    DEBUG_PRINTLN("ERROR: PCF8574 initialization failed!");
    DEBUG_PRINTLN("System halted. Check I2C connections and address.");

    // Add error to queue and display
    displayManager.addError("PCF8574 ERROR");
    
    // Display error screen
    displayManager.update(&sensors, &menu, &actuatorManager);

    // Enter infinite loop on failure
    while (1)
    {
      yield(); // Allow ESP8266 background tasks
    }
  }

  // Initialize sensors (DS18B20 and water level)
  sensors.init();
  DEBUG_PRINTLN("Sensors initialized");

  // Initialize input manager (rotary encoder and buzzer)
  input.init();
  DEBUG_PRINTLN("Input initialized");

  // Initialize menu manager (Phase 4)
  menu.init();
  DEBUG_PRINTLN("Menu initialized");

  // Display splash screen for SPLASH_SCREEN_DURATION
  displayManager.showSplashScreen();

  DEBUG_PRINTLN("Phase 1 initialization complete");
  DEBUG_PRINTLN("Phase 2: Sensor integration active");
  DEBUG_PRINTLN("Phase 3: Rotary encoder input active");
  DEBUG_PRINTLN("Phase 4: Basic menu system active");
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop()
{
  // ========================================================================
  // UPDATE SENSORS FIRST (non-blocking)
  // ========================================================================
  sensors.update();
  
  // ========================================================================
  // ERROR QUEUE MANAGEMENT (HIGHEST PRIORITY)
  // ========================================================================
  // Check safety conditions and update error queue
  // Track previous states to avoid infinite logging
  
  static bool lastWaterLevelOK = true;
  static bool lastTempValid = true;
  
  // Water level error (CRITICAL - safety issue)
  bool currentWaterLevelOK = sensors.isWaterLevelOK();
  if (!currentWaterLevelOK && lastWaterLevelOK)
  {
    // Water level just became low
    displayManager.addError("LOW WATER", true);  // Critical
    DEBUG_PRINTLN("ERROR: Water level low - added to queue");
  }
  else if (currentWaterLevelOK && !lastWaterLevelOK)
  {
    // Water level just became OK
    displayManager.removeError("LOW WATER");
    DEBUG_PRINTLN("Water level OK - error removed from queue");
  }
  lastWaterLevelOK = currentWaterLevelOK;
  
  // Temperature sensor error (CRITICAL - safety issue)
  bool currentTempValid = sensors.isTemperatureValid();
  if (!currentTempValid && lastTempValid)
  {
    // Temperature sensor just failed
    displayManager.addError("TEMP SENSOR", true);  // Critical
    DEBUG_PRINTLN("ERROR: Temperature sensor failed - added to queue");
  }
  else if (currentTempValid && !lastTempValid)
  {
    // Temperature sensor just recovered
    displayManager.removeError("TEMP SENSOR");
    DEBUG_PRINTLN("Temperature sensor OK - error removed from queue");
  }
  lastTempValid = currentTempValid;
  
  // ========================================================================
  // ERROR STATE CHECK (BLOCKS OPERATIONS IF CRITICAL ERRORS EXIST)
  // ========================================================================
  static bool shutdownLogged = false;
  
  if (displayManager.hasCriticalErrors())
  {
    // Critical errors exist - emergency shutdown all outputs
    if (!shutdownLogged)
    {
      actuatorManager.emergencyShutdown();
      DEBUG_PRINTLN("CRITICAL ERROR: Emergency shutdown activated");
      shutdownLogged = true;
    }
    
    // Update display to show error
    displayManager.update(&sensors, &menu, &actuatorManager);
    
    // Process encoder events for error navigation only
    input.update();
    if (input.hasEvent())
    {
      EncoderEvent event = input.getEvent();
      if (event == ENCODER_CW)
      {
        displayManager.nextError();
      }
      else if (event == ENCODER_CCW)
      {
        displayManager.prevError();
      }
      // Button press does nothing for critical errors (cannot dismiss)
    }
    
    yield();
    return;  // Block all other operations
  }
  else
  {
    // Reset shutdown flag when no critical errors
    shutdownLogged = false;
  }
  
  // ========================================================================
  // NON-CRITICAL ERROR HANDLING (dismissible errors)
  // ========================================================================
  if (displayManager.hasErrors())
  {
    // Non-critical errors exist - show error but allow dismissal
    displayManager.update(&sensors, &menu, &actuatorManager);
    
    // Process encoder events for error navigation and dismissal
    input.update();
    if (input.hasEvent())
    {
      EncoderEvent event = input.getEvent();
      if (event == ENCODER_CW)
      {
        displayManager.nextError();
      }
      else if (event == ENCODER_CCW)
      {
        displayManager.prevError();
      }
      else if (event == ENCODER_BUTTON)
      {
        // Dismiss current error if not critical
        displayManager.dismissCurrentError();
      }
    }
    
    yield();
    return;  // Block normal operations while errors displayed
  }
  
  // ========================================================================
  // SAFETY INTERLOCK MONITORING (CRITICAL - Check every loop iteration)
  // ========================================================================
  
  // Water level interlock - immediately deactivate pumps and heater if water low
  if (!sensors.isWaterLevelOK())
  {
    if (actuatorManager.getState(ACTUATOR_CIRCULATION_PUMP))
    {
      actuatorManager.setState(ACTUATOR_CIRCULATION_PUMP, false, &sensors);
      DEBUG_PRINTLN("INTERLOCK: Water low, deactivated circulation pump");
    }
    if (actuatorManager.getState(ACTUATOR_MASSAGE_PUMP))
    {
      actuatorManager.setState(ACTUATOR_MASSAGE_PUMP, false, &sensors);
      DEBUG_PRINTLN("INTERLOCK: Water low, deactivated massage pump");
    }
    if (actuatorManager.getState(ACTUATOR_JET_PUMP))
    {
      actuatorManager.setState(ACTUATOR_JET_PUMP, false, &sensors);
      DEBUG_PRINTLN("INTERLOCK: Water low, deactivated jet pump");
    }
    if (actuatorManager.getState(ACTUATOR_HEATER))
    {
      actuatorManager.setState(ACTUATOR_HEATER, false, &sensors);
      DEBUG_PRINTLN("INTERLOCK: Water low, deactivated heater");
    }
  }
  
  // Heater interlock - requires circulation pump running
  if (actuatorManager.getState(ACTUATOR_HEATER) &&
      !actuatorManager.getState(ACTUATOR_CIRCULATION_PUMP))
  {
    actuatorManager.setState(ACTUATOR_HEATER, false, &sensors);
    DEBUG_PRINTLN("INTERLOCK: Circulation pump off, deactivated heater");
  }
  
  // Requirement 12.6: When circulation pump turns OFF, turn OFF all other outputs
  static bool lastCirculationState = false;
  bool currentCirculationState = actuatorManager.getState(ACTUATOR_CIRCULATION_PUMP);
  
  if (lastCirculationState && !currentCirculationState)
  {
    // Circulation pump just turned OFF - turn OFF all outputs
    DEBUG_PRINTLN("Circulation pump turned OFF - deactivating all outputs");
    actuatorManager.emergencyShutdown();
  }
  
  lastCirculationState = currentCirculationState;
  
  // ========================================================================
  // UPDATE INPUT MANAGER (non-blocking)
  // ========================================================================
  input.update();
  
  // ========================================================================
  // UPDATE MENU MANAGER (idle timeout check)
  // ========================================================================
  menu.update();
  
  // ========================================================================
  // PROCESS ENCODER EVENTS
  // ========================================================================
  if (input.hasEvent())
  {
    EncoderEvent event = input.getEvent();
    
    switch (event)
    {
      case ENCODER_CW:
        // Clockwise rotation - navigate menu
        menu.handleRotation(true);
        DEBUG_PRINTLN("Encoder: CW - menu navigation");
        break;
        
      case ENCODER_CCW:
        // Counter-clockwise rotation - navigate menu
        menu.handleRotation(false);
        DEBUG_PRINTLN("Encoder: CCW - menu navigation");
        break;
        
      case ENCODER_BUTTON:
        // Button press - check if in main menu with actuator selected
        {
          int8_t actuatorId = menu.getSelectedActuatorId();
          
          if (actuatorId >= 0)  // In MENU_MAIN with actuator selected
          {
            // Toggle actuator state
            bool currentState = actuatorManager.getState(actuatorId);
            bool newState = !currentState;
            
            DEBUG_PRINT("Attempting to toggle actuator ");
            DEBUG_PRINT(actuatorId);
            DEBUG_PRINT(" to ");
            DEBUG_PRINTLN(newState ? "ON" : "OFF");
            
            // Check circulation pump prerequisite for certain actuators
            if (newState && !actuatorManager.getState(ACTUATOR_CIRCULATION_PUMP))
            {
              // Trying to turn ON actuator that requires circulation pump
              if (actuatorId == ACTUATOR_MASSAGE_PUMP ||
                  actuatorId == ACTUATOR_JET_PUMP ||
                  actuatorId == ACTUATOR_HEATER ||
                  actuatorId == ACTUATOR_OZONE)
              {
                // Add non-critical error to queue (dismissible)
                displayManager.addError("START PUMP FIRST", false);  // Not critical
                DEBUG_PRINTLN("Circulation pump prerequisite not met - error added");
                break;  // Don't toggle
              }
            }
            
            // Attempt to set state (safety checks enforced in setState)
            bool success = actuatorManager.setState(actuatorId, newState, &sensors);
            
            if (!success)
            {
              // Interlock prevented activation
              DEBUG_PRINTLN("Actuator toggle blocked by safety interlock");
            }
            else
            {
              DEBUG_PRINTLN("Actuator toggle successful");
            }
            
            // Reset idle timeout on actuator interaction
            menu.resetIdleTimeout();
            
            // Stay in main menu (don't navigate away)
          }
          else
          {
            // Navigate menus (Settings or other navigation)
            menu.handlePress();
            DEBUG_PRINTLN("Encoder: Button - menu navigation");
          }
        }
        break;
        
      case ENCODER_NONE:
      default:
        // No action
        break;
    }
  }
  
  // ========================================================================
  // UPDATE DISPLAY based on current menu state
  // ========================================================================
  displayManager.update(&sensors, &menu, &actuatorManager);
  
  // ========================================================================
  // ALLOW ESP8266 BACKGROUND TASKS
  // ========================================================================
  yield();
}
