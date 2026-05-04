# Implementation Plan: Jacuzzi/Bathtub Controller System

## Overview

This implementation plan breaks down the jacuzzi controller firmware into 10 incremental phases. Each phase delivers a working, testable main.cpp that builds upon the previous phase. The phased approach ensures continuous progress while maintaining code quality and safety on ESP8266 hardware.

**Target Platform**: ESP8266 (ESP-12E/ESP-12F) with 4 MB Flash and 80 KB SRAM

**Developer Role**: The AI agent is the developer responsible for implementing this project using the specifications in `.kiro/specs/jacuzzi-bathtub-controller/` directory (requirements.md, design.md, and this tasks.md file).

## Key Implementation Principles

- **One Branch Per Phase**: Each phase uses a single Git branch (e.g., `phase-1-hardware-init`). All tasks and sub-tasks within a phase are committed to the same branch.
- **Manual Hardware Testing Only**: No automated tests. User tests on actual ESP8266 hardware after each phase.
- **AI Agent Runs Git Commands**: AI agent executes all Git operations (add, commit, push, merge) directly.
- **User Runs PlatformIO Commands**: AI agent MUST request user to run all `pio` commands and wait for output.
- **6-Step Protocol**: Every phase follows the mandatory 6-step execution protocol.
- **Dynamic Configuration**: All adjustable parameters defined in constants.h for easy modification.
- **Sensor Polarity Independence**: Code logic independent of sensor active-high/active-low configuration.

## 6-Step Task Execution Protocol

**MANDATORY**: The AI agent (developer) MUST follow this protocol for EVERY phase:

### Step 1: Pre-Git Setup
**AI Agent Actions**:
- Verify current branch: `git branch`
- Pull latest changes: `git pull origin main`
- Verify clean state: `git status`
- Create phase branch: `git checkout -b phase-<N>-<description>`
- Confirm branch creation: `git branch`

### Step 2: Deep Codebase Analysis
**AI Agent Actions**:
- Read ALL existing source files in `src/` and `include/` directories
- Read `platformio.ini` configuration
- Read `include/constants.h` for current pin mappings and constants
- Understand current system state, dependencies, and architecture
- Identify integration points for current phase

### Step 3: Analysis of Previous Phase
**AI Agent Actions**:
- Read `docs/phase-<N-1>-<description>.md` if it exists
- Understand what was implemented in previous phase
- Identify dependencies and integration requirements
- Review any issues or notes from previous phase

### Step 4: Phase Execution
**AI Agent Actions**:
- Implement ALL tasks and sub-tasks exactly as specified in this tasks.md
- Follow ALL requirements from requirements.md
- Follow ALL design patterns from design.md
- NO deviation from specifications
- **Request user to run PlatformIO commands**:
  - Compilation: Output "**Please run: `pio run` and provide the output**"
  - Wait for user's compilation output
  - Upload: Output "**Please run: `pio run --target upload` and provide the output**"
  - Wait for user's upload output
  - Memory check: Output "**Please run: `pio run --target size` and provide the output**"
  - Wait for user's memory usage output
- **AI agent executes Git commands directly**:
  - Stage changes: `git add .`
  - Commit incrementally: `git commit -m "Implement <feature>"`
  - Continue until phase is complete

### Step 5: User Hardware Review and Feedback
**AI Agent Actions**:
- Output: "**Please test the following on hardware and provide feedback:**"
- List specific test cases for current phase (detailed in each phase below)
- Wait for user's test results and feedback
- **If issues reported**:
  - Analyze the issue
  - Fix the code
  - Request: `pio run` and `pio run --target upload`
  - Return to testing (iterate until working)
- **If user confirms working**:
  - Proceed to Step 6

### Step 6: Git Commit, Push, Merge, Cleanup
**AI Agent Actions**:
- Final commit: `git add .` and `git commit -m "Phase <N>: <description> - Complete"`
- Push branch: `git push origin phase-<N>-<description>`
- Merge to main: `git checkout main` and `git merge phase-<N>-<description>`
- Push main: `git push origin main`
- Delete phase branch: `git branch -d phase-<N>-<description>`
- Document phase completion in `docs/phase-<N>-<description>.md`
- Prepare for next phase

## Memory Budget Tracking

- Phase 1: ~50 KB Flash, ~10 KB SRAM
- Phase 2: ~70 KB Flash, ~12 KB SRAM
- Phase 3: ~80 KB Flash, ~13 KB SRAM
- Phase 4: ~100 KB Flash, ~15 KB SRAM
- Phase 5: ~110 KB Flash, ~16 KB SRAM
- Phase 6: ~130 KB Flash, ~18 KB SRAM
- Phase 7: ~140 KB Flash, ~19 KB SRAM
- Phase 8: ~150 KB Flash, ~20 KB SRAM
- Phase 9: ~160 KB Flash, ~21 KB SRAM
- Phase 10: ~160 KB Flash, ~21 KB SRAM (final)

## Tasks

### Phase 1: Basic Hardware Initialization

**Branch Name**: `phase-1-hardware-init`

**Objective**: Establish I2C communication with ESP8266-specific pins and verify display functionality

**AI Agent (Developer) Responsibilities**: Implement all tasks below following the 6-step protocol

**Step 1: Pre-Git Setup**
- AI agent executes: `git branch` (verify current branch)
- AI agent executes: `git pull origin main` (get latest changes)
- AI agent executes: `git status` (verify clean state)
- AI agent executes: `git checkout -b phase-1-hardware-init` (create phase branch)

**Step 2-3: Codebase Analysis**
- AI agent reads existing `platformio.ini` (if exists)
- AI agent reads existing `src/main.cpp` (if exists)
- This is Phase 1, so no previous phase to analyze

**Step 4: Phase Execution**

- [x] 1. Create include/constants.h with ESP8266 pin assignments and all system constants
  
  **AI Agent Must**:
  - Create file `include/constants.h`
  - Define ESP8266-specific pin assignments:
    - `#define PIN_SDA 4` (D2 / GPIO4)
    - `#define PIN_SCL 5` (D1 / GPIO5)
    - `#define PIN_TEMP_SENSOR 14` (D5 / GPIO14)
    - `#define PIN_WATER_LEVEL 2` (D4 / GPIO2)
    - `#define PIN_ENCODER_CLK 13` (D7 / GPIO13)
    - `#define PIN_ENCODER_DT 15` (D8 / GPIO15)
    - `#define PIN_ENCODER_SW 0` (D3 / GPIO0)
    - `#define PIN_BUZZER 12` (D6 / GPIO12)
  - Define I2C addresses:
    - `#define I2C_OLED_ADDR 0x3C`
    - `#define I2C_PCF8574_ADDR 0x20`
  - Define timing constants (all in milliseconds):
    - `#define TEMP_UPDATE_INTERVAL 2000`
    - `#define WATER_LEVEL_INTERVAL 500`
    - `#define DISPLAY_MIN_FRAME_TIME 100`
    - `#define ENCODER_DEBOUNCE_TIME 50`
    - `#define IDLE_TIMEOUT_DEFAULT 30000`
    - `#define BUZZER_BEEP_SHORT 50`
    - `#define BUZZER_BEEP_WARNING 200`
  - Define temperature configuration (dynamic):
    - `#define TEMP_MIN 30.0f`
    - `#define TEMP_MAX 40.0f`
    - `#define TEMP_INCREMENT 0.5f`
    - `#define TEMP_THERMAL_RUNAWAY 45.0f`
    - `#define TEMP_HYSTERESIS 0.5f`
  - Define idle timeout configuration (dynamic):
    - `#define IDLE_TIMEOUT_MIN 10`
    - `#define IDLE_TIMEOUT_MAX 120`
    - `#define IDLE_TIMEOUT_STEP 10`
  - Define sensor configuration:
    - `#define WATER_LEVEL_ACTIVE_LOW true` (allows polarity change without code modification)
  - Define safety timing:
    - `#define HEATER_INTERLOCK_RESPONSE_MS 100`
    - `#define WATER_LEVEL_INTERLOCK_MS 100`
    - `#define THERMAL_RUNAWAY_CHECK_MS 500`
  - Define actuator bit positions:
    - `#define ACTUATOR_CIRCULATION_PUMP 0`
    - `#define ACTUATOR_MASSAGE_PUMP 1`
    - `#define ACTUATOR_JET_PUMP 2`
    - `#define ACTUATOR_HEATER 3`
    - `#define ACTUATOR_OZONE 4`
    - `#define ACTUATOR_SPEAKER 5`
    - `#define ACTUATOR_LIGHT 6`
    - `#define ACTUATOR_RESERVED 7`
  - Define menu strings in PROGMEM:
    - `const char STR_SPLASH_TITLE[] PROGMEM = "Jacuzzi Controller";`
    - `const char STR_READY[] PROGMEM = "READY";`
    - `const char STR_INITIALIZING[] PROGMEM = "INITIALIZING...";`
    - (More strings will be added in later phases)
  - Use include guards: `#ifndef CONSTANTS_H` / `#define CONSTANTS_H` / `#endif`
  - Add comment header explaining this is the single source of truth
  - _Requirements: 1.1-1.6, 2.1-2.6, 19.1-19.13, 22.1-22.9, 41.1-41.10_
  - AI agent executes: `git add include/constants.h`
  - AI agent executes: `git commit -m "Create constants.h with ESP8266 pin assignments and dynamic configuration"`

- [x] 2. Configure platformio.ini for ESP8266
  
  **AI Agent Must**:
  - Create or update `platformio.ini` file
  - Set platform: `platform = espressif8266`
  - Set board: `board = esp12e`
  - Set framework: `framework = arduino`
  - Add library dependencies:
    ```ini
    lib_deps =
        adafruit/Adafruit SH110X@^2.1.14
        adafruit/Adafruit GFX Library@^1.11.0
        adafruit/Adafruit BusIO@^1.14.0
        paulstoffregen/OneWire@^2.3.7
        milesburton/DallasTemperature@^3.11.0
    ```
  - Add build flags:
    ```ini
    build_flags =
        -Os
        -DENABLE_SERIAL_DEBUG
    ```
  - Set monitor speed: `monitor_speed = 115200`
  - Set upload speed: `upload_speed = 921600`
  - _Requirements: 22.1-22.9_
  - AI agent executes: `git add platformio.ini`
  - AI agent executes: `git commit -m "Configure platformio.ini for ESP8266 esp12e board"`

- [x] 3. Initialize I2C bus and OLED display in src/main.cpp
  
  **AI Agent Must**:
  - Create `src/main.cpp` file
  - Include necessary headers:
    - `#include <Arduino.h>`
    - `#include <Wire.h>`
    - `#include <Adafruit_SH110X.h>`
    - `#include "constants.h"`
  - Define debug macros:
    ```cpp
    #ifdef ENABLE_SERIAL_DEBUG
      #define DEBUG_PRINT(x) Serial.print(x)
      #define DEBUG_PRINTLN(x) Serial.println(x)
    #else
      #define DEBUG_PRINT(x)
      #define DEBUG_PRINTLN(x)
    #endif
    ```
  - Create OLED display object: `Adafruit_SH1106G display(128, 64, &Wire, -1);`
  - In `setup()` function:
    - Initialize Serial: `Serial.begin(115200);`
    - Add delay for serial: `delay(100);`
    - Print debug message: `DEBUG_PRINTLN("Jacuzzi Controller Starting...");`
    - Initialize I2C with explicit pins: `Wire.begin(PIN_SDA, PIN_SCL);`
    - Initialize OLED display: `display.begin(I2C_OLED_ADDR, true);`
    - Check if display initialization succeeded
    - If failed: print error and enter infinite loop
    - Clear display: `display.clearDisplay();`
    - Set text size and color: `display.setTextSize(1);` and `display.setTextColor(SH110X_WHITE);`
  - _Requirements: 2.1-2.6, 11.1, 20.1-20.5_
  - AI agent executes: `git add src/main.cpp`
  - AI agent executes: `git commit -m "Initialize I2C and OLED display with ESP8266 pins"`

- [x] 4. Initialize PCF8574 with all outputs OFF
  
  **AI Agent Must**:
  - In `src/main.cpp`, add PCF8574 initialization
  - Include Wire library (already included)
  - Create function `bool initPCF8574()`:
    - Begin I2C transmission to PCF8574: `Wire.beginTransmission(I2C_PCF8574_ADDR);`
    - Write 0x00 to set all outputs LOW (relays OFF): `Wire.write(0x00);`
    - End transmission and check result: `uint8_t result = Wire.endTransmission();`
    - Return true if successful (result == 0), false otherwise
    - Add debug messages for success/failure
  - Call `initPCF8574()` in `setup()` after OLED initialization
  - If PCF8574 initialization fails: print error and enter infinite loop
  - _Requirements: 3.1-3.8, 9.1-9.2_
  - AI agent executes: `git add src/main.cpp`
  - AI agent executes: `git commit -m "Initialize PCF8574 with all relays OFF"`

- [x] 5. Display splash screen on startup
  
  **AI Agent Must**:
  - In `src/main.cpp`, create function `void showSplashScreen()`:
    - Clear display: `display.clearDisplay();`
    - Set cursor to center: `display.setCursor(10, 20);`
    - Load string from PROGMEM and display: Use `strcpy_P()` to load `STR_SPLASH_TITLE`
    - Display title: `display.println(F("Jacuzzi Controller"));`
    - Set cursor for version: `display.setCursor(40, 40);`
    - Display version: `display.println(F("v1.0"));`
    - Update display: `display.display();`
  - Call `showSplashScreen()` in `setup()` after PCF8574 initialization
  - Add non-blocking delay using millis():
    - Store start time: `uint32_t splashStart = millis();`
    - Wait 2 seconds: `while(millis() - splashStart < 2000) { yield(); }`
  - _Requirements: 39.1-39.5_
  - AI agent executes: `git add src/main.cpp`
  - AI agent executes: `git commit -m "Display splash screen for 2 seconds"`

