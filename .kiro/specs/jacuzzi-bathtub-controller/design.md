# Design Document: Jacuzzi/Bathtub Controller System

## Overview

### Purpose

This design document specifies the technical architecture for an ESP8266-based embedded control system for a jacuzzi/bathtub. The system provides safe, user-friendly control of pumps, heating, ozone generation, lighting, and audio through an OLED menu interface with rotary encoder input. The design follows a strict 6-step development protocol with user hardware testing at each phase.

### Key Design Principles

1. **Safety First**: Multiple hardware and software interlocks prevent unsafe actuator combinations
2. **Memory Efficiency**: Efficient memory usage despite ESP8266's larger capacity (4 MB Flash, 80 KB SRAM)
3. **Non-Blocking Architecture**: All operations use millis()-based state machines for responsive behavior
4. **Single Source of Truth**: All constants centralized in constants.h with dynamic configuration support
5. **Fail-Safe Design**: All outputs default to OFF state; system enters safe state on critical errors
6. **Graceful Degradation**: System remains partially functional during non-critical sensor failures
7. **Dynamic Configuration**: All adjustable parameters defined as constants for easy modification without code changes
8. **Sensor Polarity Independence**: Code logic independent of sensor active-high/active-low configuration

### System Constraints

- **Hardware**: ESP8266 (ESP-12E/ESP-12F) - 80 MHz, 4 MB Flash, 80 KB SRAM, 4 KB EEPROM (emulated)
- **Real-Time Requirements**: Sensor monitoring every 500ms, display updates at 10 FPS minimum
- **Safety-Critical**: Water level and heater interlocks must respond within 100-500ms
- **Minimal Dynamic Allocation**: Prefer static allocation to prevent fragmentation
- **No Blocking Calls**: No delay() usage; all timing via millis()
- **Boot-Safe GPIO**: All pins selected to avoid ESP8266 boot issues

### Development Protocol

**CRITICAL**: All development follows the 6-Step Task Execution Protocol:
1. **Pre-Git Setup**: Branch creation, pull, clean state verification
2. **Deep Codebase Analysis**: Read all existing code and configurations
3. **Previous Phase Analysis**: Understand what was implemented before
4. **Phase Execution**: Implement without deviation, request PlatformIO commands from user
5. **User Hardware Review**: AI agent requests testing, waits for feedback, iterates on issues
6. **Git Operations**: Commit, push, merge with user executing commands

## Architecture

### High-Level Architecture

The system follows a layered architecture with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────┐
│                    Application Layer                     │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │ Menu System  │  │ Safety Logic │  │ Auto Control │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
└─────────────────────────────────────────────────────────┘
                          │
┌─────────────────────────────────────────────────────────┐
│                   Hardware Abstraction                   │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │   Sensors    │  │   Actuators  │  │   Display    │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
└─────────────────────────────────────────────────────────┘
                          │
┌─────────────────────────────────────────────────────────┐
│                      Driver Layer                        │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │   OneWire    │  │     I2C      │  │   GPIO       │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
└─────────────────────────────────────────────────────────┘
```

### Module Organization

The codebase is organized into the following modules:

1. **constants.h**: Single source of truth for all system constants
2. **sensors.h/cpp**: Temperature and water level sensor management
3. **actuators.h/cpp**: PCF8574-based relay control with safety interlocks
4. **display.h/cpp**: OLED display management and rendering
5. **menu.h/cpp**: Menu navigation and user interaction logic
6. **safety.h/cpp**: Safety interlock enforcement and fault state management
7. **settings.h/cpp**: EEPROM-based persistent configuration
8. **main.cpp**: Main loop coordination and initialization

### State Machine Architecture

The system implements multiple concurrent state machines using millis()-based timing:

**Main System States**:
- INITIALIZING: Power-on self-test and hardware verification
- IDLE: Default state showing status screen
- MENU_ACTIVE: User navigating menu system
- FAULT: Critical error state requiring power cycle

**Sensor State Machines**:
- Temperature reading: 2-second update cycle with retry logic
- Water level monitoring: 500ms polling with debouncing
- Encoder input: 50ms debouncing with edge detection

**Actuator State Machines**:
- Heater control: Hysteresis-based automatic temperature maintenance
- Buzzer patterns: Non-blocking tone generation for feedback
- Display updates: Frame-rate limited rendering (10 FPS minimum)

### Memory Architecture

**Flash Memory (4 MB available)**:
- Program code: ~100-200 KB (ample space for features)
- PROGMEM strings: ~5-10 KB
- PROGMEM constants: ~2 KB
- File system (SPIFFS/LittleFS): Optional, ~1-2 MB if needed
- OTA updates: Optional, requires ~1 MB reserved

**SRAM (80 KB available)**:
- Global state variables: ~1-2 KB
- Display buffer: ~1024 bytes (128x64 / 8)
- Stack: ~4 KB
- Heap: ~70 KB available for dynamic allocation (use sparingly)
- WiFi stack (if enabled): ~20-30 KB

**EEPROM (4 KB emulated in Flash)**:
- Target temperature: 4 bytes (float)
- Idle timeout: 2 bytes (uint16_t)
- Auto heater mode: 1 byte (bool)
- Settings checksum: 2 bytes (uint16_t)
- Reserved: ~4000 bytes for future use

## Components and Interfaces

### Hardware Components

#### Input Devices

**Rotary Encoder (KY-040)**:
- CLK (Pin D7 / GPIO13): Rotation detection, safe for ESP8266
- DT (Pin D8 / GPIO15): Direction detection, **requires external 10kΩ pull-down resistor to GND** (critical for boot)
- SW (Pin D3 / GPIO0): Button press with internal pull-up, safe for button input
- Debouncing: 50ms software debounce on all inputs
- Interface: Direct GPIO with edge detection
- **Boot Consideration**: GPIO15 must be LOW during boot (external pull-down ensures this)

**DS18B20 Temperature Sensor**:
- Data Pin: D5 (GPIO14) - safe pin, no boot interference
- Power: Parasitic or external 3.3V
- Resolution: 12-bit (0.0625°C precision, displayed as 0.1°C)
- Update Rate: Every 2 seconds
- Error Handling: Retry on CRC failure, detect -127°C and 85°C error codes
- Interface: OneWire library with DallasTemperature wrapper
- Pull-up: 4.7kΩ resistor to 3.3V

**Water Level Sensor (Simulated)**:
- Signal Pin: D4 (GPIO2) with internal pull-up enabled
- Logic: Pin connected to GND = Water present (Water_Level_OK=TRUE)
- Logic: Pin floating/HIGH = No water (Water_Level_OK=FALSE)
- Update Rate: Every 500ms
- Debouncing: 3-sample majority voting to prevent false triggers
- Interface: Direct GPIO digital read
- **Configuration**: WATER_LEVEL_ACTIVE_LOW constant in constants.h allows polarity inversion
- **Boot Consideration**: GPIO2 has internal pull-up, safe for this application

#### Output Devices

**PCF8574 I2C I/O Expander (Address 0x20)**:
- 8 relay outputs (active HIGH to energize relay)
- Connected to I2C bus: SCL=D1 (GPIO5), SDA=D2 (GPIO4)
- Bit assignments:
  - Bit 0: Circulation Pump
  - Bit 1: Massage Pump
  - Bit 2: Jet Pump
  - Bit 3: Heater
  - Bit 4: Ozone Generator
  - Bit 5: Speaker
  - Bit 6: Light
  - Bit 7: Reserved
- Initialization: All bits LOW (relays OFF)
- Error Handling: Retry I2C writes up to 3 times, enter fault state on persistent failure
- Interface: Wire library with custom PCF8574 wrapper

**SH1106 OLED Display (Address 0x3C)**:
- Resolution: 128x64 pixels, monochrome
- Interface: I2C (shared bus with PCF8574)
- Connected to: SCL=D1 (GPIO5), SDA=D2 (GPIO4)
- Frame Rate: Minimum 10 FPS during menu navigation
- Buffer: 1024 bytes in SRAM
- Fonts: Default Adafruit GFX fonts (stored in PROGMEM)
- Library: Adafruit SH110X
- **Initialization**: Wire.begin(SDA_PIN, SCL_PIN) with explicit pin specification

**Piezo Buzzer**:
- Pin: D6 (GPIO12) - safe pin, no boot interference
- Patterns:
  - Button press: 50ms beep
  - State change: 50ms beep
  - Warning: 200ms beep
  - Fault: 1Hz continuous (1s on, 1s off)
- Interface: Direct GPIO with non-blocking tone generation

### Software Interfaces

#### Sensor Interface

```cpp
// sensors.h
class SensorManager {
public:
    void init();
    void update();  // Call every loop iteration
    
