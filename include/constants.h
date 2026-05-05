// ============================================================================
// CONSTANTS.H - Single Source of Truth for Jacuzzi Controller System
// ============================================================================
// This file contains ALL system constants including pin assignments, I2C
// addresses, timing values, temperature configuration, and menu strings.
//
// CRITICAL: This is the ONLY place where these constants should be defined.
// All adjustable parameters are defined here to allow easy configuration
// changes without modifying code logic.
//
// Target Platform: ESP8266 (ESP-12E/ESP-12F)
// ============================================================================

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    //   QT-PY / XIAO

// ============================================================================
// PIN ASSIGNMENTS (ESP8266-specific)
// ============================================================================
// I2C pins
#define PIN_SDA 4 // D2 (GPIO4)
#define PIN_SCL 5 // D1 (GPIO5)

// Sensor pins
#define PIN_TEMP_SENSOR 14 // D5 (GPIO14) - safe, no boot issues
#define PIN_WATER_LEVEL 2  // D4 (GPIO2) - has pull-up, safe for active-low sensor

// Rotary encoder pins
#define PIN_ENCODER_CLK 13 // D7 (GPIO13) - safe
#define PIN_ENCODER_DT 12  // D6 (GPIO12) - REQUIRES external 10kΩ pull-down!
#define PIN_ENCODER_SW 0   // D3 (GPIO0) - has pull-up, safe for button

// Output pins
#define PIN_BUZZER 15 // D8 (GPIO15) - safe, no boot issues

// ============================================================================
// I2C ADDRESSES
// ============================================================================
#define I2C_OLED_ADDR 0x3C
#define I2C_PCF8574_ADDR 0x20

// ============================================================================
// TIMING CONSTANTS (milliseconds)
// ============================================================================
#define TEMP_UPDATE_INTERVAL 2000    // Temperature reading interval
#define WATER_LEVEL_INTERVAL 500     // Water level check interval
#define DISPLAY_UPDATE_INTERVAL 500  // Display refresh interval for sensor data
#define DISPLAY_MIN_FRAME_TIME 100   // 10 FPS minimum
#define ENCODER_DEBOUNCE_TIME 50     // Encoder debounce delay
#define IDLE_TIMEOUT_DEFAULT 30000   // Default idle timeout (30 seconds)
#define BUZZER_BEEP_SHORT 50         // Short beep duration
#define BUZZER_BEEP_WARNING 200      // Warning beep duration
#define SPLASH_SCREEN_DURATION 2000  // Splash screen display duration
#define WATER_LEVEL_FLASH_INTERVAL 500 // Water level warning flash interval (1 Hz)

// ============================================================================
// TEMPERATURE CONFIGURATION (Dynamic - easy to adjust)
// ============================================================================
#define TEMP_MIN 30.0f             // Minimum settable temperature (°C)
#define TEMP_MAX 40.0f             // Maximum settable temperature (°C)
#define TEMP_INCREMENT 0.5f        // Temperature adjustment step (°C)
#define TEMP_THERMAL_RUNAWAY 45.0f // Thermal runaway threshold (°C)
#define TEMP_HYSTERESIS 0.5f       // Hysteresis for auto control (°C)

// ============================================================================
// IDLE TIMEOUT CONFIGURATION (Dynamic - easy to adjust)
// ============================================================================
#define IDLE_TIMEOUT_MIN 10  // Minimum timeout (seconds)
#define IDLE_TIMEOUT_MAX 120 // Maximum timeout (seconds)
#define IDLE_TIMEOUT_STEP 10 // Timeout adjustment step (seconds)

// ============================================================================
// SENSOR CONFIGURATION (Dynamic - allows polarity changes without code mods)
// ============================================================================
// Water level sensor polarity
// true = sensor is active-low (LOW when water present)
// false = sensor is active-high (HIGH when water present)
#define WATER_LEVEL_ACTIVE_LOW true

// Temperature sensor error codes
#define TEMP_SENSOR_ERROR_CODE_DISCONNECTED -127.0f // DS18B20 disconnected
#define TEMP_SENSOR_ERROR_CODE_NOT_READY 85.0f      // DS18B20 not ready
#define TEMP_SENSOR_FAIL_THRESHOLD 3                // Consecutive failures before marking invalid

// ============================================================================
// SAFETY TIMING (milliseconds)
// ============================================================================
#define HEATER_INTERLOCK_RESPONSE_MS 100 // Max heater interlock response time
#define WATER_LEVEL_INTERLOCK_MS 100     // Max water level interlock response
#define THERMAL_RUNAWAY_CHECK_MS 500     // Thermal runaway check interval

// ============================================================================
// ACTUATOR BIT POSITIONS
// ============================================================================
#define ACTUATOR_CIRCULATION_PUMP 0
#define ACTUATOR_MASSAGE_PUMP 1
#define ACTUATOR_JET_PUMP 2
#define ACTUATOR_HEATER 3
#define ACTUATOR_OZONE 4
#define ACTUATOR_SPEAKER 5
#define ACTUATOR_LIGHT 6
#define ACTUATOR_RESERVED 7

// Actuator configuration
#define ACTUATOR_COUNT 8              // Total number of actuators (PCF8574 has 8 outputs)
#define ACTUATOR_MAX_ID 7             // Maximum valid actuator ID (0-7)
#define ACTUATOR_ALL_OFF 0x00         // All actuators OFF state

// ============================================================================
// MENU STRINGS (PROGMEM)
// ============================================================================
const char STR_SPLASH_TITLE[] PROGMEM = "Jacuzzi Controller";
const char STR_READY[] PROGMEM = "READY";
const char STR_INITIALIZING[] PROGMEM = "INITIALIZING...";
const char STR_TEMP_ERROR[] PROGMEM = "TEMP ERROR";
const char STR_WATER_OK[] PROGMEM = "Water: OK";
const char STR_LOW_WATER[] PROGMEM = "LOW WATER";
const char STR_DEGREE_C[] PROGMEM = " C"; // Degree symbol not available in default font

// Additional strings will be added in later phases

// ============================================================================
// SERIAL COMMUNICATION
// ============================================================================
#define SERIAL_BAUD_RATE 115200      // Serial monitor baud rate
#define SERIAL_INIT_DELAY 100        // Delay after serial initialization (ms)

// ============================================================================
// DISPLAY LAYOUT CONSTANTS
// ============================================================================
// Text sizes
#define TEXT_SIZE_LARGE 2            // Large text (temperature, titles)
#define TEXT_SIZE_NORMAL 1           // Normal text (status, labels)

// Splash screen layout
#define SPLASH_TITLE_LINE1_X 10
#define SPLASH_TITLE_LINE1_Y 10
#define SPLASH_TITLE_LINE2_X 4
#define SPLASH_TITLE_LINE2_Y 30
#define SPLASH_VERSION_X 52
#define SPLASH_VERSION_Y 52

// Ready message layout
#define READY_MSG_X 32
#define READY_MSG_Y 24

// Sensor data display layout
#define TEMP_DISPLAY_X 0
#define TEMP_DISPLAY_Y 0
#define WATER_STATUS_X 0
#define WATER_STATUS_Y 30
#define COUNTER_DISPLAY_X 0
#define COUNTER_DISPLAY_Y 50

#endif // CONSTANTS_H