- [x] 6. Display "READY" message after initialization
  
  **AI Agent Must**:
  - In `src/main.cpp`, create function `void showReadyMessage()`:
    - Clear display: `display.clearDisplay();`
    - Set cursor to center: `display.setCursor(40, 28);`
    - Display "READY": `display.println(F("READY"));`
    - Update display: `display.display();`
  - Call `showReadyMessage()` in `setup()` after splash screen delay
  - Add debug message: `DEBUG_PRINTLN("System Ready");`
  - Create empty `loop()` function for now: `void loop() { yield(); }`
  - _Requirements: 9.3-9.5_
  - AI agent executes: `git add src/main.cpp`
  - AI agent executes: `git commit -m "Display READY message after initialization"`

- [x] 7. Request user to compile and check memory
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run` and provide the compilation output**"
  - Wait for user's compilation output
  - Analyze output for errors
  - If errors: fix and repeat
  - If successful: Output: "**Please run: `pio run --target size` and provide the memory usage output**"
  - Wait for user's memory usage output
  - Verify memory usage is within Phase 1 budget (~50 KB Flash, ~10 KB SRAM)
  - Document memory usage

**Step 5: User Hardware Testing**

- [x] 8. Request user hardware testing
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run --target upload` and provide the upload output**"
  - Wait for user's upload confirmation
  - Output: "**Please test the following on hardware and provide feedback:**"
  - Output test cases:
    ```
    Phase 1 Hardware Tests:
    1. Power on ESP8266 - verify OLED displays splash screen with "Jacuzzi Controller v1.0"
    2. Wait 2 seconds - verify "READY" message appears
    3. Open serial monitor at 115200 baud - verify initialization messages appear
    4. Check relay module - verify all relays are OFF (no clicks, LEDs off if present)
    5. Verify I2C communication is working (no I2C errors in serial monitor)
    
    Please confirm:
    - [ ] Splash screen displays correctly for 2 seconds
    - [ ] "READY" message appears after splash screen
    - [ ] Serial monitor shows initialization messages
    - [ ] All relays are OFF on startup
    - [ ] No errors in serial monitor
    - [ ] Any issues or errors observed?
    ```
  - Wait for user feedback
  - If issues reported: analyze, fix, request recompile and upload, return to testing
  - If user confirms all working: proceed to Step 6

**Step 6: Git Operations and Documentation**

- [x] 9. Finalize Phase 1
  
  **AI Agent Must**:
  - Execute: `git add .`
  - Execute: `git commit -m "Phase 1: Basic Hardware Initialization - Complete"`
  - Execute: `git push origin phase-1-hardware-init`
  - Execute: `git checkout main`
  - Execute: `git merge phase-1-hardware-init`
  - Execute: `git push origin main`
  - Execute: `git branch -d phase-1-hardware-init`
  - Create `docs/phase-1-hardware-init.md` with:
    - Phase objectives
    - Implementation details
    - Pin assignments used
    - Memory usage (from `pio run --target size` output)
    - Test results
    - Issues encountered and resolutions
    - Notes for next phase
  - Execute: `git add docs/phase-1-hardware-init.md`
  - Execute: `git commit -m "Document Phase 1 completion"`
  - Execute: `git push origin main`

**Phase 1 Complete** ✅

### Phase 2: Sensor Integration

**Branch Name**: `phase-2-sensor-integration`

**Objective**: Read temperature and water level sensors with error handling using ESP8266 GPIO pins

**AI Agent (Developer) Responsibilities**: Implement all tasks below following the 6-step protocol

**Step 1: Pre-Git Setup**
- AI agent executes: `git checkout main`
- AI agent executes: `git pull origin main`
- AI agent executes: `git status`
- AI agent executes: `git checkout -b phase-2-sensor-integration`

**Step 2-3: Codebase Analysis**
- AI agent reads `src/main.cpp` from Phase 1
- AI agent reads `include/constants.h` from Phase 1
- AI agent reads `platformio.ini` from Phase 1
- AI agent reads `docs/phase-1-hardware-init.md` to understand Phase 1 results

**Step 4: Phase Execution**

- [x] 10. Create include/sensors.h header file
  
  **AI Agent Must**:
  - Create file `include/sensors.h`
  - Add include guards
  - Include necessary headers: `<Arduino.h>`, `<OneWire.h>`, `<DallasTemperature.h>`
  - Define SensorStatus enum:
    ```cpp
    enum SensorStatus {
        SENSOR_OK,
        TEMP_SENSOR_ERROR,
        TEMP_SENSOR_DISCONNECTED,
        WATER_LEVEL_UNKNOWN
    };
    ```
  - Declare SensorManager class:
    ```cpp
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
    ```
  - _Requirements: 4.1-4.5, 5.1-5.5, 27.1-27.5_
  - AI agent executes: `git add include/sensors.h`
  - AI agent executes: `git commit -m "Create sensors.h with SensorManager class definition"`

- [x] 11. Implement lib/sensors.cpp with DS18B20 temperature sensor reading
  
  **AI Agent Must**:
  - Create file `lib/sensors.cpp`
  - Include headers: `"sensors.h"`, `"constants.h"`
  - Create OneWire and DallasTemperature objects:
    ```cpp
    OneWire oneWire(PIN_TEMP_SENSOR);
    DallasTemperature tempSensor(&oneWire);
    ```
  - Implement `SensorManager::init()`:
    - Initialize temperature sensor: `tempSensor.begin();`
    - Set resolution to 12-bit: `tempSensor.setResolution(12);`
    - Initialize variables: `lastValidTemp = 0.0f;`, `tempValid = false;`, `tempReadFailCount = 0;`
    - Initialize water level samples to 0
    - Print debug message: "Sensors initialized"
  - Implement `SensorManager::update()`:
    - Check if it's time to read temperature (every TEMP_UPDATE_INTERVAL ms)
    - If yes:
      - Request temperature: `tempSensor.requestTemperatures();`
      - Read temperature: `float temp = tempSensor.getTempCByIndex(0);`
      - Check for error codes (-127.0°C or 85.0°C)
      - If valid temperature:
        - Store in `lastValidTemp`
        - Set `tempValid = true`
        - Reset `tempReadFailCount = 0`
        - Update `lastTempUpdate = millis()`
      - If invalid temperature:
        - Increment `tempReadFailCount`
        - If `tempReadFailCount >= 3`: set `tempValid = false`
        - Print debug message: "Temperature read failed"
    - Check if it's time to read water level (every WATER_LEVEL_INTERVAL ms)
    - If yes:
      - Read pin state: `bool pinState = digitalRead(PIN_WATER_LEVEL);`
      - Apply polarity configuration:
        ```cpp
        bool waterPresent = WATER_LEVEL_ACTIVE_LOW ? !pinState : pinState;
        ```
      - Store in samples array: `waterLevelSamples[sampleIndex] = waterPresent ? 1 : 0;`
      - Increment sample index (wrap at 3)
      - Calculate majority vote: sum samples, if >= 2 then water OK
      - Update `waterLevelOK` with majority result
  - Implement getter methods:
    - `getTemperature()`: return `lastValidTemp`
    - `isTemperatureValid()`: return `tempValid`
    - `isWaterLevelOK()`: return `waterLevelOK`
    - `getStatus()`: return appropriate SensorStatus based on state
  - _Requirements: 4.1-4.5, 5.1-5.7, 27.1-27.5, 40.1-40.5_
  - AI agent executes: `git add src/sensors.cpp`
  - AI agent executes: `git commit -m "Implement SensorManager with non-blocking sensor reading and polarity independence"`

- [x] 12. Integrate sensors into src/main.cpp
  
  **AI Agent Must**:
  - In `src/main.cpp`, include `"sensors.h"`
  - Create global SensorManager instance: `SensorManager sensors;`
  - In `setup()`, after PCF8574 initialization:
    - Call `sensors.init();`
    - Add debug message: "Sensors initialized"
  - In `loop()`:
    - Call `sensors.update();` at the beginning
    - Add `yield();` to allow ESP8266 background tasks
  - Create function `void displaySensorData()`:
    - Clear display
    - Display temperature:
      - If valid: format as "XX.X°C" using `display.print(sensors.getTemperature(), 1);`
      - If invalid: display "TEMP ERROR"
    - Display water level:
      - If OK: display "Water: OK" or filled water drop icon
      - If not OK: display "LOW WATER" (flashing)
    - Update display: `display.display();`
  - Call `displaySensorData()` in `loop()` after `sensors.update()`
  - Add non-blocking delay to limit display updates (every 500ms)
  - _Requirements: 4.3, 5.5, 23.1-23.4, 24.1-24.4_
  - AI agent executes: `git add src/main.cpp`
  - AI agent executes: `git commit -m "Integrate sensors into main loop with display"`

- [ ] 13. Request user to compile and check memory
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run` and provide the compilation output**"
  - Wait for user's compilation output
  - Analyze for errors, fix if needed
  - Output: "**Please run: `pio run --target size` and provide the memory usage output**"
  - Wait for memory usage, verify within Phase 2 budget (~70 KB Flash, ~12 KB SRAM)

**Step 5: User Hardware Testing**

- [ ] 14. Request user hardware testing
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run --target upload` and provide the upload output**"
  - Wait for upload confirmation
  - Output: "**Please test the following on hardware and provide feedback:**"
  - Output test cases:
    ```
    Phase 2 Hardware Tests:
    1. Power on ESP8266 - verify temperature displays on OLED
    2. Check temperature reading - compare with known temperature (room temp ~20-25°C)
    3. Test water level sensor:
       a. Leave D4 (GPIO2) floating - verify "LOW WATER" displays
       b. Connect D4 (GPIO2) to GND - verify "Water: OK" displays
    4. Disconnect DS18B20 - verify "TEMP ERROR" displays after ~5 seconds
    5. Reconnect DS18B20 - verify temperature reading resumes
    6. Check serial monitor - verify sensor readings are logged
    
    Please confirm:
    - [ ] Temperature displays correctly (within ±2°C of actual)
    - [ ] Water level responds to D4 connection to GND
    - [ ] "LOW WATER" displays when D4 is floating
    - [ ] "Water: OK" displays when D4 connected to GND
    - [ ] Temperature error detection works (disconnect sensor)
    - [ ] Temperature recovery works (reconnect sensor)
    - [ ] Any issues or errors observed?
    ```
  - Wait for user feedback
  - If issues: fix, recompile, upload, return to testing
  - If working: proceed to Step 6

**Step 6: Git Operations and Documentation**

- [ ] 15. Finalize Phase 2
  
  **AI Agent Must**:
  - Execute: `git add .`
  - Execute: `git commit -m "Phase 2: Sensor Integration - Complete"`
  - Execute: `git push origin phase-2-sensor-integration`
  - Execute: `git checkout main`
  - Execute: `git merge phase-2-sensor-integration`
  - Execute: `git push origin main`
  - Execute: `git branch -d phase-2-sensor-integration`
  - Create `docs/phase-2-sensor-integration.md` with full documentation
  - Execute: `git add docs/phase-2-sensor-integration.md`
  - Execute: `git commit -m "Document Phase 2 completion"`
  - Execute: `git push origin main`

**Phase 2 Complete** ✅

### Phase 3: Rotary Encoder Input

**Branch Name**: `phase-3-rotary-encoder`

**Objective**: Implement user input handling with debouncing and feedback on ESP8266

**AI Agent (Developer) Responsibilities**: Implement all tasks below following the 6-step protocol

**Step 1: Pre-Git Setup**
- AI agent executes: `git checkout main`
- AI agent executes: `git pull origin main`
- AI agent executes: `git status`
- AI agent executes: `git checkout -b phase-3-rotary-encoder`

**Step 2-3: Codebase Analysis**
- AI agent reads `src/main.cpp` from Phase 2
- AI agent reads `src/sensors.cpp` from Phase 2
- AI agent reads `include/constants.h` from Phase 2
- AI agent reads `docs/phase-2-sensor-integration.md` to understand Phase 2 results

**Step 4: Phase Execution**

- [ ] 16. Create include/input.h header file
  
  **AI Agent Must**:
  - Create file `include/input.h`
  - Add include guards
  - Include necessary headers: `<Arduino.h>`
  - Define EncoderEvent enum:
    ```cpp
    enum EncoderEvent {
        ENCODER_NONE,
        ENCODER_CW,        // Clockwise rotation
        ENCODER_CCW,       // Counter-clockwise rotation
        ENCODER_BUTTON     // Button press
    };
    ```
  - Declare InputManager class with methods: `init()`, `update()`, `getEvent()`, `hasEvent()`
  - Private members: `lastCLK`, `lastDT`, `lastSW`, `lastEncoderTime`, `lastButtonTime`, `pendingEvent`
  - _Requirements: 10.1-10.8, 16.1_
  - AI agent executes: `git add include/input.h`
  - AI agent executes: `git commit -m "Create input.h with InputManager class definition"`

- [ ] 17. Configure rotary encoder GPIO pins and implement src/input.cpp
  
  **AI Agent Must**:
  - Create file `src/input.cpp`
  - Include headers: `"input.h"`, `"constants.h"`
  - Implement `InputManager::init()`:
    - Configure encoder pins:
      - `pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);` (D7 / GPIO13)
      - `pinMode(PIN_ENCODER_DT, INPUT_PULLUP);` (D8 / GPIO15)
      - `pinMode(PIN_ENCODER_SW, INPUT_PULLUP);` (D3 / GPIO0)
    - **CRITICAL**: Document in code comment that PIN_ENCODER_DT (GPIO15) requires external 10kΩ pull-down resistor to GND for proper ESP8266 boot
    - Configure buzzer pin: `pinMode(PIN_BUZZER, OUTPUT);` (D6 / GPIO12)
    - Set buzzer LOW initially: `digitalWrite(PIN_BUZZER, LOW);`
    - Read initial pin states:
      - `lastCLK = digitalRead(PIN_ENCODER_CLK);`
      - `lastDT = digitalRead(PIN_ENCODER_DT);`
      - `lastSW = digitalRead(PIN_ENCODER_SW);`
    - Initialize timing variables: `lastEncoderTime = 0;`, `lastButtonTime = 0;`
    - Initialize event: `pendingEvent = ENCODER_NONE;`
    - Print debug message: "Input manager initialized"
  - _Requirements: 10.1-10.3, 16.1_
  - AI agent executes: `git add src/input.cpp`
  - AI agent executes: `git commit -m "Configure encoder pins with ESP8266-specific settings"`