    float getTemperature();
    bool isTemperatureValid();
    bool isWaterLevelOK();
    
    enum SensorStatus {
        OK,
        TEMP_SENSOR_ERROR,
        TEMP_SENSOR_DISCONNECTED,
        WATER_LEVEL_UNKNOWN
    };
    
    SensorStatus getStatus();
    
private:
    float lastValidTemp;
    uint32_t lastTempUpdate;
    uint8_t waterLevelSamples[3];
    uint8_t sampleIndex;
};
```

**Key Design Decisions**:
- Temperature readings cached with timestamp to avoid blocking OneWire operations
- Water level uses 3-sample majority voting to filter noise
- Sensor errors tracked separately from values to enable graceful degradation
- Non-blocking update() method called from main loop

#### Actuator Interface

```cpp
// actuators.h
class ActuatorManager {
public:
    void init();
    bool setState(uint8_t actuatorId, bool state);
    bool getState(uint8_t actuatorId);
    
    // Safety interlock checks
    bool canActivateHeater();
    bool canActivatePump(uint8_t pumpId);
    
    void emergencyShutdown();  // Deactivate all outputs
    
private:
    uint8_t relayState;  // Bit field for current relay states
    bool writeToExpander();
    uint8_t i2cRetryCount;
};
```

**Key Design Decisions**:
- Single byte tracks all relay states for atomic updates
- Safety checks performed before state changes
- I2C write failures trigger retry logic with exponential backoff
- Emergency shutdown bypasses normal state management

#### Display Interface

```cpp
// display.h
class DisplayManager {
public:
    void init();
    void update();  // Call every loop iteration
    
    void showSplashScreen();
    void showIdleScreen();
    void showMenu(uint8_t selectedIndex);
    void showActuatorControl(uint8_t actuatorId, bool state);
    void showError(const char* message);
    
    void setTemperature(float temp, bool valid);
    void setWaterLevel(bool ok);
    void setActuatorStates(uint8_t states);
    
private:
    uint32_t lastFrameTime;
    bool needsRedraw;
    uint8_t currentScreen;
};
```

**Key Design Decisions**:
- Frame rate limiting to reduce I2C bus contention
- Dirty flag system to avoid unnecessary redraws
- All strings stored in PROGMEM and loaded on-demand
- Status icons rendered from bitmap data in PROGMEM

#### Menu Interface

```cpp
// menu.h
class MenuManager {
public:
    void init();
    void update();  // Call every loop iteration
    
    void handleEncoderRotation(int8_t direction);
    void handleEncoderPress();
    
    uint8_t getCurrentScreen();
    uint8_t getSelectedItem();
    
private:
    uint8_t currentMenu;
    uint8_t selectedIndex;
    uint32_t lastInteractionTime;
    
    void returnToIdle();
    void navigateToSubmenu(uint8_t submenuId);
};
```

**Key Design Decisions**:
- Timeout tracking for automatic return to idle screen
- Hierarchical menu structure with parent/child relationships
- Encoder state machine handles debouncing and edge detection
- Menu actions trigger actuator state changes through ActuatorManager

#### Safety Interface

```cpp
// safety.h
class SafetyManager {
public:
    void init();
    void update();  // Call every loop iteration
    
    bool checkWaterLevelInterlock();
    bool checkHeaterInterlock();
    bool checkThermalRunaway();
    
    void enterFaultState(const char* reason);
    bool isInFaultState();
    
private:
    bool faultState;
    uint32_t faultTime;
    char faultReason[32];
};
```

**Key Design Decisions**:
- All safety checks called every loop iteration
- Fault state is sticky (requires power cycle to clear)
- Interlock violations logged to serial debug when enabled
- Safety checks have priority over user commands

## Data Models

### System State Structure

```cpp
struct SystemState {
    // Sensor data
    float currentTemp;
    bool tempValid;
    bool waterLevelOK;
    
    // Actuator states (bit field)
    uint8_t actuatorStates;
    
    // System status
    enum {
        STATE_INIT,
        STATE_IDLE,
        STATE_MENU,
        STATE_FAULT
    } systemState;
    
    // Menu state
    uint8_t currentMenu;
    uint8_t selectedItem;
    uint32_t lastInteraction;
    
    // Settings
    float targetTemp;
    uint16_t idleTimeout;
    bool autoHeaterMode;
    
    // Timing
    uint32_t lastTempRead;
    uint32_t lastWaterLevelRead;
    uint32_t lastDisplayUpdate;
};
```

**Memory Footprint**: ~40 bytes

### Actuator Bit Mapping

```cpp
// Bit positions in actuatorStates byte
#define ACTUATOR_CIRCULATION_PUMP  0
#define ACTUATOR_MASSAGE_PUMP      1
#define ACTUATOR_JET_PUMP          2
#define ACTUATOR_HEATER            3
#define ACTUATOR_OZONE             4
#define ACTUATOR_SPEAKER           5
#define ACTUATOR_LIGHT             6
#define ACTUATOR_RESERVED          7
```

### Menu Structure

```cpp
enum MenuId {
    MENU_IDLE = 0,
    MENU_MAIN,
    MENU_CIRCULATION_PUMP,
    MENU_MASSAGE_PUMP,
    MENU_JET_PUMP,
    MENU_HEATER,
    MENU_OZONE,
    MENU_SPEAKER,
    MENU_LIGHT,
    MENU_SETTINGS,
    MENU_TARGET_TEMP,
    MENU_IDLE_TIMEOUT,
    MENU_ABOUT
};

struct MenuItem {
    const char* label;  // Stored in PROGMEM
    MenuId submenu;
    uint8_t actuatorId;  // 0xFF if not an actuator control
};
```

### EEPROM Layout

```cpp
struct EEPROMSettings {
    uint16_t magic;           // 0xAA55 - validity marker
    float targetTemp;         // 30.0 - 40.0°C
    uint16_t idleTimeout;     // 10 - 120 seconds
    bool autoHeaterMode;      // true/false
    uint8_t reserved[10];     // Future expansion
    uint16_t checksum;        // CRC16 of above fields
};
```

**Address**: 0x0000-0x001F (32 bytes)
**Write Strategy**: Delayed write (1 second after last change) to minimize EEPROM wear

### Constants Organization

All constants defined in constants.h following this structure with dynamic configuration support:

```cpp
// ============================================================================
// PIN ASSIGNMENTS (ESP8266-specific)
// ============================================================================
// I2C pins
#define PIN_SDA            4   // D2 (GPIO4)
#define PIN_SCL            5   // D1 (GPIO5)

