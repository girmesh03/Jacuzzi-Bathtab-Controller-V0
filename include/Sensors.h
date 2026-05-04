// ============================================================================
// SENSORS.H - Sensor Management Interface
// ============================================================================
// This module manages temperature and water level sensors with non-blocking
// updates, error detection, and majority voting for water level stability.
//
// Temperature Sensor: DS18B20 One-Wire on GPIO14 (D5)
// Water Level Sensor: Digital input on GPIO2 (D4) with pull-up
//
// Target Platform: ESP8266 (ESP-12E/ESP-12F)
// ============================================================================

#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ============================================================================
// SENSOR STATUS ENUMERATION
// ============================================================================
enum SensorStatus
{
    SENSOR_OK,                  // All sensors operating normally
    TEMP_SENSOR_ERROR,          // Temperature sensor read error (transient)
    TEMP_SENSOR_DISCONNECTED,   // Temperature sensor disconnected (persistent)
    WATER_LEVEL_UNKNOWN         // Water level sensor state unknown
};

// ============================================================================
// SENSOR MANAGER CLASS
// ============================================================================
class SensorManager
{
public:
    // ========================================================================
    // PUBLIC METHODS
    // ========================================================================
    
    /**
     * @brief Initialize all sensors
     * 
     * Initializes DS18B20 temperature sensor and water level sensor.
     * Sets up OneWire communication and configures GPIO pins.
     */
    void init();
    
    /**
     * @brief Update sensor readings (non-blocking)
     * 
     * Call this method every loop iteration. It performs non-blocking
     * sensor updates based on configured intervals:
     * - Temperature: every TEMP_UPDATE_INTERVAL ms (2000ms)
     * - Water level: every WATER_LEVEL_INTERVAL ms (500ms)
     */
    void update();
    
    /**
     * @brief Get current temperature reading
     * 
     * @return float Temperature in degrees Celsius (0.1°C precision)
     *               Returns last valid reading if sensor error occurred
     */
    float getTemperature();
    
    /**
     * @brief Check if temperature reading is valid
     * 
     * @return true if temperature sensor is working and reading is valid
     * @return false if sensor error or disconnection detected
     */
    bool isTemperatureValid();
    
    /**
     * @brief Check if water level is OK
     * 
     * Uses 3-sample majority voting to filter noise and prevent false triggers.
     * 
     * @return true if water level is sufficient (sensor indicates water present)
     * @return false if water level is low (sensor indicates no water)
     */
    bool isWaterLevelOK();
    
    /**
     * @brief Get overall sensor status
     * 
     * @return SensorStatus Current status of sensor system
     */
    SensorStatus getStatus();

private:
    // ========================================================================
    // PRIVATE MEMBER VARIABLES
    // ========================================================================
    
    // Temperature sensor state
    float lastValidTemp;        // Last valid temperature reading (°C)
    uint32_t lastTempUpdate;    // Timestamp of last temperature update (ms)
    uint8_t tempReadFailCount;  // Consecutive temperature read failures
    bool tempValid;             // Temperature reading validity flag
    
    // Water level sensor state
    uint8_t waterLevelSamples[3]; // 3-sample buffer for majority voting
    uint8_t sampleIndex;          // Current sample index (0-2)
    bool waterLevelOK;            // Water level status (true = OK, false = low)
};

#endif // SENSORS_H