- [ ] 18. Implement encoder rotation detection with debouncing
  
  **AI Agent Must**:
  - In `src/input.cpp`, implement `InputManager::update()`:
    - Read current encoder pin states:
      - `uint8_t currentCLK = digitalRead(PIN_ENCODER_CLK);`
      - `uint8_t currentDT = digitalRead(PIN_ENCODER_DT);`
    - Detect CLK state change (edge detection):
      - If `currentCLK != lastCLK` AND `currentCLK == LOW`:
        - Check debounce time: `if (millis() - lastEncoderTime > ENCODER_DEBOUNCE_TIME)`
        - Determine direction by reading DT:
          - If `currentDT == HIGH`: Clockwise rotation → set `pendingEvent = ENCODER_CW`
          - If `currentDT == LOW`: Counter-clockwise rotation → set `pendingEvent = ENCODER_CCW`
        - Update `lastEncoderTime = millis()`
        - Print debug message with direction
    - Update last state: `lastCLK = currentCLK;`
  - _Requirements: 10.4, 10.5, 10.7_
  - AI agent executes: `git add src/input.cpp`
  - AI agent executes: `git commit -m "Implement encoder rotation detection with 50ms debouncing"`

- [ ] 19. Implement encoder button detection with debouncing
  
  **AI Agent Must**:
  - In `src/input.cpp`, continue `InputManager::update()`:
    - Read current button state: `uint8_t currentSW = digitalRead(PIN_ENCODER_SW);`
    - Detect button press (falling edge):
      - If `currentSW != lastSW` AND `currentSW == LOW`:
        - Check debounce time: `if (millis() - lastButtonTime > ENCODER_DEBOUNCE_TIME)`
        - Set `pendingEvent = ENCODER_BUTTON`
        - Update `lastButtonTime = millis()`
        - Print debug message: "Button pressed"
    - Update last state: `lastSW = currentSW;`
  - Implement `InputManager::hasEvent()`:
    - Return `pendingEvent != ENCODER_NONE`
  - Implement `InputManager::getEvent()`:
    - Store current event: `EncoderEvent event = pendingEvent;`
    - Clear pending event: `pendingEvent = ENCODER_NONE;`
    - Return stored event
  - _Requirements: 10.6, 10.7_
  - AI agent executes: `git add src/input.cpp`
  - AI agent executes: `git commit -m "Implement button detection with debouncing and event retrieval"`

- [ ] 20. Implement buzzer feedback module
  
  **AI Agent Must**:
  - In `src/input.cpp`, create buzzer state management:
    - Add static variables at file scope:
      - `static bool buzzerActive = false;`
      - `static uint32_t buzzerStartTime = 0;`
      - `static uint16_t buzzerDuration = 0;`
    - Create function `void startBuzzer(uint16_t duration)`:
      - Set `buzzerActive = true`
      - Set `buzzerStartTime = millis()`
      - Set `buzzerDuration = duration`
      - Turn buzzer ON: `digitalWrite(PIN_BUZZER, HIGH)`
    - Create function `void updateBuzzer()`:
      - If `buzzerActive`:
        - Check if duration elapsed: `if (millis() - buzzerStartTime >= buzzerDuration)`
        - Turn buzzer OFF: `digitalWrite(PIN_BUZZER, LOW)`
        - Set `buzzerActive = false`
    - Call `updateBuzzer()` at the end of `InputManager::update()`
    - Call `startBuzzer(BUZZER_BEEP_SHORT)` when encoder event detected
    - Call `startBuzzer(BUZZER_BEEP_SHORT)` when button event detected
  - _Requirements: 10.8, 16.2, 16.3, 16.6_
  - AI agent executes: `git add src/input.cpp`
  - AI agent executes: `git commit -m "Implement non-blocking buzzer feedback for encoder events"`

- [ ] 21. Integrate input manager into src/main.cpp with test counter
  
  **AI Agent Must**:
  - In `src/main.cpp`, include `"input.h"`
  - Create global InputManager instance: `InputManager input;`
  - Create test counter variable: `int16_t testCounter = 0;`
  - In `setup()`, after sensors initialization:
    - Call `input.init();`
    - Add debug message: "Input initialized"
  - In `loop()`, after `sensors.update()`:
    - Call `input.update();`
    - Check for events: `if (input.hasEvent())`
    - Get event: `EncoderEvent event = input.getEvent();`
    - Handle events:
      - `ENCODER_CW`: increment `testCounter++`
      - `ENCODER_CCW`: decrement `testCounter--`
      - `ENCODER_BUTTON`: reset `testCounter = 0`
  - Update `displaySensorData()` to also show counter:
    - Display counter value on bottom of screen: `display.print("Counter: ");` `display.println(testCounter);`
  - _Requirements: 10.4, 10.5, 10.6_
  - AI agent executes: `git add src/main.cpp`
  - AI agent executes: `git commit -m "Integrate input manager with test counter display"`

- [ ] 22. Request user to compile and check memory
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run` and provide the compilation output**"
  - Wait for user's compilation output
  - Analyze for errors, fix if needed
  - Output: "**Please run: `pio run --target size` and provide the memory usage output**"
  - Wait for memory usage, verify within Phase 3 budget (~80 KB Flash, ~13 KB SRAM)

**Step 5: User Hardware Testing**

- [ ] 23. Request user hardware testing
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run --target upload` and provide the upload output**"
  - Wait for upload confirmation
  - Output: "**Please test the following on hardware and provide feedback:**"
  - Output test cases:
    ```
    Phase 3 Hardware Tests:
    1. Power on ESP8266 - verify display shows temperature, water level, and counter
    2. Rotate encoder clockwise - verify counter increments and buzzer beeps
    3. Rotate encoder counter-clockwise - verify counter decrements and buzzer beeps
    4. Press encoder button - verify counter resets to 0 and buzzer beeps
    5. Test rapid rotation - verify debouncing works (no double counts)
    6. Test rapid button presses - verify debouncing works (no double triggers)
    7. Check serial monitor - verify encoder events are logged
    8. Verify GPIO15 (D8/DT) has 10kΩ pull-down resistor to GND
    
    Please confirm:
    - [ ] Counter increments on clockwise rotation
    - [ ] Counter decrements on counter-clockwise rotation
    - [ ] Counter resets on button press
    - [ ] Buzzer beeps on each interaction (short beep)
    - [ ] No double-counting or bouncing issues
    - [ ] ESP8266 boots properly with encoder connected
    - [ ] Any issues or errors observed?
    ```
  - Wait for user feedback
  - If issues: fix, recompile, upload, return to testing
  - If working: proceed to Step 6

**Step 6: Git Operations and Documentation**

- [ ] 24. Finalize Phase 3
  
  **AI Agent Must**:
  - Execute: `git add .`
  - Execute: `git commit -m "Phase 3: Rotary Encoder Input - Complete"`
  - Execute: `git push origin phase-3-rotary-encoder`
  - Execute: `git checkout main`
  - Execute: `git merge phase-3-rotary-encoder`
  - Execute: `git push origin main`
  - Execute: `git branch -d phase-3-rotary-encoder`
  - Create `docs/phase-3-rotary-encoder.md` with full documentation
  - Execute: `git add docs/phase-3-rotary-encoder.md`
  - Execute: `git commit -m "Document Phase 3 completion"`
  - Execute: `git push origin main`

**Phase 3 Complete** ✅

### Phase 4: Basic Menu System

**Branch Name**: `phase-4-menu-system`

**Objective**: Implement menu navigation and rendering on ESP8266

**AI Agent (Developer) Responsibilities**: Implement all tasks below following the 6-step protocol

**Step 1: Pre-Git Setup**
- AI agent executes: `git checkout main`
- AI agent executes: `git pull origin main`
- AI agent executes: `git status`
- AI agent executes: `git checkout -b phase-4-menu-system`

**Step 2-3: Codebase Analysis**
- AI agent reads all existing source files from Phases 1-3
- AI agent reads `docs/phase-3-rotary-encoder.md` to understand Phase 3 results
- AI agent understands current input handling and display capabilities

**Step 4: Phase Execution**

- [ ] 25. Create include/menu.h with menu structure definitions
  
  **AI Agent Must**:
  - Create file `include/menu.h`
  - Add include guards
  - Include necessary headers: `<Arduino.h>`, `<avr/pgmspace.h>`
  - Define MenuId enum:
    ```cpp
    enum MenuId {
        MENU_IDLE,
        MENU_MAIN,
        MENU_CIRCULATION_PUMP,
        MENU_MASSAGE_PUMP,
        MENU_JET_PUMP,
        MENU_HEATER,
        MENU_OZONE,
        MENU_SPEAKER,
        MENU_LIGHT,
        MENU_SETTINGS,
        MENU_SETTINGS_TEMP,
        MENU_SETTINGS_TIMEOUT,
        MENU_SETTINGS_ABOUT
    };
    ```
  - Define MenuItem struct:
    ```cpp
    struct MenuItem {
        const char* label;      // Stored in PROGMEM
        MenuId submenu;         // Submenu to open, or MENU_IDLE if none
        int8_t actuatorId;      // Actuator index, or -1 if not an actuator
    };
    ```
  - Declare menu arrays in PROGMEM (to be defined in menu.cpp)
  - Declare MenuManager class with methods: `init()`, `update()`, `handleRotation()`, `handlePress()`, `getCurrentMenu()`, `isIdle()`
  - Private members: `currentMenu`, `selectedIndex`, `lastInteractionTime`
  - _Requirements: 12.1, 12.2, 19.5_
  - AI agent executes: `git add include/menu.h`
  - AI agent executes: `git commit -m "Create menu.h with menu structure and MenuManager class"`

- [ ] 26. Implement src/menu.cpp with menu data and navigation logic
  
  **AI Agent Must**:
  - Create file `src/menu.cpp`
  - Include headers: `"menu.h"`, `"constants.h"`
  - Define menu strings in PROGMEM:
    ```cpp
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
    ```
  - Define main menu array in PROGMEM:
    ```cpp
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
    ```
  - Define settings menu array in PROGMEM:
    ```cpp
    const MenuItem settingsMenuItems[] PROGMEM = {
        {STR_TARGET_TEMP, MENU_SETTINGS_TEMP, -1},
        {STR_IDLE_TIMEOUT, MENU_SETTINGS_TIMEOUT, -1},
        {STR_ABOUT, MENU_SETTINGS_ABOUT, -1}
    };
    const uint8_t settingsMenuCount = 3;
    ```
  - Implement `MenuManager::init()`:
    - Set `currentMenu = MENU_IDLE`
    - Set `selectedIndex = 0`
    - Set `lastInteractionTime = millis()`
    - Print debug message: "Menu manager initialized"
  - _Requirements: 12.1, 12.2, 19.5_
  - AI agent executes: `git add src/menu.cpp`
  - AI agent executes: `git commit -m "Define menu structure and data in PROGMEM"`

- [ ] 27. Implement menu navigation logic in src/menu.cpp
  
  **AI Agent Must**:
  - Implement `MenuManager::handleRotation(bool clockwise)`:
    - Update `lastInteractionTime = millis()`
    - Get current menu item count based on `currentMenu`
    - If clockwise: increment `selectedIndex`
    - If counter-clockwise: decrement `selectedIndex`
    - Implement wrapping: if `selectedIndex < 0`, set to `count - 1`; if `selectedIndex >= count`, set to `0`
    - Print debug message with new selection
  - Implement `MenuManager::handlePress()`:
    - Update `lastInteractionTime = millis()`
    - If `currentMenu == MENU_IDLE`:
      - Enter main menu: `currentMenu = MENU_MAIN`, `selectedIndex = 0`
    - Else if in main menu or settings menu:
      - Get selected menu item from PROGMEM
      - If item has submenu: navigate to submenu
      - If item is actuator control: navigate to actuator menu
    - Else if in actuator or settings submenu:
      - Return to parent menu (MENU_MAIN or MENU_SETTINGS)
    - Print debug message with menu transition
  - _Requirements: 12.2, 12.4, 12.5_
  - AI agent executes: `git add src/menu.cpp`
  - AI agent executes: `git commit -m "Implement menu navigation with wrapping and submenu support"`

- [ ] 28. Implement idle timeout logic in src/menu.cpp
  
  **AI Agent Must**:
  - Implement `MenuManager::update()`:
    - Check if not in idle: `if (currentMenu != MENU_IDLE)`
    - Check timeout: `if (millis() - lastInteractionTime > IDLE_TIMEOUT_DEFAULT)`
    - If timeout expired:
      - Return to idle: `currentMenu = MENU_IDLE`, `selectedIndex = 0`
      - Print debug message: "Idle timeout, returning to idle screen"
  - Implement `MenuManager::getCurrentMenu()`:
    - Return `currentMenu`
  - Implement `MenuManager::isIdle()`:
    - Return `currentMenu == MENU_IDLE`
  - _Requirements: 12.3, 37.1, 37.3_
  - AI agent executes: `git add src/menu.cpp`
  - AI agent executes: `git commit -m "Implement idle timeout with configurable duration"`