// Sensor pins
#define PIN_TEMP_SENSOR    14  // D5 (GPIO14) - safe, no boot issues
#define PIN_WATER_LEVEL    2   // D4 (GPIO2) - has pull-up, safe for active-low sensor

// Rotary encoder pins
#define PIN_ENCODER_CLK    13  // D7 (GPIO13) - safe
#define PIN_ENCODER_DT     15  // D8 (GPIO15) - REQUIRES external 10kΩ pull-down!
#define PIN_ENCODER_SW     0   // D3 (GPIO0) - has pull-up, safe for button

// Output pins
#define PIN_BUZZER         12  // D6 (GPIO12) - safe, no boot issues

// ============================================================================
// I2C ADDRESSES
// ============================================================================
#define I2C_OLED_ADDR      0x3C
#define I2C_PCF8574_ADDR   0x20

// ============================================================================
// SENSOR CONFIGURATION (Dynamic - allows polarity changes without code mods)
// ============================================================================
// Water level sensor polarity
// true = sensor is active-low (LOW when water present)
// false = sensor is active-high (HIGH when water present)
#define WATER_LEVEL_ACTIVE_LOW  true

// ============================================================================
// TIMING CONSTANTS (milliseconds)
// ============================================================================
#define TEMP_UPDATE_INTERVAL      2000   // Temperature reading interval
#define WATER_LEVEL_INTERVAL      500    // Water level check interval
#define DISPLAY_MIN_FRAME_TIME    100    // 10 FPS minimum
#define ENCODER_DEBOUNCE_TIME     50     // Encoder debounce delay
#define IDLE_TIMEOUT_DEFAULT      30000  // Default idle timeout (30 seconds)
#define BUZZER_BEEP_SHORT         50     // Short beep duration
#define BUZZER_BEEP_WARNING       200    // Warning beep duration

// ============================================================================
// TEMPERATURE CONFIGURATION (Dynamic - easy to adjust)
// ============================================================================
#define TEMP_MIN                  30.0f   // Minimum settable temperature (°C)
#define TEMP_MAX                  40.0f   // Maximum settable temperature (°C)
#define TEMP_INCREMENT            0.5f    // Temperature adjustment step (°C)
#define TEMP_THERMAL_RUNAWAY      45.0f   // Thermal runaway threshold (°C)
#define TEMP_HYSTERESIS           0.5f    // Hysteresis for auto control (°C)

// ============================================================================
// IDLE TIMEOUT CONFIGURATION (Dynamic - easy to adjust)
// ============================================================================
#define IDLE_TIMEOUT_MIN          10      // Minimum timeout (seconds)
#define IDLE_TIMEOUT_MAX          120     // Maximum timeout (seconds)
#define IDLE_TIMEOUT_STEP         10      // Timeout adjustment step (seconds)

// ============================================================================
// SAFETY TIMING (milliseconds)
// ============================================================================
#define HEATER_INTERLOCK_RESPONSE_MS  100   // Max heater interlock response time
#define WATER_LEVEL_INTERLOCK_MS      100   // Max water level interlock response
#define THERMAL_RUNAWAY_CHECK_MS      500   // Thermal runaway check interval

// ============================================================================
// MENU STRINGS (PROGMEM)
// ============================================================================
const char STR_CIRCULATION_PUMP[] PROGMEM = "Circulation Pump";
const char STR_MASSAGE_PUMP[] PROGMEM = "Massage Pump";
const char STR_JET_PUMP[] PROGMEM = "Jet Pump";
const char STR_HEATER[] PROGMEM = "Heater";
const char STR_OZONE[] PROGMEM = "Ozone";
const char STR_SPEAKER[] PROGMEM = "Speaker";
const char STR_LIGHT[] PROGMEM = "Light";
const char STR_SETTINGS[] PROGMEM = "Settings";
const char STR_TARGET_TEMP[] PROGMEM = "Target Temp";
const char STR_IDLE_TIMEOUT[] PROGMEM = "Idle Timeout";
const char STR_ABOUT[] PROGMEM = "About";
// ... etc

// ============================================================================
// ACTUATOR BIT POSITIONS
// ============================================================================
#define ACTUATOR_CIRCULATION_PUMP  0
#define ACTUATOR_MASSAGE_PUMP      1
#define ACTUATOR_JET_PUMP          2
#define ACTUATOR_HEATER            3
#define ACTUATOR_OZONE             4
#define ACTUATOR_SPEAKER           5
#define ACTUATOR_LIGHT             6
#define ACTUATOR_RESERVED          7
```

**Key Design Decision**: All adjustable parameters (temperature ranges, increments, timeouts, sensor polarity) are defined as constants. This allows easy configuration changes without modifying code logic, ensuring the system remains flexible and maintainable.

## Error Handling

### Error Categories

**Critical Errors (Enter Fault State)**:
1. Thermal runaway (temperature > 45°C)
2. Persistent I2C communication failure (3 consecutive failures)
3. Hardware initialization failure during POST

**Recoverable Errors (Display Warning, Continue Operation)**:
1. Transient temperature sensor read failure
2. Single I2C communication failure
3. Temperature sensor disconnection (allow manual control)

**User Errors (Display Message, Sound Buzzer)**:
1. Attempting heater activation without circulation pump
2. Attempting pump activation without water level OK
3. Invalid menu navigation

### Error Recovery Strategies

**Temperature Sensor Failures**:
- Retry read operation after 1 second
- After 3 consecutive failures, mark sensor as invalid
- Display "TEMP ERROR" but allow manual actuator control
- Disable automatic heater control
- Resume normal operation when sensor recovers

**I2C Communication Failures**:
- Retry operation immediately (up to 3 attempts)
- Use exponential backoff: 10ms, 50ms, 100ms
- After 3 failures, enter fault state
- Log failure details to serial debug

**Water Level Sensor Failures**:
- Use 3-sample majority voting to filter noise
- If sensor stuck LOW, allow operation with user confirmation
- Display warning icon on all screens

**Fault State Behavior**:
- Deactivate all PCF8574 outputs immediately
- Display fault reason on OLED
- Sound buzzer continuously (1Hz pattern)
- Ignore all user input
- Require power cycle to exit

### Diagnostic Logging

When ENABLE_SERIAL_DEBUG is defined:

```cpp
#ifdef ENABLE_SERIAL_DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif
```

**Logged Events**:
- System initialization steps
- Sensor read failures and retries
- I2C communication errors
- Safety interlock violations
- Actuator state changes
- Menu navigation events
- Fault state entries

## Testing Strategy

### Manual Hardware Testing Approach

**CRITICAL**: This system uses **manual hardware testing only**. No automated tests will be implemented. All testing is performed on actual ESP8266 hardware by the user.

### Test Execution Protocol

**Phase-Based Testing** (follows 6-Step Task Execution Protocol):

Each development phase includes:
1. **Pre-Git Setup**: AI agent MUST apply the step 1 of the Task Execution Protocol
2. **Deep Codebase Analysis**: AI agent reads all existing code
3. **Previous Phase Analysis**: AI agent understands what was implemented before
4. **Phase Execution**: 
   - AI agent implements code
   - AI agent requests user to run: `pio run` (compilation)
   - AI agent waits for compilation output
   - AI agent requests user to run: `pio run --target upload` (upload to ESP8266)
   - AI agent waits for upload confirmation
   - AI agent requests user to run: `pio run --target size` (memory check)
   - AI agent waits for memory usage output
5. **User Hardware Review and Feedback**:
   - AI agent explicitly asks: "**Please test the following on hardware and provide feedback:**"
   - AI agent lists specific test cases for current phase
   - User tests on actual hardware
   - User provides feedback (working / issues found)
   - If issues: AI agent iterates and fixes, returns to step 4
   - If working: proceed to step 6
6. **Git Operations**:
   - AI agent requests user to run git commands (add, commit, push, merge)
   - AI agent documents phase completion

### Test Categories

**Hardware Integration Tests** (Manual):
1. Verify OLED display initialization and rendering
2. Verify PCF8574 relay control (listen for relay clicks or observe LED indicators)
3. Verify DS18B20 temperature reading (compare with known temperature)
4. Verify water level sensor reading (connect pin to GND, observe display)
5. Verify rotary encoder input (rotate and press, observe menu changes)
6. Verify buzzer output (listen for beeps)

**Safety Interlock Tests** (Manual):
1. Water level interlock prevents pump activation (disconnect GND, try to activate pump)
2. Water level interlock prevents heater activation (disconnect GND, try to activate heater)
3. Heater interlock requires circulation pump (try to activate heater without pump)
4. Thermal runaway triggers fault state (simulate high temperature if possible)
5. Fault state deactivates all outputs (verify all relays turn OFF)
6. Interlock response time < 100ms (observe with oscilloscope if available)

**Functional Tests** (Manual):
1. Menu navigation with rotary encoder (navigate through all menus)
2. Actuator control through menu (toggle each actuator ON/OFF)
3. Automatic heater control with hysteresis (set target temp, observe heater cycling)
4. Settings persistence in EEPROM (change settings, power cycle, verify retained)
5. Idle timeout and screen transitions (wait for timeout, verify return to idle)
6. Error message display (disconnect sensor, verify error message)

**Memory Tests** (via PlatformIO):
1. Flash usage check: `pio run --target size`
2. SRAM usage check: `pio run --target size`
3. Verify no dynamic memory allocation issues (monitor serial debug for heap fragmentation)
4. No stack overflow under normal operation (monitor for crashes/resets)

**Timing Tests** (Manual with Serial Debug):
1. Main loop iteration time < 50ms (enable serial debug, observe loop timing)
2. Display update rate >= 10 FPS (observe smooth menu navigation)
3. Sensor update intervals maintained (observe serial debug timestamps)
4. Non-blocking operation verified (system remains responsive during all operations)

### AI Agent Testing Instructions

**When AI agent reaches Step 5 of any phase:**

1. AI agent MUST output: "**Please test the following on hardware and provide feedback:**"
2. AI agent MUST list specific test cases for the current phase
3. AI agent MUST wait for user's test results
4. If user reports issues:
   - AI agent analyzes the issue
   - AI agent fixes the code
   - AI agent requests recompilation and upload
   - AI agent returns to testing (repeat until working)
5. If user confirms working:
   - AI agent proceeds to Step 6 (Git operations)

**Example Test Request Format:**
```
**Please test the following on hardware and provide feedback:**

