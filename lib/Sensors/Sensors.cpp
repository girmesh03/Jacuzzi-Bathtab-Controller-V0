// ============================================================================
// SENSORS.CPP - Sensor Manager Implementation
// ============================================================================
// Manages DS18B20 temperature sensor and water level sensor
// Non-blocking sensor reading with error detection and recovery
//
// Target Platform: ESP8266 (ESP-12E/ESP-12F)
// Phase 2: Sensor Integration
// ============================================================================

#include "Sensors.h"
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
// GLOBAL SENSOR OBJECTS
// ============================================================================
OneWire oneWire(PIN_TEMP_SENSOR);
DallasTemperature tempSensor(&oneWire);

// ============================================================================
// SENSOR MANAGER IMPLEMENTATION
// ============================================================================

/**
 * @brief Initialize sensors and variables
 * 
 * Initializes DS18B20 temperature sensor with 12-bit resolution
 * and water level sensor variables. Sets all state to initial values.
 */
void SensorManager::init()
{
    DEBUG_PRINTLN("Initializing sensors...");
    
    // Initialize DS18B20 temperature sensor
    tempSensor.begin();
    tempSensor.setResolution(12);  // 12-bit resolution (0.0625°C precision)
    
    DEBUG_PRINTLN("DS18B20 temperature sensor initialized (12-bit resolution)");
    
    // Initialize temperature variables
    lastValidTemp = 0.0f;
    tempValid = false;
    tempReadFailCount = 0;
    lastTempUpdate = 0;
    
    // Initialize water level variables
    waterLevelSamples[0] = 0;
    waterLevelSamples[1] = 0;
    waterLevelSamples[2] = 0;
    sampleIndex = 0;
    waterLevelOK = false;
    
    // Configure water level pin with internal pull-up
    pinMode(PIN_WATER_LEVEL, INPUT_PULLUP);
    
    DEBUG_PRINTLN("Sensors initialized");
}

/**
 * @brief Non-blocking sensor update (call every loop iteration)
 * 
 * Reads temperature sensor every TEMP_UPDATE_INTERVAL ms
 * Reads water level sensor every WATER_LEVEL_INTERVAL ms
 * Implements error detection and majority voting for reliability
 */
void SensorManager::update()
{
    // ========================================================================
    // TEMPERATURE SENSOR UPDATE
    // ========================================================================
    static uint32_t lastTempRead = 0;
    
    if (millis() - lastTempRead >= TEMP_UPDATE_INTERVAL)
    {
        // Request temperature reading (non-blocking)
        tempSensor.requestTemperatures();
        
        // Read temperature from first sensor on bus
        float temp = tempSensor.getTempCByIndex(0);
        
        // Check for error codes
        // TEMP_SENSOR_ERROR_CODE_DISCONNECTED = sensor disconnected or CRC error
        // TEMP_SENSOR_ERROR_CODE_NOT_READY = sensor not ready or power-on reset value
        if (temp == TEMP_SENSOR_ERROR_CODE_DISCONNECTED || temp == TEMP_SENSOR_ERROR_CODE_NOT_READY)
        {
            // Invalid temperature reading
            tempReadFailCount++;
            
            DEBUG_PRINT("Temperature read failed (error code: ");
            DEBUG_PRINT(temp);
            DEBUG_PRINT("°C), fail count: ");
            DEBUG_PRINTLN(tempReadFailCount);
            
            // After TEMP_SENSOR_FAIL_THRESHOLD consecutive failures, mark sensor as invalid
            if (tempReadFailCount >= TEMP_SENSOR_FAIL_THRESHOLD)
            {
                tempValid = false;
                DEBUG_PRINT("Temperature sensor marked as INVALID (");
                DEBUG_PRINT(TEMP_SENSOR_FAIL_THRESHOLD);
                DEBUG_PRINTLN(" consecutive failures)");
            }
        }
        else
        {
            // Valid temperature reading
            lastValidTemp = temp;
            tempValid = true;
            tempReadFailCount = 0;
            lastTempUpdate = millis();
            
            DEBUG_PRINT("Temperature: ");
            DEBUG_PRINT(temp);
            DEBUG_PRINTLN("°C");
        }
        
        lastTempRead = millis();
    }
    
    // ========================================================================
    // WATER LEVEL SENSOR UPDATE
    // ========================================================================
    static uint32_t lastWaterLevelRead = 0;
    
    if (millis() - lastWaterLevelRead >= WATER_LEVEL_INTERVAL)
    {
        // Read pin state
        bool pinState = digitalRead(PIN_WATER_LEVEL);
        
        // Apply polarity configuration
        // WATER_LEVEL_ACTIVE_LOW = true: LOW = water present
        // WATER_LEVEL_ACTIVE_LOW = false: HIGH = water present
        bool waterPresent = WATER_LEVEL_ACTIVE_LOW ? !pinState : pinState;
        
        // Store in samples array for majority voting
        waterLevelSamples[sampleIndex] = waterPresent ? 1 : 0;
        
        // Increment sample index (wrap at 3)
        sampleIndex++;
        if (sampleIndex >= 3)
        {
            sampleIndex = 0;
        }
        
        // Calculate majority vote (sum samples, if >= 2 then water OK)
        uint8_t sum = waterLevelSamples[0] + waterLevelSamples[1] + waterLevelSamples[2];
        waterLevelOK = (sum >= 2);
        
        DEBUG_PRINT("Water level: pin=");
        DEBUG_PRINT(pinState ? "HIGH" : "LOW");
        DEBUG_PRINT(", present=");
        DEBUG_PRINT(waterPresent ? "YES" : "NO");
        DEBUG_PRINT(", samples=[");
        DEBUG_PRINT(waterLevelSamples[0]);
        DEBUG_PRINT(",");
        DEBUG_PRINT(waterLevelSamples[1]);
        DEBUG_PRINT(",");
        DEBUG_PRINT(waterLevelSamples[2]);
        DEBUG_PRINT("], majority=");
        DEBUG_PRINTLN(waterLevelOK ? "OK" : "LOW");
        
        lastWaterLevelRead = millis();
    }
}

/**
 * @brief Get last valid temperature reading
 * @return Temperature in °C (0.0 if never read)
 */
float SensorManager::getTemperature()
{
    return lastValidTemp;
}

/**
 * @brief Check if temperature reading is valid
 * @return true if temperature sensor is working, false if failed
 */
bool SensorManager::isTemperatureValid()
{
    return tempValid;
}

/**
 * @brief Check if water level is OK
 * @return true if water present (majority vote), false if low water
 */
bool SensorManager::isWaterLevelOK()
{
    return waterLevelOK;
}

/**
 * @brief Get overall sensor status
 * @return SensorStatus enum indicating current state
 */
SensorStatus SensorManager::getStatus()
{
    // Check for temperature sensor errors first
    if (!tempValid && tempReadFailCount >= TEMP_SENSOR_FAIL_THRESHOLD)
    {
        return TEMP_SENSOR_DISCONNECTED;
    }
    else if (!tempValid)
    {
        return TEMP_SENSOR_ERROR;
    }
    
    // Check for water level issues
    if (!waterLevelOK)
    {
        return WATER_LEVEL_UNKNOWN;
    }
    
    // All sensors OK
    return SENSOR_OK;
}