- [ ] 29. Create include/display.h for display management
  
  **AI Agent Must**:
  - Create file `include/display.h`
  - Add include guards
  - Include necessary headers: `<Arduino.h>`, `<Adafruit_SH110X.h>`
  - Forward declare classes: `class SensorManager;`, `class MenuManager;`
  - Declare DisplayManager class:
    ```cpp
    class DisplayManager {
    public:
        void init(Adafruit_SH1106G* disp);
        void update(SensorManager* sensors, MenuManager* menu);
        
    private:
        void showIdleScreen(SensorManager* sensors);
        void showMainMenu(MenuManager* menu, SensorManager* sensors);
        void showSettingsMenu(MenuManager* menu);
        
        Adafruit_SH1106G* display;
        uint32_t lastFrameTime;
        bool needsRedraw;
    };
    ```
  - _Requirements: 11.2, 11.7_
  - AI agent executes: `git add include/display.h`
  - AI agent executes: `git commit -m "Create display.h with DisplayManager class"`

- [ ] 30. Implement idle screen rendering in src/display.cpp
  
  **AI Agent Must**:
  - Create file `src/display.cpp`
  - Include headers: `"display.h"`, `"sensors.h"`, `"menu.h"`, `"constants.h"`
  - Implement `DisplayManager::init(Adafruit_SH1106G* disp)`:
    - Store display pointer: `display = disp`
    - Initialize timing: `lastFrameTime = 0`, `needsRedraw = true`
  - Implement `DisplayManager::showIdleScreen(SensorManager* sensors)`:
    - Clear display: `display->clearDisplay()`
    - Set text size: `display->setTextSize(2)`
    - Display temperature prominently at top:
      - Position cursor: `display->setCursor(10, 10)`
      - If temperature valid: `display->print(sensors->getTemperature(), 1);` `display->println(" C")`
      - If invalid: `display->println("TEMP ERR")`
    - Set text size: `display->setTextSize(1)`
    - Display water level status:
      - Position cursor: `display->setCursor(10, 40)`
      - If water OK: `display->println("Water: OK")`
      - If low water: `display->println("LOW WATER")` (implement flashing with millis())
    - Display placeholder for actuator status (bottom row)
    - Update display: `display->display()`
  - _Requirements: 13.1-13.4, 24.2, 24.3_
  - AI agent executes: `git add src/display.cpp`
  - AI agent executes: `git commit -m "Implement idle screen with temperature and water level"`

- [ ] 31. Implement menu rendering in src/display.cpp
  
  **AI Agent Must**:
  - Implement `DisplayManager::showMainMenu(MenuManager* menu, SensorManager* sensors)`:
    - Clear display
    - Draw header with temperature and water level (small text, top 12 pixels)
    - Draw menu title: "Main Menu" (centered, line 2)
    - Get current menu items from PROGMEM
    - Display 3 visible menu items (selected item in middle when possible)
    - Highlight selected item with inverse video: `display->setTextColor(SH110X_BLACK, SH110X_WHITE)`
    - Draw scroll indicators if more items above/below
    - Update display
  - Implement `DisplayManager::showSettingsMenu(MenuManager* menu)`:
    - Similar to main menu but for settings items
    - Display "Settings" as title
    - Show 3 visible items with selection highlight
  - _Requirements: 11.3, 11.4, 11.5, 11.6_
  - AI agent executes: `git add src/display.cpp`
  - AI agent executes: `git commit -m "Implement menu rendering with scrolling and selection highlight"`

- [ ] 32. Implement display update with frame rate limiting
  
  **AI Agent Must**:
  - Implement `DisplayManager::update(SensorManager* sensors, MenuManager* menu)`:
    - Check frame time: `if (millis() - lastFrameTime < DISPLAY_MIN_FRAME_TIME) return;`
    - Update `lastFrameTime = millis()`
    - Get current menu from MenuManager
    - Switch based on menu:
      - `MENU_IDLE`: call `showIdleScreen(sensors)`
      - `MENU_MAIN`: call `showMainMenu(menu, sensors)`
      - `MENU_SETTINGS`: call `showSettingsMenu(menu)`
      - Other menus: display placeholder for now
  - _Requirements: 11.2, 11.7, 17.1, 17.2_
  - AI agent executes: `git add src/display.cpp`
  - AI agent executes: `git commit -m "Implement display update with 10 FPS frame rate limiting"`

- [ ] 33. Integrate menu and display into src/main.cpp
  
  **AI Agent Must**:
  - In `src/main.cpp`, include `"menu.h"` and `"display.h"`
  - Create global instances: `MenuManager menu;`, `DisplayManager displayMgr;`
  - In `setup()`, after input initialization:
    - Call `menu.init();`
    - Call `displayMgr.init(&display);`
    - Remove old `showReadyMessage()` call
  - In `loop()`, replace old display code:
    - Call `menu.update();` (for idle timeout)
    - Handle encoder events:
      - `ENCODER_CW` or `ENCODER_CCW`: call `menu.handleRotation(event == ENCODER_CW)`
      - `ENCODER_BUTTON`: call `menu.handlePress()`
    - Call `displayMgr.update(&sensors, &menu);`
  - Remove test counter code
  - _Requirements: 12.1, 12.2, 17.1, 17.2_
  - AI agent executes: `git add src/main.cpp`
  - AI agent executes: `git commit -m "Integrate menu and display managers into main loop"`

- [ ] 34. Request user to compile and check memory
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run` and provide the compilation output**"
  - Wait for user's compilation output
  - Analyze for errors, fix if needed
  - Output: "**Please run: `pio run --target size` and provide the memory usage output**"
  - Wait for memory usage, verify within Phase 4 budget (~100 KB Flash, ~15 KB SRAM)

**Step 5: User Hardware Testing**

- [ ] 35. Request user hardware testing
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run --target upload` and provide the upload output**"
  - Wait for upload confirmation
  - Output: "**Please test the following on hardware and provide feedback:**"
  - Output test cases:
    ```
    Phase 4 Hardware Tests:
    1. Power on ESP8266 - verify idle screen shows temperature and water level
    2. Press encoder button - verify main menu appears
    3. Rotate encoder - verify menu selection moves up/down with wrapping
    4. Navigate to last menu item - rotate down - verify wraps to first item
    5. Navigate to first menu item - rotate up - verify wraps to last item
    6. Select "Settings" - verify settings submenu appears
    7. Navigate settings menu - verify 3 items visible
    8. Wait 30 seconds without interaction - verify returns to idle screen
    9. Check serial monitor - verify menu transitions are logged
    10. Verify temperature and water level display in menu header
    
    Please confirm:
    - [ ] Idle screen displays temperature and water level
    - [ ] Button press enters main menu
    - [ ] Encoder rotation navigates menu items
    - [ ] Menu wrapping works (first ↔ last)
    - [ ] Settings submenu accessible
    - [ ] Idle timeout returns to idle screen after 30 seconds
    - [ ] Temperature/water level visible in menu header
    - [ ] Any issues or errors observed?
    ```
  - Wait for user feedback
  - If issues: fix, recompile, upload, return to testing
  - If working: proceed to Step 6

**Step 6: Git Operations and Documentation**

- [ ] 36. Finalize Phase 4
  
  **AI Agent Must**:
  - Execute: `git add .`
  - Execute: `git commit -m "Phase 4: Basic Menu System - Complete"`
  - Execute: `git push origin phase-4-menu-system`
  - Execute: `git checkout main`
  - Execute: `git merge phase-4-menu-system`
  - Execute: `git push origin main`
  - Execute: `git branch -d phase-4-menu-system`
  - Create `docs/phase-4-menu-system.md` with full documentation
  - Execute: `git add docs/phase-4-menu-system.md`
  - Execute: `git commit -m "Document Phase 4 completion"`
  - Execute: `git push origin main`

**Phase 4 Complete** ✅

### Phase 5: Actuator Control

**Branch Name**: `phase-5-actuator-control`

**Objective**: Control relays through menu interface on ESP8266

**AI Agent (Developer) Responsibilities**: Implement all tasks below following the 6-step protocol

**Step 1: Pre-Git Setup**
- AI agent executes: `git checkout main`
- AI agent executes: `git pull origin main`
- AI agent executes: `git status`
- AI agent executes: `git checkout -b phase-5-actuator-control`

**Step 2-3: Codebase Analysis**
- AI agent reads all existing source files from Phases 1-4
- AI agent reads `docs/phase-4-menu-system.md` to understand Phase 4 results
- AI agent understands menu system and PCF8574 initialization

**Step 4: Phase Execution**

- [ ] 37. Create include/actuators.h for relay control
  
  **AI Agent Must**:
  - Create file `include/actuators.h`
  - Add include guards
  - Include necessary headers: `<Arduino.h>`, `<Wire.h>`
  - Declare ActuatorManager class with methods: `init()`, `setState()`, `getState()`, `getAllStates()`, private `updatePCF8574()`
  - Private members: `relayState` (uint8_t bit field), `retryCount`
  - _Requirements: 3.1-3.7, 14.1-14.6_
  - AI agent executes: `git add include/actuators.h`
  - AI agent executes: `git commit -m "Create actuators.h with ActuatorManager class"`

- [ ] 38. Implement PCF8574 relay control in src/actuators.cpp
  
  **AI Agent Must**:
  - Create file `src/actuators.cpp`
  - Include headers: `"actuators.h"`, `"constants.h"`
  - Implement `init()`: set `relayState = 0x00`, call `updatePCF8574()`
  - Implement `updatePCF8574()` with retry logic (3 attempts: 10ms, 50ms, 100ms delays)
  - Use I2C: `Wire.beginTransmission()`, `Wire.write(relayState)`, `Wire.endTransmission()`
  - Return true on success, false on failure
  - _Requirements: 3.1-3.7, 2.4, 2.5_
  - AI agent executes: `git add src/actuators.cpp`
  - AI agent executes: `git commit -m "Implement PCF8574 control with I2C retry logic"`

- [ ] 39. Implement individual actuator control methods
  
  **AI Agent Must**:
  - Implement `setState(uint8_t actuatorId, bool state)`: validate ID, update bit, call `updatePCF8574()`
  - Implement `getState(uint8_t actuatorId)`: validate ID, return bit state
  - Implement `getAllStates()`: return `relayState`
  - _Requirements: 14.1-14.6_
  - AI agent executes: `git add src/actuators.cpp`
  - AI agent executes: `git commit -m "Implement individual actuator control with bit manipulation"`

- [ ] 40. Add actuator control screens to display.cpp
  
  **AI Agent Must**:
  - Update `include/display.h`: add ActuatorManager forward declaration, update method signatures
  - Add private method: `showActuatorControl(MenuManager*, ActuatorManager*, int8_t)`
  - Implement in `src/display.cpp`: display actuator name, current status (ON/OFF), toggle instruction
  - Update `DisplayManager::update()` to handle actuator menus
  - _Requirements: 14.7, 14.8_
  - AI agent executes: `git add include/display.h src/display.cpp`
  - AI agent executes: `git commit -m "Add actuator control screen rendering"`

- [ ] 41. Implement actuator toggle logic in menu.cpp
  
  **AI Agent Must**:
  - Add method to `include/menu.h`: `int8_t getSelectedActuatorId()`
  - Implement in `src/menu.cpp`: return actuator ID based on current menu
  - Update `handlePress()` to support actuator toggle
  - _Requirements: 14.1-14.8_
  - AI agent executes: `git add include/menu.h src/menu.cpp`
  - AI agent executes: `git commit -m "Add actuator ID retrieval for toggle logic"`

- [ ] 42. Integrate actuator control into main.cpp
  
  **AI Agent Must**:
  - Include `"actuators.h"`, create global `ActuatorManager actuators`
  - In `setup()`: call `actuators.init()`
  - In `loop()`: handle button press in actuator menus, toggle state
  - Update `displayMgr.update()` to include actuators pointer
  - _Requirements: 14.1-14.8, 16.3_
  - AI agent executes: `git add src/main.cpp`
  - AI agent executes: `git commit -m "Integrate actuator control with menu system"`

- [ ] 43. Add actuator status indicators to idle screen
  
  **AI Agent Must**:
  - Update `showIdleScreen()` in `src/display.cpp`: add actuators parameter
  - Display compact indicators at bottom: "PUMP", "MSGE", "JET", "HEAT", "O3", "AUD", "LGT"
  - Only show indicators for active actuators
  - _Requirements: 25.1-25.8_
  - AI agent executes: `git add src/display.cpp`
  - AI agent executes: `git commit -m "Add actuator status indicators to idle screen"`

- [ ] 44. Request user to compile and check memory
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run` and provide the compilation output**"
  - Wait for user's compilation output, analyze for errors
  - Output: "**Please run: `pio run --target size` and provide the memory usage output**"
  - Wait for memory usage, verify within Phase 5 budget (~110 KB Flash, ~16 KB SRAM)

**Step 5: User Hardware Testing**

- [ ] 45. Request user hardware testing
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run --target upload` and provide the upload output**"
  - Wait for upload confirmation
  - Output: "**Please test the following on hardware and provide feedback:**"
  - Output test cases:
    ```
    Phase 5 Hardware Tests:
    1. Power on ESP8266 - verify idle screen shows no actuator indicators (all OFF)
    2. Enter main menu - navigate to "Circulation Pump"
    3. Press button - verify relay clicks ON and display shows "STATUS: ON"
    4. Press button again - verify relay clicks OFF and display shows "STATUS: OFF"
    5. Test each actuator (7 total): Circulation, Massage, Jet, Heater, Ozone, Speaker, Light
    6. Turn on multiple actuators - return to idle screen
    7. Verify idle screen shows indicators for active actuators
    8. Check serial monitor - verify relay state changes are logged
    
    Please confirm:
    - [ ] All 7 actuators can be toggled ON/OFF
    - [ ] Relay clicks heard for each state change
    - [ ] Actuator control screen shows correct status
    - [ ] Idle screen shows active actuator indicators
    - [ ] Multiple actuators can be ON simultaneously
    - [ ] Any issues or errors observed?
    ```
  - Wait for user feedback
  - If issues: fix, recompile, upload, return to testing
  - If working: proceed to Step 6