Phase 1 Tests:
1. Power on ESP8266 - verify OLED displays splash screen
2. Wait 2 seconds - verify "READY" message appears
3. Check serial monitor (115200 baud) - verify initialization messages
4. Observe relay module - verify all relays are OFF (no clicks)

Please confirm:
- [ ] Splash screen displays correctly
- [ ] "READY" message appears after 2 seconds
- [ ] All relays are OFF on startup
- [ ] Any issues or errors observed?
```

### Continuous Monitoring

- Monitor serial debug output during operation (when ENABLE_SERIAL_DEBUG is defined)
- Track memory usage after each phase using `pio run --target size`
- Verify timing constraints with serial debug timestamps
- Test safety interlocks with simulated failures (disconnect sensors, etc.)

### Property-Based Testing Applicability

**Assessment**: Property-based testing (PBT) is **NOT applicable** to this embedded system design.

**Rationale**:

This jacuzzi controller is an embedded hardware control system with the following characteristics that make property-based testing inappropriate:

1. **Hardware-Dependent I/O**: The system's primary function is interfacing with physical hardware (I2C devices, GPIO sensors, relays). These operations cannot be meaningfully tested with generated inputs—they require actual hardware.

2. **State Machine Control Logic**: The system is fundamentally a state machine that coordinates multiple hardware devices with safety interlocks. The behavior is discrete and state-dependent rather than having universal properties.

3. **Real-Time Safety Requirements**: Safety interlocks (water level, heater control, thermal runaway) must be verified with specific timing constraints on actual hardware.

4. **Limited Pure Functions**: Most code involves side effects (I2C writes, GPIO reads, display updates). The few pure functions (temperature calculations, bit manipulation) are simple enough that manual testing provides complete coverage.

5. **Platform Constraints**: Manual hardware testing is more appropriate and reliable for this safety-critical embedded system.

**Testing Approach**: Manual hardware testing with user feedback at each development phase, following the 6-step protocol.

## Implementation Phases

**CRITICAL**: Each phase follows the 6-Step Task Execution Protocol. AI agent must request user to run PlatformIO commands and wait for output before proceeding.

### Phase 1: Basic Hardware Initialization
**Objective**: Verify hardware communication and display basic status

**6-Step Protocol**:
- Step 1: Create branch `phase-1-hardware-init`
- Step 2-3: Analyze codebase (first phase, no previous code)
- Step 4: Implement, request `pio run`, `pio run --target upload`, `pio run --target size`
- Step 5: Request user hardware testing (splash screen, READY message, relays OFF)
- Step 6: Git commit and merge

**Deliverables**:
- Initialize I2C bus with Wire.begin(SDA_PIN, SCL_PIN)
- Initialize OLED display at 0x3C
- Initialize PCF8574 with all outputs OFF
- Display splash screen
- Display "READY" message

**Memory Estimate**: ~50 KB Flash, ~10 KB SRAM (ample headroom)

### Phase 2: Sensor Integration
**Objective**: Read and display sensor data

**6-Step Protocol**:
- Step 1: Create branch `phase-2-sensor-integration`
- Step 2-3: Analyze existing code and Phase 1 results
- Step 4: Implement, request PlatformIO commands
- Step 5: Request user testing (temperature reading, water level with GND connection)
- Step 6: Git commit and merge

**Deliverables**:
- Initialize DS18B20 temperature sensor on GPIO14
- Initialize water level sensor on GPIO2 with pull-up
- Read sensors in non-blocking manner
- Display temperature and water level status
- Implement sensor error detection

**Memory Estimate**: ~70 KB Flash, ~12 KB SRAM

### Phase 3: Rotary Encoder Input
**Objective**: Implement user input handling

**6-Step Protocol**:
- Step 1: Create branch `phase-3-rotary-encoder`
- Step 2-3: Analyze existing code and Phase 2 results
- Step 4: Implement, request PlatformIO commands
- Step 5: Request user testing (rotate encoder, press button, verify buzzer)
- Step 6: Git commit and merge

**Deliverables**:
- Initialize rotary encoder pins (GPIO13, GPIO15, GPIO0)
- Implement debouncing
- Detect rotation direction
- Detect button press
- Add buzzer feedback on GPIO12

**Memory Estimate**: ~80 KB Flash, ~13 KB SRAM

### Phase 4: Basic Menu System
**Objective**: Implement menu navigation

**6-Step Protocol**:
- Step 1: Create branch `phase-4-menu-system`
- Step 2-3: Analyze existing code and Phase 3 results
- Step 4: Implement, request PlatformIO commands
- Step 5: Request user testing (navigate menus, verify timeout, check display)
- Step 6: Git commit and merge

**Deliverables**:
- Main menu structure with PROGMEM strings
- Menu item selection
- Menu rendering
- Idle timeout using IDLE_TIMEOUT_DEFAULT
- Return to idle screen

**Memory Estimate**: ~100 KB Flash, ~15 KB SRAM

### Phase 5: Actuator Control
**Objective**: Control relays through menu

**6-Step Protocol**:
- Step 1: Create branch `phase-5-actuator-control`
- Step 2-3: Analyze existing code and Phase 4 results
- Step 4: Implement, request PlatformIO commands
- Step 5: Request user testing (toggle each relay, listen for clicks, verify status display)
- Step 6: Git commit and merge

**Deliverables**:
- PCF8574 relay control via I2C
- Individual actuator on/off
- Actuator status display on idle screen
- State persistence during navigation

**Memory Estimate**: ~110 KB Flash, ~16 KB SRAM

### Phase 6: Safety Interlocks
**Objective**: Implement all safety features

**6-Step Protocol**:
- Step 1: Create branch `phase-6-safety-interlocks`
- Step 2-3: Analyze existing code and Phase 5 results
- Step 4: Implement, request PlatformIO commands
- Step 5: Request user testing (test all interlocks, verify fault state, check response times)
- Step 6: Git commit and merge

**Deliverables**:
- Water level interlock (using WATER_LEVEL_ACTIVE_LOW constant)
- Heater interlock
- Thermal runaway protection (using TEMP_THERMAL_RUNAWAY constant)
- Fault state management
- Emergency shutdown

**Memory Estimate**: ~130 KB Flash, ~18 KB SRAM

### Phase 7: Settings and EEPROM
**Objective**: Persistent configuration

**6-Step Protocol**:
- Step 1: Create branch `phase-7-settings-eeprom`
- Step 2-3: Analyze existing code and Phase 6 results
- Step 4: Implement, request PlatformIO commands
- Step 5: Request user testing (change settings, power cycle, verify persistence)
- Step 6: Git commit and merge

**Deliverables**:
- Settings menu
- Target temperature setting (using TEMP_MIN, TEMP_MAX, TEMP_INCREMENT)
- Idle timeout setting (using IDLE_TIMEOUT_MIN, IDLE_TIMEOUT_MAX, IDLE_TIMEOUT_STEP)
- EEPROM read/write (ESP8266 EEPROM emulation)
- Settings validation

**Memory Estimate**: ~140 KB Flash, ~19 KB SRAM

### Phase 8: Automatic Heater Control
**Objective**: Temperature regulation

**6-Step Protocol**:
- Step 1: Create branch `phase-8-automatic-heater`
- Step 2-3: Analyze existing code and Phase 7 results
- Step 4: Implement, request PlatformIO commands
- Step 5: Request user testing (enable auto mode, verify hysteresis control)
- Step 6: Git commit and merge

**Deliverables**:
- Automatic heater mode
- Hysteresis control (using TEMP_HYSTERESIS constant)
- Manual/auto mode toggle
- Target temperature display

**Memory Estimate**: ~150 KB Flash, ~20 KB SRAM

### Phase 9: Polish and Optimization
**Objective**: Final refinements

**6-Step Protocol**:
- Step 1: Create branch `phase-9-polish-optimization`
- Step 2-3: Analyze existing code and Phase 8 results
- Step 4: Implement, request PlatformIO commands
- Step 5: Request user testing (verify all features, check POST, test graceful degradation)
- Step 6: Git commit and merge

**Deliverables**:
- About screen with memory usage
- Startup splash screen improvements
- Icon improvements
- Code cleanup and optimization
- Power-on self-test (POST)

**Memory Estimate**: ~160 KB Flash, ~21 KB SRAM

### Phase 10: Documentation and Testing
**Objective**: Complete documentation

**6-Step Protocol**:
- Step 1: Create branch `phase-10-documentation`
- Step 2-3: Analyze all previous phases
- Step 4: Create documentation
- Step 5: Request user to review documentation and perform comprehensive testing
- Step 6: Git commit and merge

**Deliverables**:
- Hardware connection diagram (with ESP8266 pin mappings)
- Menu navigation diagram
- State machine diagram
- Comprehensive testing documentation
- Phase completion summaries

**Memory Estimate**: ~160 KB Flash, ~21 KB SRAM (final)


## Correctness Properties

**Property-Based Testing Assessment**: Property-based testing is **NOT applicable** to this embedded system design.

**Rationale**:

This jacuzzi controller is an embedded hardware control system with the following characteristics that make property-based testing inappropriate:

1. **Hardware-Dependent I/O**: The system's primary function is interfacing with physical hardware (I2C devices, GPIO sensors, relays). These operations cannot be meaningfully tested with generated inputs—they require actual hardware or complex mocking that defeats the purpose of PBT.

2. **State Machine Control Logic**: The system is fundamentally a state machine that coordinates multiple hardware devices with safety interlocks. The behavior is discrete and state-dependent rather than having universal properties that hold across arbitrary inputs.

3. **Real-Time Safety Requirements**: Safety interlocks (water level, heater control, thermal runaway) must be verified with specific timing constraints on actual hardware. Property-based testing cannot validate real-time behavior or hardware response times.

4. **Limited Pure Functions**: Most code involves side effects (I2C writes, GPIO reads, display updates). The few pure functions (temperature calculations, bit manipulation) are simple enough that example-based tests provide complete coverage.

5. **Platform Constraints**: The Arduino platform lacks mature property-based testing frameworks, and the 2 KB SRAM constraint makes it impractical to include PBT libraries.

**Testing Approach Instead**:

- **Example-Based Unit Tests**: Test specific scenarios with concrete inputs
  - "Heater activation blocked when circulation pump is OFF"
  - "Water level interlock prevents all pumps when level is LOW"
  - "Temperature reading of 85°C triggers error handling"
  - "Menu wraps from last item to first item"

- **Integration Tests on Hardware**: Verify complete workflows
  - Navigate menu and activate each actuator
  - Trigger each safety interlock and verify response
  - Test sensor failure recovery
  - Verify EEPROM persistence across power cycles

- **Safety Verification Tests**: Exhaustively test all safety interlock combinations
  - All combinations of water level (OK/LOW) × pump states (ON/OFF)
  - Heater interlock with circulation pump (ON/OFF) × water level (OK/LOW)
  - Thermal runaway at various temperature thresholds

- **Timing Verification**: Use oscilloscope/logic analyzer to verify
  - Interlock response times < 100ms
  - Sensor update intervals
  - Display frame rates
  - Non-blocking operation

**Conclusion**: This design requires hardware integration testing and example-based unit tests rather than property-based testing. The discrete, hardware-dependent nature of the system makes PBT unsuitable.

## Security Considerations

### Physical Security

**Relay Control**:
- All relays default to OFF state on power-up
- Fault state immediately deactivates all relays
- No remote control interface (local control only)

**Electrical Safety**:
- 5V logic isolated from high-voltage relay circuits
- Heater interlock prevents dry-fire conditions
- Thermal runaway protection prevents overheating

### Software Security

**Memory Safety**:
- No dynamic memory allocation (prevents heap corruption)
- All buffers statically sized (prevents overflow)
- Stack usage monitored and limited

**Input Validation**:
- Temperature settings bounded to 30-40°C range
- Timeout settings bounded to 10-120 seconds
- EEPROM settings validated with checksum

**Fault Injection Resistance**:
- I2C communication errors trigger retry logic
- Sensor failures detected and handled gracefully
- Invalid EEPROM data replaced with safe defaults

### Safety-Critical Design

**Multiple Layers of Protection**:
1. **Hardware**: Water level sensor physically prevents operation without water
2. **Software**: Interlocks enforce safety rules in code
3. **Fail-Safe**: All outputs default to OFF on any error

**Interlock Priority**:
- Safety checks execute every loop iteration
- Safety checks have priority over user commands
- Fault state is sticky (requires power cycle)

**Timing Guarantees**:
- Water level interlock response: < 100ms
- Heater interlock response: < 100ms
- Thermal runaway detection: < 500ms

## Performance Considerations

### Memory Optimization Techniques

**Flash Memory**:
- All constant strings stored in PROGMEM using F() macro
- Menu strings loaded on-demand from PROGMEM
- Compiler optimization flags: -Os (size) and -flto (link-time optimization)
- Unused library features excluded via build flags

**SRAM Optimization**:
- Use smallest data types: uint8_t, uint16_t instead of int
- Bit fields for boolean flags
- Single display buffer shared across all screens
- Minimize global variables
- Reuse local variables across functions

**Code Size Reduction**:
- Inline small functions
- Avoid C++ virtual functions and RTTI
- Use lookup tables instead of switch statements where beneficial
- Share common code paths

### Timing Optimization

**Non-Blocking Architecture**:
- All operations use millis() for timing
- No delay() calls anywhere in code
- State machines for time-dependent operations
- Sensor reads cached with timestamps

**I2C Bus Management**:
- Minimize I2C transactions
- Batch PCF8574 writes (single byte updates all relays)
- Frame-rate limit display updates to reduce bus contention
- Error recovery with exponential backoff

**Main Loop Performance**:
- Target: < 50ms per iteration
- Sensor updates: Only when interval elapsed
- Display updates: Only when content changes
- Menu updates: Only on user input

### Real-Time Guarantees

**Safety-Critical Timing**:
- Water level checked every 500ms
- Heater interlock enforced within 100ms of pump state change
- Thermal runaway checked every 500ms
- Fault state entered within 500ms of critical error

**User Interface Responsiveness**:
- Encoder input processed every loop iteration
- Display updates within 50ms of state change
- Buzzer feedback within 50ms of user action
- Menu navigation feels immediate (< 100ms latency)

## Deployment Considerations

### Hardware Setup

**Required Components**:
- ESP8266 module (ESP-12E or ESP-12F)
- 1.3" SH1106 OLED display (I2C, 0x3C)
- PCF8574 I2C I/O expander (0x20)
- 8-channel 5V relay module
- DS18B20 temperature sensor
- Piezo buzzer
- KY-040 rotary encoder
- 4.7kΩ pull-up resistor (OneWire to 3.3V)
- 10kΩ pull-down resistor (GPIO15 to GND) - **CRITICAL for boot**
- Power supply (3.3V, 500mA minimum for ESP8266)
- 5V power supply for relay module (separate from ESP8266)

**Wiring**:
- **I2C bus**: SDA=D2 (GPIO4), SCL=D1 (GPIO5) shared between OLED and PCF8574
- **OneWire**: D5 (GPIO14) with 4.7kΩ pull-up to 3.3V
- **Water level**: D4 (GPIO2) with internal pull-up (connect to GND to simulate water present)
- **Encoder CLK**: D7 (GPIO13)
- **Encoder DT**: D8 (GPIO15) with **10kΩ pull-down to GND** (required for boot)
- **Encoder SW**: D3 (GPIO0) with internal pull-up
- **Buzzer**: D6 (GPIO12) with current-limiting resistor

**CRITICAL Boot Requirements**:
- GPIO15 (D8) MUST have external 10kΩ pull-down resistor to GND
- GPIO0 (D3) has internal pull-up, safe for button (pulled HIGH during boot)
- GPIO2 (D4) has internal pull-up, safe for water level sensor
- Do NOT connect anything that pulls GPIO0 LOW during boot (prevents programming)

**Relay Connections**:
- PCF8574 outputs drive relay module inputs
- Relay module powered separately (5V, NOT from ESP8266)
- High-voltage loads connected to relay N.O. terminals
- Heater uses 30A relay (separate from 8-channel module)
- **IMPORTANT**: ESP8266 GPIO outputs are 3.3V - ensure relay module is compatible or use level shifter

### Software Deployment

**Build Configuration**:
```ini
[env:esp12e]
platform = espressif8266
board = esp12e
framework = arduino
lib_deps =
    adafruit/Adafruit SH110X@^2.1.14
    adafruit/Adafruit GFX Library@^1.11.0
    adafruit/Adafruit BusIO@^1.14.0
    paulstoffregen/OneWire@^2.3.7
    milesburton/DallasTemperature@^3.11.0
