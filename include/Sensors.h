#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

enum SensorStatus {
    SENSOR_OK,
    TEMP_SENSOR_ERROR,
    TEMP_SENSOR_DISCONNECTED,
    WATER_LEVEL_UNKNOWN
};

class SensorManager {
public:
    void init();
    void update();  // Non-blocking, call every loop
    
    float getTemperature();
    bool isTemperatureValid();
    bool isWaterLevelOK();
    SensorStatus getStatus();
    
private:
    float lastValidTemp;
    uint32_t lastTempUpdate;
    uint8_t waterLevelSamples[3];  // For majority voting
    uint8_t sampleIndex;
    uint8_t tempReadFailCount;
    bool tempValid;
    bool waterLevelOK;
};

#endif // SENSORS_H