**Step 6: Git Operations and Documentation**

- [ ] 46. Finalize Phase 5
  
  **AI Agent Must**:
  - Execute: `git add .`
  - Execute: `git commit -m "Phase 5: Actuator Control - Complete"`
  - Execute: `git push origin phase-5-actuator-control`
  - Execute: `git checkout main`
  - Execute: `git merge phase-5-actuator-control`
  - Execute: `git push origin main`
  - Execute: `git branch -d phase-5-actuator-control`
  - Create `docs/phase-5-actuator-control.md` with full documentation
  - Execute: `git add docs/phase-5-actuator-control.md`
  - Execute: `git commit -m "Document Phase 5 completion"`
  - Execute: `git push origin main`

**Phase 5 Complete** ✅
  - Ensure all tests pass, ask the user if questions arise.

### Phase 6: Safety Interlocks

**Branch Name**: `phase-6-safety-interlocks`

**Objective**: Implement all safety features and fault handling on ESP8266

**AI Agent (Developer) Responsibilities**: Implement all tasks below following the 6-step protocol

**Step 1: Pre-Git Setup**
- AI agent executes: `git checkout main`
- AI agent executes: `git pull origin main`
- AI agent executes: `git status`
- AI agent executes: `git checkout -b phase-6-safety-interlocks`

**Step 2-3: Codebase Analysis**
- AI agent reads all existing source files from Phases 1-5
- AI agent reads `docs/phase-5-actuator-control.md` to understand Phase 5 results
- AI agent understands actuator control, sensor reading, and current system state

**Step 4: Phase Execution**

- [ ] 47. Create include/safety.h for safety management
  
  **AI Agent Must**:
  - Create file `include/safety.h`
  - Add include guards
  - Include necessary headers: `<Arduino.h>`
  - Forward declare classes: `class SensorManager;`, `class ActuatorManager;`
  - Declare SafetyManager class:
    ```cpp
    class SafetyManager {
    public:
        void init();
        void update(SensorManager* sensors, ActuatorManager* actuators);
        
        bool checkWaterLevelInterlock(uint8_t actuatorId);
        bool checkHeaterInterlock(ActuatorManager* actuators);
        bool checkThermalRunaway(float temperature);
        
        void enterFaultState(const char* reason);
        bool isInFaultState();
        const char* getFaultReason();
        
    private:
        bool faultState;
        const char* faultReason;
        uint32_t lastThermalCheck;
        uint32_t buzzerToggleTime;
        bool buzzerState;
    };
    ```
  - _Requirements: 8.1-8.7, 9.5_
  - AI agent executes: `git add include/safety.h`
  - AI agent executes: `git commit -m "Create safety.h with SafetyManager class"`

- [ ] 48. Implement water level interlock in src/safety.cpp
  
  **AI Agent Must**:
  - Create file `src/safety.cpp`
  - Include headers: `"safety.h"`, `"sensors.h"`, `"actuators.h"`, `"constants.h"`
  - Implement `SafetyManager::init()`:
    - Set `faultState = false`
    - Set `faultReason = nullptr`
    - Set `lastThermalCheck = 0`
    - Set `buzzerToggleTime = 0`, `buzzerState = false`
    - Print debug message: "Safety manager initialized"
  - Implement `SafetyManager::checkWaterLevelInterlock(uint8_t actuatorId)`:
    - If actuator is pump (CIRCULATION, MASSAGE, JET) or HEATER:
      - Return false if water level is NOT OK (interlock prevents activation)
      - Return true if water level is OK (allow activation)
    - For other actuators: return true (no interlock)
  - _Requirements: 6.1-6.10_
  - AI agent executes: `git add src/safety.cpp`
  - AI agent executes: `git commit -m "Implement water level interlock for pumps and heater"`

- [ ] 49. Implement heater safety interlock in src/safety.cpp
  
  **AI Agent Must**:
  - Implement `SafetyManager::checkHeaterInterlock(ActuatorManager* actuators)`:
    - Check if circulation pump is ON: `bool pumpOn = actuators->getState(ACTUATOR_CIRCULATION_PUMP)`
    - If pump is OFF: return false (interlock prevents heater activation)
    - If pump is ON: return true (allow heater activation)
  - _Requirements: 7.1-7.7_
  - AI agent executes: `git add src/safety.cpp`
  - AI agent executes: `git commit -m "Implement heater interlock requiring circulation pump"`

- [ ] 50. Implement thermal runaway protection in src/safety.cpp
  
  **AI Agent Must**:
  - Implement `SafetyManager::checkThermalRunaway(float temperature)`:
    - Check if enough time elapsed: `if (millis() - lastThermalCheck < THERMAL_RUNAWAY_CHECK_MS) return false`
    - Update check time: `lastThermalCheck = millis()`
    - Check temperature: `if (temperature >= TEMP_THERMAL_RUNAWAY)`
      - Return true (thermal runaway detected)
    - Else: return false (temperature OK)
  - _Requirements: 8.1-8.7_
  - AI agent executes: `git add src/safety.cpp`
  - AI agent executes: `git commit -m "Implement thermal runaway detection at 45°C threshold"`

- [ ] 51. Implement fault state management in src/safety.cpp
  
  **AI Agent Must**:
  - Implement `SafetyManager::enterFaultState(const char* reason)`:
    - Set `faultState = true`
    - Set `faultReason = reason`
    - Print debug message with fault reason
    - Print: "FAULT STATE ENTERED - POWER CYCLE REQUIRED"
  - Implement `SafetyManager::isInFaultState()`:
    - Return `faultState`
  - Implement `SafetyManager::getFaultReason()`:
    - Return `faultReason`
  - _Requirements: 8.6, 8.7_
  - AI agent executes: `git add src/safety.cpp`
  - AI agent executes: `git commit -m "Implement sticky fault state management"`

- [ ] 52. Implement safety update loop in src/safety.cpp
  
  **AI Agent Must**:
  - Implement `SafetyManager::update(SensorManager* sensors, ActuatorManager* actuators)`:
    - If in fault state:
      - Manage continuous buzzer (1Hz on/off pattern)
      - Check if toggle time elapsed: `if (millis() - buzzerToggleTime >= 1000)`
      - Toggle buzzer: `buzzerState = !buzzerState`, `digitalWrite(PIN_BUZZER, buzzerState ? HIGH : LOW)`
      - Update toggle time: `buzzerToggleTime = millis()`
      - Return early (no other checks needed)
    - Check thermal runaway if temperature valid:
      - If `checkThermalRunaway(sensors->getTemperature())`:
        - Call `actuators->emergencyShutdown()` (to be implemented)
        - Call `enterFaultState("OVERHEAT")`
    - Check water level interlock for active actuators:
      - If water level NOT OK:
        - Deactivate circulation pump if ON
        - Deactivate massage pump if ON
        - Deactivate jet pump if ON
        - Deactivate heater if ON
        - Print debug message: "Water level interlock triggered"
    - Check heater interlock:
      - If heater is ON AND circulation pump is OFF:
        - Deactivate heater within 100ms
        - Print debug message: "Heater interlock triggered"
  - _Requirements: 6.1-6.10, 7.1-7.7, 8.1-8.7_
  - AI agent executes: `git add src/safety.cpp`
  - AI agent executes: `git commit -m "Implement safety update loop with all interlock checks"`

- [ ] 53. Add emergency shutdown to actuators.cpp
  
  **AI Agent Must**:
  - In `include/actuators.h`, add method declaration: `void emergencyShutdown();`
  - In `src/actuators.cpp`, implement `ActuatorManager::emergencyShutdown()`:
    - Set all relays OFF: `relayState = 0x00`
    - Write to PCF8574 immediately: call `updatePCF8574()`
    - Print debug message: "EMERGENCY SHUTDOWN - ALL RELAYS OFF"
  - _Requirements: 8.3, 9.1_
  - AI agent executes: `git add include/actuators.h src/actuators.cpp`
  - AI agent executes: `git commit -m "Add emergency shutdown method to ActuatorManager"`

- [ ] 54. Add interlock checks to actuator control in main.cpp
  
  **AI Agent Must**:
  - In `src/main.cpp`, include `"safety.h"`
  - Create global instance: `SafetyManager safety;`
  - In `setup()`, after actuators initialization:
    - Call `safety.init();`
    - Print debug message: "Safety initialized"
  - In `loop()`, before handling encoder events:
    - Call `safety.update(&sensors, &actuators);`
    - If in fault state: skip all user input handling
  - In actuator toggle logic (button press):
    - Before toggling actuator ON, check interlocks:
      - Check water level: `if (!safety.checkWaterLevelInterlock(actId))`
        - Display warning, sound buzzer, don't activate
      - If heater: check pump interlock: `if (!safety.checkHeaterInterlock(&actuators))`
        - Display warning, sound buzzer, don't activate
    - Only toggle if interlocks pass
  - _Requirements: 6.1-6.10, 7.1-7.7, 8.1-8.7_
  - AI agent executes: `git add src/main.cpp`
  - AI agent executes: `git commit -m "Integrate safety interlocks into actuator control"`

- [ ] 55. Add fault state display to display.cpp
  
  **AI Agent Must**:
  - In `include/display.h`, add method: `void showFaultScreen(SafetyManager* safety);`
  - In `src/display.cpp`, implement `showFaultScreen()`:
    - Clear display
    - Set large text: `display->setTextSize(2)`
    - Display "FAULT" at top (centered)
    - Set normal text: `display->setTextSize(1)`
    - Display fault reason: `display->println(safety->getFaultReason())`
    - Display "POWER CYCLE" instruction
    - Display "REQUIRED" on next line
    - Update display
  - Update `DisplayManager::update()` to check fault state first:
    - If `safety->isInFaultState()`: call `showFaultScreen(safety)` and return
    - Else: continue with normal display logic
  - Add interlock warning display to actuator control screen:
    - If water level low: display "LOW WATER" warning
    - If heater without pump: display "START PUMP FIRST" warning
  - _Requirements: 8.6, 8.7, 6.10, 7.7_
  - AI agent executes: `git add include/display.h src/display.cpp`
  - AI agent executes: `git commit -m "Add fault state and interlock warning displays"`

- [ ] 56. Request user to compile and check memory
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run` and provide the compilation output**"
  - Wait for user's compilation output, analyze for errors
  - Output: "**Please run: `pio run --target size` and provide the memory usage output**"
  - Wait for memory usage, verify within Phase 6 budget (~130 KB Flash, ~18 KB SRAM)

**Step 5: User Hardware Testing**

- [ ] 57. Request user hardware testing
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run --target upload` and provide the upload output**"
  - Wait for upload confirmation
  - Output: "**Please test the following on hardware and provide feedback:**"
  - Output test cases:
    ```
    Phase 6 Hardware Tests:
    1. Power on ESP8266 - verify normal operation
    2. Test water level interlock:
       a. Disconnect D4 from GND (simulate low water)
       b. Try to activate circulation pump - verify prevented, buzzer sounds, warning displays
       c. Try to activate heater - verify prevented, buzzer sounds, warning displays
       d. Connect D4 to GND (water OK) - verify pumps and heater can activate
    3. Test heater interlock:
       a. Ensure circulation pump is OFF
       b. Try to activate heater - verify prevented, "START PUMP FIRST" displays, buzzer sounds
       c. Turn ON circulation pump
       d. Activate heater - verify allowed
       e. Turn OFF circulation pump - verify heater deactivates within 100ms
    4. Test thermal runaway (CAREFUL - use modified code or simulated temperature):
       a. Modify code temporarily to report temperature > 45°C
       b. Upload and run - verify fault state entered within 500ms
       c. Verify all relays turn OFF immediately
       d. Verify "OVERHEAT FAULT" displays
       e. Verify continuous buzzer (1Hz on/off)
       f. Try encoder input - verify ignored
       g. Power cycle - verify system recovers
    5. Check serial monitor - verify all safety events are logged
    
    Please confirm:
    - [ ] Water level interlock prevents pump/heater activation when water low
    - [ ] Heater interlock prevents activation without circulation pump
    - [ ] Heater deactivates within 100ms when pump turns off
    - [ ] Thermal runaway triggers fault state and emergency shutdown
    - [ ] Fault state is sticky (requires power cycle)
    - [ ] Fault state ignores all user input
    - [ ] Continuous buzzer sounds in fault state
    - [ ] Any issues or errors observed?
    ```
  - Wait for user feedback
  - If issues: fix, recompile, upload, return to testing
  - If working: proceed to Step 6

**Step 6: Git Operations and Documentation**

- [ ] 58. Finalize Phase 6
  
  **AI Agent Must**:
  - Execute: `git add .`
  - Execute: `git commit -m "Phase 6: Safety Interlocks - Complete"`
  - Execute: `git push origin phase-6-safety-interlocks`
  - Execute: `git checkout main`
  - Execute: `git merge phase-6-safety-interlocks`
  - Execute: `git push origin main`
  - Execute: `git branch -d phase-6-safety-interlocks`
  - Create `docs/phase-6-safety-interlocks.md` with full documentation
  - Execute: `git add docs/phase-6-safety-interlocks.md`
  - Execute: `git commit -m "Document Phase 6 completion"`
  - Execute: `git push origin main`

**Phase 6 Complete** ✅


### Phase 7: Settings and EEPROM

**Branch Name**: `phase-7-settings-eeprom`

**Objective**: Implement persistent configuration storage on ESP8266

**AI Agent (Developer) Responsibilities**: Implement all tasks below following the 6-step protocol

**Step 1: Pre-Git Setup**
- AI agent executes: `git checkout main`
- AI agent executes: `git pull origin main`
- AI agent executes: `git status`
- AI agent executes: `git checkout -b phase-7-settings-eeprom`