build_flags =
    -Os
    -flto
    -DENABLE_SERIAL_DEBUG  ; Remove for production
monitor_speed = 115200
upload_speed = 921600
```

**Upload Process** (AI agent requests user to run these):
1. Connect ESP8266 via USB (using USB-to-Serial adapter or NodeMCU board)
2. AI agent requests: "Please run: `pio run`" (build project)
3. User provides compilation output
4. AI agent requests: "Please run: `pio run --target upload`" (upload to ESP8266)
5. User provides upload output
6. AI agent requests: "Please run: `pio device monitor`" (monitor serial output at 115200 baud)
7. User provides serial monitor output

**Production Build**:
- Remove `-DENABLE_SERIAL_DEBUG` flag
- AI agent requests: "Please run: `pio run --target size`"
- User provides memory usage output
- Test all functionality on hardware
- Document final memory usage

### Calibration and Configuration

**Temperature Sensor Calibration**:
- DS18B20 is factory calibrated (±0.5°C accuracy)
- Verify reading against known reference
- Adjust TEMP_OFFSET constant in constants.h if systematic offset detected

**Water Level Sensor Configuration**:
- Current implementation: Simulated sensor (pin to GND = water present)
- To use actual XKC-Y25-V sensor: Verify sensor output polarity
- Adjust WATER_LEVEL_ACTIVE_LOW constant in constants.h if needed:
  - `true` = sensor outputs LOW when water present
  - `false` = sensor outputs HIGH when water present
- Code logic automatically adapts to polarity setting

**Display Contrast**:
- Adjust via software if needed: `display.setContrast(value)`
- Default value usually optimal for SH1106

**EEPROM Initialization**:
- First boot loads default settings from constants.h
- Settings menu allows user customization
- Settings persist across power cycles using ESP8266 EEPROM emulation

**Dynamic Configuration**:
- All adjustable parameters in constants.h:
  - Temperature range: TEMP_MIN, TEMP_MAX, TEMP_INCREMENT
  - Timeout range: IDLE_TIMEOUT_MIN, IDLE_TIMEOUT_MAX, IDLE_TIMEOUT_STEP
  - Hysteresis: TEMP_HYSTERESIS
  - Sensor polarity: WATER_LEVEL_ACTIVE_LOW
- Modify constants.h and recompile to change behavior (no code changes needed)

### Maintenance and Troubleshooting

**Common Issues**:

1. **ESP8266 won't boot / stuck in boot loop**:
   - **CRITICAL**: Verify GPIO15 (D8) has 10kΩ pull-down resistor to GND
   - Check GPIO0 (D3) is not pulled LOW during boot
   - Verify power supply provides stable 3.3V with sufficient current

2. **Display not working**:
   - Check I2C address (0x3C vs 0x3D)
   - Verify I2C connections: SDA=D2 (GPIO4), SCL=D1 (GPIO5)
   - Ensure Wire.begin(SDA_PIN, SCL_PIN) is called with correct pins
   - Run I2C scanner sketch

3. **Temperature reads -127°C or 85°C**:
   - Check OneWire connection on D5 (GPIO14)
   - Verify 4.7kΩ pull-up resistor to 3.3V (NOT 5V)
   - Check sensor power supply (3.3V)

4. **Relays not switching**:
   - Verify PCF8574 I2C address (0x20)
   - Check relay module power supply (5V, separate from ESP8266)
   - Verify PCF8574 outputs are compatible with relay module inputs (may need level shifter for 5V relays)
   - Test PCF8574 with I2C scanner

5. **Encoder not responding**:
   - Check pin connections: CLK=D7 (GPIO13), DT=D8 (GPIO15), SW=D3 (GPIO0)
   - Verify GPIO15 has external 10kΩ pull-down
   - Verify GPIO0 has internal pull-up enabled
   - Test with multimeter

**Diagnostic Mode**:
- Enable serial debug: Add `-DENABLE_SERIAL_DEBUG` to build flags
- AI agent requests: "Please run: `pio device monitor`"
- Monitor serial output at 115200 baud
- Observe sensor readings, I2C transactions, state changes

**Memory Monitoring**:
```bash
# AI agent requests user to run:
pio run --target size

# Expected output (example):
# RAM:   [==        ]  20.0% (used 16384 bytes from 81920 bytes)
# Flash: [====      ]  40.0% (used 409600 bytes from 1024000 bytes)
```

### Future Enhancements

**Potential Additions** (ESP8266 has sufficient memory):

1. **WiFi Connectivity** (ESP8266 native capability):
   - Remote monitoring via web interface
   - Mobile app control via REST API
   - MQTT integration for home automation
   - OTA (Over-The-Air) firmware updates

2. **Advanced Scheduling**:
   - Timer-based pump activation
   - Scheduled heating cycles
   - Ozone generation schedules
   - NTP time synchronization

3. **Data Logging**:
   - Temperature history logging
   - Runtime statistics (pump hours, heating cycles)
   - Error history log
   - Export data via WiFi

4. **Enhanced Diagnostics**:
   - Web-based diagnostic dashboard
   - Real-time sensor graphs
   - Maintenance reminders
   - Email/push notifications for faults

5. **User Profiles**:
   - Multiple saved temperature presets
   - User-specific preferences
   - Quick-access favorites
   - Cloud backup of settings

**Memory Availability**:
Current design uses ~160 KB Flash and ~21 KB SRAM, leaving significant room for enhancements. ESP8266 provides 4 MB Flash and 80 KB SRAM, allowing for substantial feature additions without hardware upgrade.-channel module)

### Software Deployment

**Build Configuration**:
```ini
[env:esp12e]
platform = espressif8266
board = esp12e
framework = arduino
lib_deps =
    adafruit/Adafruit SH110X@^2.1.14
    adafruit/Adafruit BusIO@^1.14.0
    paulstoffregen/OneWire@^2.3.7
    milesburton/DallasTemperature@^3.11.0