**Step 2-3: Codebase Analysis**
- AI agent reads all existing source files from Phases 1-6
- AI agent reads `docs/phase-6-safety-interlocks.md` to understand Phase 6 results
- AI agent understands current system configuration and menu structure

**Step 4: Phase Execution**

- [ ] 59. Create include/settings.h for EEPROM management
  
  **AI Agent Must**:
  - Create file `include/settings.h`
  - Add include guards
  - Include necessary headers: `<Arduino.h>`, `<EEPROM.h>`
  - Define EEPROM settings structure:
    ```cpp
    struct EEPROMSettings {
        uint16_t magicNumber;      // 0xABCD for validation
        float targetTemp;          // Target temperature (30-40°C)
        uint16_t idleTimeout;      // Idle timeout in seconds (10-120)
        bool autoHeaterMode;       // Auto heater control enabled
        uint16_t checksum;         // CRC16 checksum
    };
    ```
  - Define constants:
    - `#define EEPROM_MAGIC 0xABCD`
    - `#define EEPROM_START_ADDR 0`
    - `#define EEPROM_SIZE sizeof(EEPROMSettings)`
  - Declare SettingsManager class:
    ```cpp
    class SettingsManager {
    public:
        void init();
        void loadSettings();
        void saveSettings();
        
        float getTargetTemp();
        void setTargetTemp(float temp);
        uint16_t getIdleTimeout();
        void setIdleTimeout(uint16_t timeout);
        bool getAutoHeaterMode();
        void setAutoHeaterMode(bool mode);
        
    private:
        uint16_t calculateCRC16(uint8_t* data, size_t length);
        bool validateSettings(EEPROMSettings* settings);
        void setDefaults();
        
        EEPROMSettings currentSettings;
        uint32_t lastSaveTime;
        bool pendingSave;
    };
    ```
  - _Requirements: 32.1-32.3, 32.6_
  - AI agent executes: `git add include/settings.h`
  - AI agent executes: `git commit -m "Create settings.h with EEPROM structure and SettingsManager"`

- [ ] 60. Implement EEPROM functions in src/settings.cpp
  
  **AI Agent Must**:
  - Create file `src/settings.cpp`
  - Include headers: `"settings.h"`, `"constants.h"`
  - Implement `SettingsManager::init()`:
    - Initialize EEPROM: `EEPROM.begin(512);` (ESP8266 requires explicit size)
    - Set defaults initially: call `setDefaults()`
    - Load settings from EEPROM: call `loadSettings()`
    - Set `pendingSave = false`, `lastSaveTime = 0`
    - Print debug message: "Settings manager initialized"
  - Implement `SettingsManager::setDefaults()`:
    - Set `currentSettings.magicNumber = EEPROM_MAGIC`
    - Set `currentSettings.targetTemp = 37.0f` (default)
    - Set `currentSettings.idleTimeout = 30` (default 30 seconds)
    - Set `currentSettings.autoHeaterMode = false` (default manual)
    - Calculate and set checksum
  - Implement `SettingsManager::calculateCRC16(uint8_t* data, size_t length)`:
    - Implement CRC16-CCITT algorithm
    - Initial value: 0xFFFF
    - Polynomial: 0x1021
    - Return calculated CRC
  - _Requirements: 32.1-32.3, 32.6_
  - AI agent executes: `git add src/settings.cpp`
  - AI agent executes: `git commit -m "Implement EEPROM initialization and CRC16 calculation"`

- [ ] 61. Implement EEPROM read/write in src/settings.cpp
  
  **AI Agent Must**:
  - Implement `SettingsManager::loadSettings()`:
    - Read settings from EEPROM:
      ```cpp
      EEPROMSettings loaded;
      EEPROM.get(EEPROM_START_ADDR, loaded);
      ```
    - Validate magic number: `if (loaded.magicNumber != EEPROM_MAGIC)`
      - Use defaults if invalid
      - Print debug: "EEPROM invalid, using defaults"
      - Return
    - Calculate checksum of loaded data (exclude checksum field)
    - Compare with stored checksum
    - If checksum valid:
      - Copy to `currentSettings`
      - Print debug: "Settings loaded from EEPROM"
    - Else:
      - Use defaults
      - Print debug: "EEPROM checksum failed, using defaults"
  - Implement `SettingsManager::saveSettings()`:
    - Calculate checksum: `currentSettings.checksum = calculateCRC16(...)`
    - Write to EEPROM:
      ```cpp
      EEPROM.put(EEPROM_START_ADDR, currentSettings);
      EEPROM.commit();  // ESP8266 requires commit
      ```
    - Print debug: "Settings saved to EEPROM"
  - _Requirements: 32.4, 32.5, 32.6_
  - AI agent executes: `git add src/settings.cpp`
  - AI agent executes: `git commit -m "Implement EEPROM read/write with validation"`

- [ ] 62. Implement settings getters and setters with validation
  
  **AI Agent Must**:
  - Implement `SettingsManager::getTargetTemp()`: return `currentSettings.targetTemp`
  - Implement `SettingsManager::setTargetTemp(float temp)`:
    - Validate range: `if (temp < TEMP_MIN || temp > TEMP_MAX) return`
    - Set value: `currentSettings.targetTemp = temp`
    - Mark for save: `pendingSave = true`, `lastSaveTime = millis()`
  - Implement `SettingsManager::getIdleTimeout()`: return `currentSettings.idleTimeout`
  - Implement `SettingsManager::setIdleTimeout(uint16_t timeout)`:
    - Validate range: `if (timeout < IDLE_TIMEOUT_MIN || timeout > IDLE_TIMEOUT_MAX) return`
    - Set value: `currentSettings.idleTimeout = timeout`
    - Mark for save: `pendingSave = true`, `lastSaveTime = millis()`
  - Implement `SettingsManager::getAutoHeaterMode()`: return `currentSettings.autoHeaterMode`
  - Implement `SettingsManager::setAutoHeaterMode(bool mode)`:
    - Set value: `currentSettings.autoHeaterMode = mode`
    - Mark for save: `pendingSave = true`, `lastSaveTime = millis()`
  - Add update method to handle delayed save:
    - If `pendingSave` AND `millis() - lastSaveTime > 1000`:
      - Call `saveSettings()`
      - Set `pendingSave = false`
  - _Requirements: 30.1, 37.1, 29.5_
  - AI agent executes: `git add src/settings.cpp`
  - AI agent executes: `git commit -m "Implement settings accessors with validation and delayed save"`

- [ ] 63. Add Settings submenu to menu.cpp
  
  **AI Agent Must**:
  - In `src/menu.cpp`, update settings menu strings in PROGMEM (already defined in Phase 4)
  - Verify settings menu items array includes:
    - "Target Temp" → MENU_SETTINGS_TEMP
    - "Idle Timeout" → MENU_SETTINGS_TIMEOUT
    - "About" → MENU_SETTINGS_ABOUT
  - Update menu navigation to handle settings submenus
  - _Requirements: 29.1, 29.2, 29.4_
  - AI agent executes: `git add src/menu.cpp`
  - AI agent executes: `git commit -m "Verify settings submenu structure"`

- [ ] 64. Implement target temperature setting screen in display.cpp
  
  **AI Agent Must**:
  - In `include/display.h`, add forward declaration: `class SettingsManager;`
  - Add method: `void showTempSettingScreen(SettingsManager* settings);`
  - In `src/display.cpp`, implement `showTempSettingScreen()`:
    - Clear display
    - Display title: "Target Temperature"
    - Display current value: `settings->getTargetTemp()` with 1 decimal place
    - Display unit: "°C"
    - Display range: "30.0 - 40.0"
    - Display instruction: "Rotate to adjust"
    - Update display
  - Update `DisplayManager::update()` to handle MENU_SETTINGS_TEMP
  - _Requirements: 30.1, 29.5_
  - AI agent executes: `git add include/display.h src/display.cpp`
  - AI agent executes: `git commit -m "Implement target temperature setting screen"`

- [ ] 65. Implement idle timeout setting screen in display.cpp
  
  **AI Agent Must**:
  - Add method to `include/display.h`: `void showTimeoutSettingScreen(SettingsManager* settings);`
  - Implement in `src/display.cpp`:
    - Clear display
    - Display title: "Idle Timeout"
    - Display current value: `settings->getIdleTimeout()`
    - Display unit: "seconds"
    - Display range: "10 - 120"
    - Display instruction: "Rotate to adjust"
    - Update display
  - Update `DisplayManager::update()` to handle MENU_SETTINGS_TIMEOUT
  - _Requirements: 37.1, 29.5_
  - AI agent executes: `git add include/display.h src/display.cpp`
  - AI agent executes: `git commit -m "Implement idle timeout setting screen"`

- [ ] 66. Implement About screen in display.cpp
  
  **AI Agent Must**:
  - Add method to `include/display.h`: `void showAboutScreen();`
  - Implement in `src/display.cpp`:
    - Clear display
    - Display "Jacuzzi Controller"
    - Display version: "v1.0"
    - Display compile date: `__DATE__`
    - Display compile time: `__TIME__`
    - Display uptime: calculate from `millis()`, format as "HH:MM:SS"
    - Update display
  - Update `DisplayManager::update()` to handle MENU_SETTINGS_ABOUT
  - _Requirements: 38.1-38.5_
  - AI agent executes: `git add include/display.h src/display.cpp`
  - AI agent executes: `git commit -m "Implement About screen with version and uptime"`

- [ ] 67. Integrate settings into main.cpp with encoder adjustment
  
  **AI Agent Must**:
  - In `src/main.cpp`, include `"settings.h"`
  - Create global instance: `SettingsManager settings;`
  - In `setup()`, after safety initialization:
    - Call `settings.init();`
    - Print debug message: "Settings initialized"
  - In `loop()`, handle encoder events in settings screens:
    - If in MENU_SETTINGS_TEMP:
      - On ENCODER_CW: increment target temp by TEMP_INCREMENT (0.5°C)
      - On ENCODER_CCW: decrement target temp by TEMP_INCREMENT
      - Call `settings.setTargetTemp(newValue)`
    - If in MENU_SETTINGS_TIMEOUT:
      - On ENCODER_CW: increment timeout by IDLE_TIMEOUT_STEP (10 seconds)
      - On ENCODER_CCW: decrement timeout by IDLE_TIMEOUT_STEP
      - Call `settings.setIdleTimeout(newValue)`
    - On ENCODER_BUTTON in settings screens: return to settings menu
  - Call `settings.update()` in main loop (for delayed save)
  - Update `displayMgr.update()` to include settings pointer
  - _Requirements: 30.1, 37.1, 29.5_
  - AI agent executes: `git add src/main.cpp`
  - AI agent executes: `git commit -m "Integrate settings with encoder adjustment and EEPROM save"`

- [ ] 68. Request user to compile and check memory
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run` and provide the compilation output**"
  - Wait for user's compilation output, analyze for errors
  - Output: "**Please run: `pio run --target size` and provide the memory usage output**"
  - Wait for memory usage, verify within Phase 7 budget (~140 KB Flash, ~19 KB SRAM)

**Step 5: User Hardware Testing**

- [ ] 69. Request user hardware testing
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run --target upload` and provide the upload output**"
  - Wait for upload confirmation
  - Output: "**Please test the following on hardware and provide feedback:**"
  - Output test cases:
    ```
    Phase 7 Hardware Tests:
    1. Power on ESP8266 - verify normal operation
    2. Navigate to Settings menu - verify 3 items: Target Temp, Idle Timeout, About
    3. Select "Target Temp":
       a. Verify current value displays (default 37.0°C)
       b. Rotate encoder clockwise - verify increments by 0.5°C
       c. Rotate encoder counter-clockwise - verify decrements by 0.5°C
       d. Try to exceed 40.0°C - verify clamped at maximum
       e. Try to go below 30.0°C - verify clamped at minimum
       f. Set to 35.5°C - press button to exit
    4. Select "Idle Timeout":
       a. Verify current value displays (default 30 seconds)
       b. Rotate encoder - verify adjusts by 10-second increments
       c. Set to 60 seconds - press button to exit
    5. Power cycle ESP8266
    6. Navigate to Settings - verify target temp is 35.5°C and timeout is 60 seconds
    7. Select "About":
       a. Verify firmware version displays
       b. Verify compile date/time displays
       c. Verify uptime displays and increments
    8. Check serial monitor - verify EEPROM operations are logged
    
    Please confirm:
    - [ ] Target temperature adjusts in 0.5°C increments
    - [ ] Idle timeout adjusts in 10-second increments
    - [ ] Settings are clamped to valid ranges
    - [ ] Settings persist after power cycle
    - [ ] About screen shows correct information
    - [ ] Uptime increments correctly
    - [ ] Any issues or errors observed?
    ```
  - Wait for user feedback
  - If issues: fix, recompile, upload, return to testing
  - If working: proceed to Step 6

**Step 6: Git Operations and Documentation**

- [ ] 70. Finalize Phase 7
  
  **AI Agent Must**:
  - Execute: `git add .`
  - Execute: `git commit -m "Phase 7: Settings and EEPROM - Complete"`
  - Execute: `git push origin phase-7-settings-eeprom`
  - Execute: `git checkout main`
  - Execute: `git merge phase-7-settings-eeprom`
  - Execute: `git push origin main`
  - Execute: `git branch -d phase-7-settings-eeprom`
  - Create `docs/phase-7-settings-eeprom.md` with full documentation
  - Execute: `git add docs/phase-7-settings-eeprom.md`
  - Execute: `git commit -m "Document Phase 7 completion"`
  - Execute: `git push origin main`

**Phase 7 Complete** ✅


### Phase 8: Automatic Heater Control

**Branch Name**: `phase-8-automatic-heater`

**Objective**: Implement temperature regulation with hysteresis on ESP8266

**AI Agent (Developer) Responsibilities**: Implement all tasks below following the 6-step protocol

**Step 1: Pre-Git Setup**
- AI agent executes: `git checkout main`
- AI agent executes: `git pull origin main`
- AI agent executes: `git status`
- AI agent executes: `git checkout -b phase-8-automatic-heater`

**Step 2-3: Codebase Analysis**
- AI agent reads all existing source files from Phases 1-7
- AI agent reads `docs/phase-7-settings-eeprom.md` to understand Phase 7 results
- AI agent understands settings management, actuator control, and safety interlocks

**Step 4: Phase Execution**

- [ ] 71. Verify auto heater mode in settings (already added in Phase 7)
  
  **AI Agent Must**:
  - Verify `include/settings.h` has `autoHeaterMode` in EEPROMSettings struct
  - Verify `src/settings.cpp` has getters/setters for auto heater mode
  - If missing, add them now
  - _Requirements: 31.4, 32.3_
  - AI agent executes: `git add include/settings.h src/settings.cpp` (if modified)
  - AI agent executes: `git commit -m "Verify auto heater mode in settings"` (if modified)

- [ ] 72. Create include/heater_control.h for automatic control logic
  
  **AI Agent Must**:
  - Create file `include/heater_control.h`
  - Add include guards, forward declarations
  - Declare HeaterController class with methods: `init()`, `update()`, `isHeating()`
  - Private members: `heatingActive`, `lastControlUpdate`
  - _Requirements: 30.2, 30.3, 31.1, 31.2_
  - AI agent executes: `git add include/heater_control.h`
  - AI agent executes: `git commit -m "Create heater_control.h with HeaterController class"`

- [ ] 73. Implement hysteresis-based control in src/heater_control.cpp
  
  **AI Agent Must**:
  - Create file `src/heater_control.cpp`
  - Implement `init()`: initialize variables
  - Implement `update()` with hysteresis logic:
    - Check auto mode enabled, temperature valid
    - If temp < (target - TEMP_HYSTERESIS): activate heater (check interlocks first)
    - If temp >= target: deactivate heater
  - Implement `isHeating()`: return heating status
  - _Requirements: 30.2, 30.3, 30.5, 31.1, 31.2_
  - AI agent executes: `git add src/heater_control.cpp`
  - AI agent executes: `git commit -m "Implement hysteresis-based temperature control"`

- [ ] 74. Add auto mode toggle to heater control screen
  
  **AI Agent Must**:
  - Update `showActuatorControl()` in `src/display.cpp` for heater
  - Display mode (MANUAL/AUTO), target temp, current temp, heating status
  - Display instruction for mode toggle
  - _Requirements: 31.4, 30.4, 31.3_
  - AI agent executes: `git add src/display.cpp`
  - AI agent executes: `git commit -m "Update heater screen to show auto mode"`

- [ ] 75. Integrate automatic control into main.cpp
  
  **AI Agent Must**:
  - Include `"heater_control.h"`, create global `HeaterController heaterCtrl`
  - In `setup()`: call `heaterCtrl.init()`
  - In `loop()`: call `heaterCtrl.update(&sensors, &actuators, &settings, &safety)`
  - Implement mode toggle on long button press in heater menu
  - _Requirements: 31.1, 31.2, 31.4, 31.5_
  - AI agent executes: `git add src/main.cpp`
  - AI agent executes: `git commit -m "Integrate automatic heater control"`

- [ ] 76. Add auto mode indicators to idle screen
  
  **AI Agent Must**:
  - Update `showIdleScreen()` in `src/display.cpp`
  - Display "AUTO" indicator if auto mode enabled
  - Display target temperature with arrow
  - Display "HEATING" indicator when actively heating
  - _Requirements: 30.4, 31.3, 15.5_
  - AI agent executes: `git add src/display.cpp`
  - AI agent executes: `git commit -m "Add auto mode indicators to idle screen"`

- [ ] 77. Implement graceful degradation for sensor failure
  
  **AI Agent Must**:
  - Update `heater_control.cpp`: disable auto heater if temperature invalid
  - Update display: show "SENSOR FAULT" message in auto mode
  - _Requirements: 40.1-40.5_
  - AI agent executes: `git add src/heater_control.cpp src/display.cpp`
  - AI agent executes: `git commit -m "Implement graceful degradation for sensor failure"`

- [ ] 78. Request user to compile and check memory
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run` and provide the compilation output**"
  - Wait for user's compilation output, analyze for errors
  - Output: "**Please run: `pio run --target size` and provide the memory usage output**"
  - Wait for memory usage, verify within Phase 8 budget (~150 KB Flash, ~20 KB SRAM)

**Step 5: User Hardware Testing**

- [ ] 79. Request user hardware testing
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run --target upload` and provide the upload output**"
  - Wait for upload confirmation
  - Output: "**Please test the following on hardware and provide feedback:**"
  - Output test cases:
    ```
    Phase 8 Hardware Tests:
    1. Navigate to Heater - verify "Mode: MANUAL"
    2. Long press button - verify mode changes to "AUTO"
    3. Set target temp above current - turn ON circulation pump
    4. Verify heater activates when temp < (target - 0.5°C)
    5. Verify heater deactivates when temp >= target
    6. Test interlocks: turn OFF pump - verify heater deactivates
    7. Disconnect sensor - verify auto mode disables
    8. Power cycle - verify auto mode setting persists
    
    Please confirm:
    - [ ] Auto mode toggles with long press
    - [ ] Heater activates/deactivates based on temperature
    - [ ] Hysteresis prevents rapid cycling
    - [ ] Interlocks work in auto mode
    - [ ] Sensor failure disables auto mode
    - [ ] Auto mode setting persists
    - [ ] Any issues or errors observed?
    ```
  - Wait for user feedback
  - If issues: fix, recompile, upload, return to testing
  - If working: proceed to Step 6

**Step 6: Git Operations and Documentation**

- [ ] 80. Finalize Phase 8
  
  **AI Agent Must**:
  - Execute: `git add .`
  - Execute: `git commit -m "Phase 8: Automatic Heater Control - Complete"`
  - Execute: `git push origin phase-8-automatic-heater`
  - Execute: `git checkout main`
  - Execute: `git merge phase-8-automatic-heater`
  - Execute: `git push origin main`
  - Execute: `git branch -d phase-8-automatic-heater`
  - Create `docs/phase-8-automatic-heater.md` with full documentation
  - Execute: `git add docs/phase-8-automatic-heater.md`
  - Execute: `git commit -m "Document Phase 8 completion"`
  - Execute: `git push origin main`

**Phase 8 Complete** ✅


### Phase 9: Polish and Optimization

**Branch Name**: `phase-9-polish-optimization`

**Objective**: Final refinements and memory optimization on ESP8266

**AI Agent (Developer) Responsibilities**: Implement all tasks below following the 6-step protocol

**Step 1: Pre-Git Setup**
- AI agent executes: `git checkout main`
- AI agent executes: `git pull origin main`
- AI agent executes: `git status`
- AI agent executes: `git checkout -b phase-9-polish-optimization`

**Step 2-3: Codebase Analysis**
- AI agent reads all existing source files from Phases 1-8
- AI agent reads `docs/phase-8-automatic-heater.md` to understand Phase 8 results
- AI agent reviews entire codebase for optimization opportunities

**Step 4: Phase Execution**

- [ ] 81. Optimize memory usage across all files
  
  **AI Agent Must**:
  - Review all source files for string literals not in PROGMEM
  - Move remaining strings to PROGMEM using `F()` macro or PROGMEM arrays
  - Replace `int` with `uint8_t` or `uint16_t` where appropriate
  - Review global variables, minimize usage
  - Check for unused library features, remove if possible
  - _Requirements: 18.1-18.9_
  - AI agent executes: `git add .`
  - AI agent executes: `git commit -m "Optimize memory usage - PROGMEM strings and variable types"`

- [ ] 82. Improve display rendering with icons and formatting
  
  **AI Agent Must**:
  - In `src/display.cpp`, create water drop icon bitmap:
    ```cpp
    const uint8_t waterDropIcon[] PROGMEM = {
        0x04, 0x0E, 0x1F, 0x1F, 0x1F, 0x0E, 0x04
    };
    ```
  - Update `showIdleScreen()`: use water drop icon instead of text for water level
  - Improve menu item spacing: add consistent padding
  - Add visual separators: draw horizontal lines between sections
  - Optimize font usage: use appropriate text sizes for readability
  - _Requirements: 11.7, 24.2, 24.3_
  - AI agent executes: `git add src/display.cpp`
  - AI agent executes: `git commit -m "Improve display rendering with icons and better formatting"`

- [ ] 83. Enhance buzzer feedback patterns
  
  **AI Agent Must**:
  - In `src/input.cpp`, verify buzzer patterns:
    - Short beep (50ms) for button press and rotation - already implemented
    - Medium beep (200ms) for warnings - verify in safety.cpp
  - In `src/safety.cpp`, verify continuous pattern (1Hz) for fault state - already implemented
  - Add function to play different beep patterns:
    ```cpp
    void playBeepPattern(uint8_t pattern) {
        // pattern 0: short (50ms)
        // pattern 1: medium (200ms)
        // pattern 2: long (500ms)
    }
    ```
  - _Requirements: 16.2-16.5_
  - AI agent executes: `git add src/input.cpp src/safety.cpp`
  - AI agent executes: `git commit -m "Enhance buzzer feedback patterns"`

- [ ] 84. Add startup power-on self-test (POST)
  
  **AI Agent Must**:
  - In `src/main.cpp`, create function `bool performPOST()`:
    - Test OLED: already done in setup(), verify communication
    - Test PCF8574: call `initPCF8574()`, check return value
    - Test temperature sensor: call `tempSensor.begin()`, check device count
    - Display test results on OLED during POST
    - Return true if all tests pass, false if any fail
  - Call `performPOST()` in `setup()` after hardware initialization
  - If POST fails:
    - Display specific error message
    - Enter fault state
    - Sound continuous buzzer
  - If POST passes:
    - Sound single short beep
    - Display "POST OK" briefly
  - _Requirements: 28.1-28.6_
  - AI agent executes: `git add src/main.cpp`
  - AI agent executes: `git commit -m "Add power-on self-test with hardware verification"`

- [ ] 85. Verify graceful degradation (already implemented in Phase 8)
  
  **AI Agent Must**:
  - Verify `src/heater_control.cpp` handles sensor failure
  - Verify `src/display.cpp` shows "TEMP SENSOR FAULT" message
  - Verify manual actuator control still works with sensor failure
  - Add additional logging for sensor failures in `src/sensors.cpp`
  - _Requirements: 40.1-40.5_
  - AI agent executes: `git add src/sensors.cpp` (if modified)
  - AI agent executes: `git commit -m "Verify graceful degradation implementation"` (if modified)

- [ ] 86. Final code cleanup and documentation
  
  **AI Agent Must**:
  - Add function documentation comments to all public methods:
    ```cpp
    /**
     * @brief Initialize the sensor manager
     * Configures DS18B20 and water level sensor
     */
    void init();
    ```
  - Organize code: group related functions together
  - Remove any debug code or commented-out sections
  - Verify all constants are in `constants.h`
  - Ensure consistent code style: indentation, naming conventions
  - Add file header comments with purpose and author
  - _Requirements: 34.1-34.5_
  - AI agent executes: `git add .`
  - AI agent executes: `git commit -m "Final code cleanup and documentation"`

- [ ] 87. Request user to compile and check memory
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run` and provide the compilation output**"
  - Wait for user's compilation output, analyze for errors
  - Output: "**Please run: `pio run --target size` and provide the memory usage output**"
  - Wait for memory usage, verify within Phase 9 budget (~160 KB Flash, ~21 KB SRAM)

**Step 5: User Hardware Testing**

- [ ] 88. Request user hardware testing
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run --target upload` and provide the upload output**"
  - Wait for upload confirmation
  - Output: "**Please test the following on hardware and provide feedback:**"
  - Output test cases:
    ```
    Phase 9 Hardware Tests:
    1. Power on ESP8266 - verify POST runs and displays test results
    2. Verify single beep on successful POST
    3. Test POST failure:
       a. Disconnect OLED - verify POST fails with specific error
       b. Disconnect PCF8574 - verify POST fails with specific error
       c. Disconnect DS18B20 - verify POST fails with specific error
    4. With all hardware connected, verify all features work:
       a. Menu navigation smooth and responsive
       b. Display rendering clear with icons
       c. Buzzer patterns appropriate for each event
       d. All actuators controllable
       e. Safety interlocks functional
       f. Settings persist
       g. Auto heater control works
    5. Test graceful degradation:
       a. Disconnect temperature sensor during operation
       b. Verify manual actuator control still works
       c. Verify "TEMP SENSOR FAULT" displays
       d. Verify auto heater disables
    6. Check memory usage - verify within budget
    7. Check serial monitor - verify clean logging
    
    Please confirm:
    - [ ] POST runs successfully on startup
    - [ ] POST detects hardware failures
    - [ ] All features work correctly
    - [ ] Display rendering improved
    - [ ] Buzzer patterns appropriate
    - [ ] Graceful degradation works
    - [ ] Memory usage within budget
    - [ ] Any issues or errors observed?
    ```
  - Wait for user feedback
  - If issues: fix, recompile, upload, return to testing
  - If working: proceed to Step 6

**Step 6: Git Operations and Documentation**

- [ ] 89. Finalize Phase 9
  
  **AI Agent Must**:
  - Execute: `git add .`
  - Execute: `git commit -m "Phase 9: Polish and Optimization - Complete"`
  - Execute: `git push origin phase-9-polish-optimization`
  - Execute: `git checkout main`
  - Execute: `git merge phase-9-polish-optimization`
  - Execute: `git push origin main`
  - Execute: `git branch -d phase-9-polish-optimization`
  - Create `docs/phase-9-polish-optimization.md` with full documentation
  - Execute: `git add docs/phase-9-polish-optimization.md`
  - Execute: `git commit -m "Document Phase 9 completion"`
  - Execute: `git push origin main`