build_flags =
    -Os
    -flto
    -DENABLE_SERIAL_DEBUG  ; Remove for production
monitor_speed = 115200
upload_speed = 921600
```

**Upload Process** (AI agent requests user to execute):
1. Connect ESP8266 via USB (NodeMCU board or USB-to-Serial adapter)
2. AI agent: "Please run: `pio run`"
3. User provides compilation output
4. AI agent: "Please run: `pio run --target upload`"
5. User provides upload output
6. AI agent: "Please run: `pio device monitor`"
7. User provides serial monitor output (115200 baud)

**Production Build**:
- Remove `-DENABLE_SERIAL_DEBUG` flag from build_flags
- AI agent requests: "Please run: `pio run --target size`"
- User provides memory usage output
- Test all functionality on hardware
- Document final memory usage in phase documentation

### Calibration and Configuration

**Temperature Sensor Calibration**:
- DS18B20 is factory calibrated (±0.5°C accuracy)
- Verify reading against known reference
- Adjust in software if systematic offset detected

**Water Level Sensor Adjustment**:
- Mount sensor at minimum safe water level
- Verify LOW signal when water present
- Adjust mounting height if needed

**Display Contrast**:
- Adjust via software if needed: `display.setContrast(value)`
- Default value usually optimal

**EEPROM Initialization**:
- First boot loads default settings
- Settings menu allows user customization
- Factory reset: Hold encoder button during power-up (future feature)

### Maintenance and Troubleshooting

**Common Issues**:

1. **Display not working**:
   - Check I2C address (0x3C vs 0x3D)
   - Verify I2C connections (SDA, SCL)
   - Run I2C scanner sketch

2. **Temperature reads -127°C or 85°C**:
   - Check OneWire connection
   - Verify 4.7kΩ pull-up resistor
   - Check sensor power supply

3. **Relays not switching**:
   - Verify PCF8574 I2C address (0x20)
   - Check relay module power supply
   - Test PCF8574 with I2C scanner

4. **Encoder not responding**:
   - Check pin connections (D2, D3, D4)
   - Verify internal pull-ups enabled
   - Test with multimeter

**Diagnostic Mode**:
- Enable serial debug: Add `-DENABLE_SERIAL_DEBUG` to build flags
- Monitor serial output at 9600 baud
- Observe sensor readings, I2C transactions, state changes

**Memory Monitoring**:
```bash
# Check memory usage after build
pio run --target size

# Expected output:
# Flash: ~29 KB / 30 KB (96%)
# SRAM: ~1.8 KB / 2 KB (90%)
```

### Future Enhancements

**Potential Additions** (if memory allows):

1. **WiFi Connectivity** (requires ESP32 upgrade):
   - Remote monitoring via web interface
   - Mobile app control
   - Data logging to cloud

2. **Advanced Scheduling**:
   - Timer-based pump activation
   - Scheduled heating cycles
   - Ozone generation schedules

3. **Multi-Zone Control**:
   - Support multiple temperature sensors
   - Zone-specific heating control
   - Individual zone status display

4. **Enhanced Diagnostics**:
   - Runtime statistics (pump hours, heating cycles)
   - Error history log
   - Maintenance reminders

5. **User Profiles**:
   - Multiple saved temperature presets
   - User-specific preferences
   - Quick-access favorites

**Memory Constraints**:
Current design uses ~29 KB Flash and ~1.8 KB SRAM, leaving minimal room for enhancements. Major feature additions would require:
- Migrating to Arduino Mega (256 KB Flash, 8 KB SRAM)
- Migrating to ESP32 (4 MB Flash, 520 KB SRAM)
- Aggressive optimization of existing code

## Appendix

### Pin Assignment Summary (ESP8266)

| ESP8266 Pin | GPIO | Function | Type | Notes |
|-------------|------|----------|------|-------|
| D1 | GPIO5 | I2C SCL | I2C | Shared bus (OLED + PCF8574) |
| D2 | GPIO4 | I2C SDA | I2C | Shared bus (OLED + PCF8574) |
| D3 | GPIO0 | Encoder SW | Input | Internal pull-up, safe for button |
| D4 | GPIO2 | Water Level | Input | Internal pull-up, active-low sensor |
| D5 | GPIO14 | DS18B20 Data | OneWire | 4.7kΩ pull-up to 3.3V required |
| D6 | GPIO12 | Buzzer | Output | Safe, no boot issues |
| D7 | GPIO13 | Encoder CLK | Input | Safe, no boot issues |
| D8 | GPIO15 | Encoder DT | Input | **Requires 10kΩ pull-down to GND!** |

**Boot-Critical Notes**:
- GPIO15 (D8) MUST be LOW during boot - external 10kΩ pull-down required
- GPIO0 (D3) must be HIGH during boot - internal pull-up provides this
- GPIO2 (D4) must be HIGH during boot - internal pull-up provides this
- Avoid using GPIO16 (D0) - limited functionality, used for deep sleep wake

### I2C Device Summary

| Device | Address | Purpose | Library | Pins |
|--------|---------|---------|---------|------|
| SH1106 OLED | 0x3C | Display | Adafruit SH110X | SCL=D1, SDA=D2 |
| PCF8574 | 0x20 | Relay control | Custom wrapper | SCL=D1, SDA=D2 |

### Memory Budget (ESP8266)

| Category | Allocation | Usage |
|----------|------------|-------|
| **Flash (4 MB)** | | |
| Program code | ~160 KB | 4% |
| PROGMEM strings | ~10 KB | 0.25% |
| PROGMEM constants | ~2 KB | 0.05% |
| File system (optional) | 0-2 MB | 0-50% |
| OTA partition (optional) | 0-1 MB | 0-25% |
| Available | ~3.8 MB | 95% |
| **SRAM (80 KB)** | | |
| Display buffer | 1024 bytes | 1.25% |
| Global variables | ~2 KB | 2.5% |
| Stack | ~4 KB | 5% |
| WiFi stack (if used) | ~20 KB | 25% |
| Available heap | ~53 KB | 66% |
| **EEPROM (4 KB emulated)** | | |
| Settings | 32 bytes | 0.8% |
| Reserved | ~4000 bytes | 99.2% |

### Timing Budget

| Operation | Interval | Max Duration |
|-----------|----------|--------------|
| Main loop | Continuous | < 50ms |
| Temperature read | 2000ms | < 750ms (blocking) |
| Water level read | 500ms | < 1ms |
| Display update | 100ms | < 20ms |
| Encoder poll | Every loop | < 1ms |
| Safety checks | Every loop | < 1ms |
| I2C transaction | As needed | < 5ms |

### Safety Interlock Matrix

| Actuator | Water Level | Circulation Pump | Temperature | Notes |
|----------|-------------|------------------|-------------|-------|
| Circulation Pump | Must be OK | N/A | Any | Primary pump |
| Massage Pump | Must be OK | Any | Any | Independent |
| Jet Pump | Must be OK | Any | Any | Independent |
| Heater | Must be OK | Must be ON | < TEMP_THERMAL_RUNAWAY | Critical interlock |
| Ozone | Any | Any | Any | Independent |
| Speaker | Any | Any | Any | Independent |
| Light | Any | Any | Any | Independent |

### Dynamic Configuration Constants

| Constant | Default Value | Purpose | Adjustable |
|----------|---------------|---------|------------|
| TEMP_MIN | 30.0°C | Minimum target temperature | Yes |
| TEMP_MAX | 40.0°C | Maximum target temperature | Yes |
| TEMP_INCREMENT | 0.5°C | Temperature adjustment step | Yes |
| TEMP_HYSTERESIS | 0.5°C | Auto control hysteresis | Yes |
| TEMP_THERMAL_RUNAWAY | 45.0°C | Thermal runaway threshold | Yes |
| IDLE_TIMEOUT_MIN | 10 seconds | Minimum idle timeout | Yes |
| IDLE_TIMEOUT_MAX | 120 seconds | Maximum idle timeout | Yes |
| IDLE_TIMEOUT_STEP | 10 seconds | Timeout adjustment step | Yes |
| WATER_LEVEL_ACTIVE_LOW | true | Sensor polarity | Yes |

**Note**: All constants defined in constants.h. Modify and recompile to change behavior without code changes.

### State Transition Diagram

```
                    ┌─────────────┐
                    │ POWER ON    │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │ INITIALIZING│ ◄─── POST: OLED, PCF8574, Sensors
                    └──────┬──────┘
                           │
                    ┌──────┴──────┐
                    │             │
                    ▼             ▼
            ┌──────────┐   ┌──────────┐
            │   IDLE   │   │  FAULT   │◄──── Critical Error
            └────┬─────┘   └──────────┘      (Thermal Runaway,
                 │                │            I2C Failure, etc.)
                 │                │
                 │                └──── Power Cycle Required
                 │
         ┌───────┴───────┐
         │               │
         ▼               ▼
    ┌─────────┐   ┌──────────┐
    │  MENU   │   │  FAULT   │
    └────┬────┘   └──────────┘
         │
         └──── Timeout (configurable) ───► IDLE
```

### 6-Step Task Execution Protocol Summary

**For AI Agent Development**:

1. **Pre-Git Setup**: Apply the step 1 of the Task Execution Protocol
2. **Deep Codebase Analysis**: Read all existing source files, platformio.ini, constants.h
3. **Previous Phase Analysis**: Read docs/phase-<N-1>-<description>.md
4. **Phase Execution**: 
   - Implement code
   - Request: `pio run` (wait for output)
   - Request: `pio run --target upload` (wait for output)
   - Request: `pio run --target size` (wait for output)
5. **User Hardware Review**: 
   - Output: "**Please test the following on hardware and provide feedback:**"
   - List specific test cases
   - Wait for user feedback
   - Iterate on issues until user confirms working
6. **Git Operations**: Request user to run git add, commit, push, merge

### References

**ESP8266 Documentation**:
- [ESP8266 Arduino Core](https://arduino-esp8266.readthedocs.io/) - Official ESP8266 Arduino documentation
- [ESP8266 Pinout Reference](https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/) - Comprehensive GPIO pin reference and boot modes
- [ESP8266 Boot Mode Selection](https://github.com/esp8266/esp8266-wiki/wiki/Boot-Process) - Understanding boot pins and requirements

**Memory Optimization**:
- [ESP8266 Memory Management](https://arduino-esp8266.readthedocs.io/en/latest/PROGMEM.html) - PROGMEM usage and memory optimization
- [ESP8266 EEPROM](https://arduino-esp8266.readthedocs.io/en/latest/libraries.html#eeprom) - EEPROM emulation in Flash

**Non-Blocking Patterns**:
- [Using millis() for Timing](https://forum.arduino.cc/t/using-millis-for-timing-a-beginners-guide/483573) - Comprehensive guide on non-blocking timing patterns
- [State Machines in Arduino](http://www.arduinolearning.com/learning/state-machines-in-arduino-projects-explained-with-examples.php) - State machine implementation patterns

**DS18B20 with ESP8266**:
- [DS18B20 Error Handling](https://forum.arduino.cc/t/ds18b20-error-85-c-127-c-causes-analysis-fixes-and-reliable-reading-methods/1433302) - Handling DS18B20 error conditions
- [OneWire on ESP8266](https://github.com/PaulStoffregen/OneWire) - OneWire library compatible with ESP8266

**I2C Communication**:
- [ESP8266 I2C](https://arduino-esp8266.readthedocs.io/en/latest/libraries.html#i2c-wire-library) - I2C (Wire) library documentation for ESP8266
- [PCF8574 GPIO Extender](https://www.hackster.io/tarantula3/pcf8574-gpio-extender-with-arduino-and-nodemcu-a8cb00) - Using PCF8574 with ESP8266/NodeMCU

**Libraries**:
- Adafruit SH110X: OLED display driver (ESP8266 compatible)
- OneWire: Dallas OneWire protocol (ESP8266 compatible)
- DallasTemperature: DS18B20 sensor interface (ESP8266 compatible)

---

**Document Version**: 2.0 (ESP8266)  
**Last Updated**: 2025-01-XX  
**Status**: Ready for Implementation  
**Hardware Platform**: ESP8266 (ESP-12E/ESP-12F)  
**Development Protocol**: 6-Step Task Execution with User Hardware Testing