**Phase 9 Complete** ✅


### Phase 10: Documentation and Testing

**Branch Name**: `phase-10-documentation`

**Objective**: Complete documentation and comprehensive testing on ESP8266

**AI Agent (Developer) Responsibilities**: Implement all tasks below following the 6-step protocol

**Step 1: Pre-Git Setup**
- AI agent executes: `git checkout main`
- AI agent executes: `git pull origin main`
- AI agent executes: `git status`
- AI agent executes: `git checkout -b phase-10-documentation`

**Step 2-3: Codebase Analysis**
- AI agent reads all existing source files from Phases 1-9
- AI agent reads `docs/phase-9-polish-optimization.md` to understand Phase 9 results
- AI agent reviews entire project for documentation needs

**Step 4: Phase Execution**

- [ ] 90. Create hardware connection diagram
  
  **AI Agent Must**:
  - Create file `docs/hardware-connections.md`
  - Document ESP8266 pin connections:
    - I2C: SCL=D1(GPIO5), SDA=D2(GPIO4)
    - DS18B20: D5(GPIO14)
    - Buzzer: D6(GPIO12)
    - Encoder CLK: D7(GPIO13)
    - Encoder DT: D8(GPIO15) **with 10kΩ pull-down to GND**
    - Encoder SW: D3(GPIO0)
    - Water Level: D4(GPIO2) - simulated (GND=water present)
  - Document I2C addresses:
    - OLED: 0x3C
    - PCF8574: 0x20
  - Document power supply: 5V via USB or Vin pin
  - Include relay wiring: PCF8574 outputs to relay module inputs
  - Add warnings about GPIO15 pull-down requirement
  - _Requirements: 33.4_
  - AI agent executes: `git add docs/hardware-connections.md`
  - AI agent executes: `git commit -m "Create hardware connection diagram"`

- [ ] 91. Create menu navigation diagram
  
  **AI Agent Must**:
  - Create file `docs/menu-navigation.md`
  - Document menu hierarchy:
    ```
    IDLE SCREEN
    ├─ [Button Press] → MAIN MENU
       ├─ Circulation Pump → Toggle ON/OFF
       ├─ Massage Pump → Toggle ON/OFF
       ├─ Jet Pump → Toggle ON/OFF
       ├─ Heater → Toggle ON/OFF or AUTO mode
       ├─ Ozone → Toggle ON/OFF
       ├─ Speaker → Toggle ON/OFF
       ├─ Light → Toggle ON/OFF
       └─ Settings → SETTINGS MENU
          ├─ Target Temp → Adjust 30-40°C
          ├─ Idle Timeout → Adjust 10-120s
          └─ About → View system info
    ```
  - Document navigation controls:
    - Rotate encoder: navigate up/down
    - Press button: select/toggle
    - Idle 30s: return to idle screen
  - Document all menu functions
  - _Requirements: 33.5_
  - AI agent executes: `git add docs/menu-navigation.md`
  - AI agent executes: `git commit -m "Create menu navigation diagram"`

- [ ] 92. Create state machine diagram
  
  **AI Agent Must**:
  - Create file `docs/state-machine.md`
  - Document system states:
    - INIT: Power-on, POST, hardware initialization
    - IDLE: Display temperature and status
    - MENU: User navigating menus
    - ACTUATOR_CONTROL: Controlling specific actuator
    - SETTINGS: Adjusting settings
    - FAULT: Hardware fault detected
  - Document state transitions:
    - INIT → IDLE (POST success)
    - INIT → FAULT (POST failure)
    - IDLE → MENU (button press)
    - MENU → IDLE (timeout)
    - ANY → FAULT (thermal runaway, critical error)
  - Document safety interlock logic:
    - Water level interlock flowchart
    - Heater interlock flowchart
    - Thermal runaway detection flowchart
  - _Requirements: 33.6_
  - AI agent executes: `git add docs/state-machine.md`
  - AI agent executes: `git commit -m "Create state machine diagram"`

- [ ] 93. Update README.md with complete project information
  
  **AI Agent Must**:
  - Update or create `README.md` in project root
  - Add project overview:
    - Description: ESP8266-based jacuzzi/bathtub controller
    - Features: 7 actuator control, temperature regulation, safety interlocks
  - Add hardware requirements:
    - ESP8266 (ESP-12E/ESP-12F)
    - 128x64 OLED (SH1106, I2C)
    - PCF8574 I2C GPIO expander
    - 8-channel relay module
    - DS18B20 temperature sensor
    - Rotary encoder with button
    - Passive buzzer
    - Power supply (5V)
  - Add build instructions:
    ```bash
    # Install PlatformIO
    # Clone repository
    # Build project
    pio run
    # Upload to ESP8266
    pio run --target upload
    # Monitor serial output
    pio device monitor
    ```
  - Add usage instructions:
    - Power on, wait for POST
    - Navigate menus with encoder
    - Control actuators
    - Configure settings
  - Add troubleshooting section:
    - ESP8266 won't boot: check GPIO15 pull-down
    - OLED not working: check I2C connections
    - Relays not switching: check PCF8574 address
    - Temperature sensor error: check DS18B20 wiring
  - _Requirements: 33.3_
  - AI agent executes: `git add README.md`
  - AI agent executes: `git commit -m "Update README with complete project information"`

- [ ] 94. Verify phase documentation exists for all phases
  
  **AI Agent Must**:
  - Verify existence of phase documentation files:
    - `docs/phase-1-hardware-init.md`
    - `docs/phase-2-sensor-integration.md`
    - `docs/phase-3-rotary-encoder.md`
    - `docs/phase-4-menu-system.md`
    - `docs/phase-5-actuator-control.md`
    - `docs/phase-6-safety-interlocks.md`
    - `docs/phase-7-settings-eeprom.md`
    - `docs/phase-8-automatic-heater.md`
    - `docs/phase-9-polish-optimization.md`
  - Each file should contain:
    - Phase objectives
    - Implementation details
    - Testing procedure
    - Memory usage
    - Issues encountered and resolutions
  - If any files are missing, note them for user to create during testing
  - _Requirements: 21.4, 21.5, 33.1, 33.2_
  - AI agent executes: `git add docs/` (if any files created)
  - AI agent executes: `git commit -m "Verify phase documentation"` (if any files created)

- [ ] 95. Create comprehensive test plan document
  
  **AI Agent Must**:
  - Create file `docs/test-plan.md`
  - Document test categories:
    - Hardware initialization tests
    - Sensor reading tests
    - User input tests
    - Menu navigation tests
    - Actuator control tests
    - Safety interlock tests
    - Settings persistence tests
    - Automatic heater control tests
    - Fault handling tests
    - Memory usage tests
  - For each category, list specific test cases with expected results
  - Include test checklist format for user to complete
  - _Requirements: All requirements_
  - AI agent executes: `git add docs/test-plan.md`
  - AI agent executes: `git commit -m "Create comprehensive test plan"`

- [ ] 96. Request user to compile and check final memory usage
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run` and provide the compilation output**"
  - Wait for user's compilation output, analyze for errors
  - Output: "**Please run: `pio run --target size` and provide the memory usage output**"
  - Wait for memory usage, verify within final budget (~160 KB Flash, ~21 KB SRAM)
  - Document final memory usage in `docs/phase-10-documentation.md`

**Step 5: User Hardware Testing**

- [ ] 97. Request comprehensive system testing
  
  **AI Agent Must**:
  - Output: "**Please run: `pio run --target upload` and provide the upload output**"
  - Wait for upload confirmation
  - Output: "**Please perform comprehensive system testing and provide feedback:**"
  - Output test plan:
    ```
    Phase 10 Comprehensive System Tests:
    
    1. Hardware Initialization:
       - [ ] POST runs successfully
       - [ ] All hardware detected
       - [ ] Splash screen displays
       - [ ] Idle screen appears
    
    2. Sensor Reading:
       - [ ] Temperature displays correctly
       - [ ] Water level detection works
       - [ ] Sensor errors detected and handled
    
    3. User Input:
       - [ ] Encoder rotation smooth
       - [ ] Button press responsive
       - [ ] Buzzer feedback appropriate
    
    4. Menu Navigation:
       - [ ] All menus accessible
       - [ ] Menu wrapping works
       - [ ] Idle timeout functional
       - [ ] Display clear and readable
    
    5. Actuator Control:
       - [ ] All 7 actuators controllable
       - [ ] Relays switch correctly
       - [ ] Status indicators accurate
    
    6. Safety Interlocks:
       - [ ] Water level interlock prevents pump/heater
       - [ ] Heater interlock requires circulation pump
       - [ ] Thermal runaway triggers fault state
       - [ ] Fault state sticky (requires power cycle)
    
    7. Settings:
       - [ ] Target temperature adjustable
       - [ ] Idle timeout adjustable
       - [ ] Settings persist after power cycle
       - [ ] About screen shows correct info
    
    8. Automatic Heater Control:
       - [ ] Auto mode toggles correctly
       - [ ] Heater activates/deactivates based on temperature
       - [ ] Hysteresis prevents rapid cycling
       - [ ] Interlocks respected in auto mode
    
    9. Error Handling:
       - [ ] Sensor failures handled gracefully
       - [ ] I2C errors recovered
       - [ ] Fault state displays correctly
    
    10. Memory and Performance:
        - [ ] Memory usage within budget
        - [ ] No memory leaks observed
        - [ ] System responsive
        - [ ] No crashes or hangs
    
    Please confirm all tests pass and document any issues.
    ```
  - Wait for user's complete test results
  - If issues: fix, recompile, upload, return to testing
  - If all tests pass: proceed to Step 6

**Step 6: Git Operations and Documentation**

- [ ] 98. Finalize Phase 10 and project completion
  
  **AI Agent Must**:
  - Execute: `git add .`
  - Execute: `git commit -m "Phase 10: Documentation and Testing - Complete"`
  - Execute: `git push origin phase-10-documentation`
  - Execute: `git checkout main`
  - Execute: `git merge phase-10-documentation`
  - Execute: `git push origin main`
  - Execute: `git branch -d phase-10-documentation`
  - Create `docs/phase-10-documentation.md` with:
    - Phase objectives
    - Documentation created
    - Test results summary
    - Final memory usage
    - Project completion notes
  - Execute: `git add docs/phase-10-documentation.md`
  - Execute: `git commit -m "Document Phase 10 completion"`
  - Execute: `git push origin main`
  - Create final release tag: `git tag -a v1.0.0 -m "Jacuzzi Controller v1.0.0 - Initial Release"`
  - Execute: `git push origin v1.0.0`
  - Output: "**🎉 PROJECT COMPLETE! All 10 phases finished successfully. System ready for deployment.**"

**Phase 10 Complete** ✅
**PROJECT COMPLETE** 🎉


## Notes

- **No Automated Tests**: This is an embedded hardware project. All testing is manual on actual ESP8266 hardware.
- **Memory Tracking**: After each phase, user runs `pio run --target size` to verify memory usage.
- **Incremental Development**: Each phase builds on the previous phase. Do not skip phases.
- **Hardware Required**: ESP8266 (ESP-12E/ESP-12F), 128x64 OLED (SH1106), PCF8574, 8-channel relay module, DS18B20 sensor, rotary encoder, passive buzzer.
- **Serial Debug**: Enable `-DENABLE_SERIAL_DEBUG` in platformio.ini during development for diagnostic output.
- **Phase Documentation**: Each phase must be documented in docs/phase-<N>-<description>.md before moving to next phase.
- **Checkpoints**: Each checkpoint task ensures the phase is complete and working before proceeding.
- **Safety First**: Safety interlocks are critical. Test thoroughly in Phase 6.
- **PROGMEM Usage**: All constant strings must use PROGMEM to conserve SRAM.
- **Non-Blocking Code**: Never use delay(). Always use millis() for timing.
- **ESP8266 Specific**: GPIO15 (D8/Encoder DT) requires external 10kΩ pull-down resistor to GND for proper boot.
- **Git Commands**: AI agent executes all Git operations directly.
- **PlatformIO Commands**: AI agent MUST request user to run all `pio` commands and wait for output.

## Implementation Strategy

Each task should be implemented following this pattern:

1. **Read Context**: Review requirements and design documents for the specific feature
2. **Implement Code**: Write C++ code following Arduino/ESP8266 conventions
3. **Request Compile**: AI agent requests user to run `pio run`
4. **Request Upload**: AI agent requests user to run `pio run --target upload`
5. **Request Test**: AI agent provides detailed test checklist for user to perform on hardware
6. **Iterate**: If issues reported, fix and repeat compile/upload/test
7. **Measure**: AI agent requests user to run `pio run --target size` for memory verification
8. **Document**: AI agent creates phase documentation with results

## Memory Budget Verification

After each phase, user runs:

```bash
pio run --target size
```

Expected output format:
```
RAM:   [====      ]  XX.X% (used XXXX bytes from 81920 bytes)
Flash: [========  ]  XX.X% (used XXXXX bytes from 1044464 bytes)
```

If memory usage exceeds phase target, optimize before proceeding to next phase.

## Success Criteria

The implementation is complete when:

- ✅ All 98 tasks are completed (tasks 1-98)
- ✅ All 10 phases are complete
- ✅ All checkpoints pass
- ✅ Memory usage is within budget (≤160 KB Flash, ≤21 KB SRAM)
- ✅ All safety interlocks function correctly
- ✅ All menu navigation works smoothly
- ✅ All actuators can be controlled
- ✅ Automatic heater control maintains target temperature
- ✅ Settings persist across power cycles
- ✅ System recovers gracefully from sensor failures
- ✅ All documentation is complete
- ✅ Comprehensive system testing passes
- ✅ Project tagged as v1.0.0 release

---

**Ready to begin implementation!** Start with Phase 1, Task 1: Create constants.h with ESP8266 pin assignments and all system constants.

