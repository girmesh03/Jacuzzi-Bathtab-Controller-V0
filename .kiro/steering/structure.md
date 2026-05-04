# Project Structure

## Directory Organization

```
project/
├── include/              # Public header files (.h)
│   ├── constants.h       # Single source of truth for all constants
│   ├── Display.h         # Display manager interface
│   ├── Actuators.h       # Actuator manager interface
│   ├── Sensors.h         # Sensor manager interface
│   ├── Input.h           # Input manager interface (Phase 3+)
│   ├── Menu.h            # Menu manager interface (Phase 4+)
│   └── Safety.h          # Safety manager interface (Phase 6+)
├── lib/                  # Module implementations
│   ├── Display/
│   │   └── Display.cpp   # Display implementation
│   ├── Actuators/
│   │   └── Actuators.cpp # Actuator implementation
│   ├── Sensors/
│   │   └── Sensors.cpp   # Sensor implementation (Phase 2+)
│   ├── Input/
│   │   └── Input.cpp     # Input implementation (Phase 3+)
│   ├── Menu/
│   │   └── Menu.cpp      # Menu implementation (Phase 4+)
│   └── Safety/
│       └── Safety.cpp    # Safety implementation (Phase 6+)
├── src/
│   └── main.cpp          # Main program entry point (integrates all modules)
├── docs/                 # Phase documentation
│   ├── phase-1-hardware-init.md
│   ├── phase-2-sensor-integration.md
│   └── phase-N-*.md      # One document per phase
├── .kiro/
│   ├── specs/            # Feature specifications
│   │   └── jacuzzi-bathtub-controller/
│   │       ├── requirements.md
│   │       ├── design.md
│   │       ├── tasks.md
│   │       └── task-execution-protocol.md
│   └── steering/         # Project guidance documents
│       ├── product.md
│       ├── tech.md
│       └── structure.md
├── .pio/                 # PlatformIO build artifacts (gitignored)
├── .vscode/              # VS Code configuration
├── platformio.ini        # Build configuration
└── README.md             # Project overview
```

## Module Pattern

All modules follow a consistent structure for maintainability:

### Header Files (include/)
- Public API declarations
- Class interfaces with clear method signatures
- Include guards (`#ifndef MODULE_H` / `#define MODULE_H` / `#endif`)
- Minimal dependencies (typically only constants.h)
- Well-documented public methods with purpose, parameters, return values

### Implementation Files (lib/ModuleName/)
- Private implementation details
- Located in `lib/ModuleName/ModuleName.cpp`
- Includes corresponding header from `include/`
- No cross-module dependencies (modules don't include each other)
- main.cpp orchestrates module interactions

### Example Module Structure
```cpp
// include/Sensors.h
#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include "constants.h"

class SensorManager {
public:
    void init();
    void update();  // Non-blocking, call every loop
    float getTemperature();
    bool isTemperatureValid();
    bool isWaterLevelOK();
    
private:
    float lastValidTemp;
    uint32_t lastTempUpdate;
    bool tempValid;
    bool waterLevelOK;
};

#endif

// lib/Sensors/Sensors.cpp
#include "Sensors.h"

void SensorManager::init() {
    // Implementation
}

void SensorManager::update() {
    // Non-blocking sensor reading
}
```

## Constants Management

**Single Source of Truth**: `include/constants.h`

All system constants defined in one location for easy configuration:

### Pin Assignments
```cpp
// I2C pins (ESP8266-specific)
#define PIN_SDA 4   // D2 (GPIO4)
#define PIN_SCL 5   // D1 (GPIO5)

// Sensor pins
#define PIN_TEMP_SENSOR 14  // D5 (GPIO14) - safe, no boot issues
#define PIN_WATER_LEVEL 2   // D4 (GPIO2) - has pull-up, safe for active-low

// Rotary encoder pins
#define PIN_ENCODER_CLK 13  // D7 (GPIO13) - safe
#define PIN_ENCODER_DT 15   // D8 (GPIO15) - REQUIRES external 10kΩ pull-down!
#define PIN_ENCODER_SW 0    // D3 (GPIO0) - has pull-up, safe for button

// Output pins
#define PIN_BUZZER 12       // D6 (GPIO12) - safe, no boot issues
```

### I2C Addresses
```cpp
#define I2C_OLED_ADDR 0x3C
#define I2C_PCF8574_ADDR 0x20
```

### Timing Constants (milliseconds)
```cpp
#define TEMP_UPDATE_INTERVAL 2000
#define WATER_LEVEL_INTERVAL 500
#define DISPLAY_MIN_FRAME_TIME 100
#define ENCODER_DEBOUNCE_TIME 50
#define IDLE_TIMEOUT_DEFAULT 30000
```

### Dynamic Configuration Constants
```cpp
// Temperature configuration (easy to adjust)
#define TEMP_MIN 30.0f
#define TEMP_MAX 40.0f
#define TEMP_INCREMENT 0.5f
#define TEMP_THERMAL_RUNAWAY 45.0f
#define TEMP_HYSTERESIS 0.5f

// Idle timeout configuration (easy to adjust)
#define IDLE_TIMEOUT_MIN 10
#define IDLE_TIMEOUT_MAX 120
#define IDLE_TIMEOUT_STEP 10

// Sensor polarity configuration (allows code-independent changes)
#define WATER_LEVEL_ACTIVE_LOW true
```

### Actuator Bit Positions
```cpp
#define ACTUATOR_CIRCULATION_PUMP 0
#define ACTUATOR_MASSAGE_PUMP 1
#define ACTUATOR_JET_PUMP 2
#define ACTUATOR_HEATER 3
#define ACTUATOR_OZONE 4
#define ACTUATOR_SPEAKER 5
#define ACTUATOR_LIGHT 6
#define ACTUATOR_RESERVED 7
```

### Menu Strings (PROGMEM)
```cpp
const char STR_SPLASH_TITLE[] PROGMEM = "Jacuzzi Controller";
const char STR_READY[] PROGMEM = "READY";
const char STR_CIRCULATION_PUMP[] PROGMEM = "Circulation Pump";
// ... more strings
```

**Rule**: Never hardcode values outside `constants.h`

## Naming Conventions

### Files
- Headers: `ModuleName.h` (PascalCase)
- Implementation: `ModuleName.cpp` (matches header)
- Main: `main.cpp` (lowercase)
- Documentation: `phase-N-description.md` (lowercase with hyphens)

### Classes
- PascalCase: `DisplayManager`, `ActuatorManager`, `SensorManager`
- Manager suffix for system-level modules
- Descriptive names indicating responsibility

### Functions
- camelCase: `init()`, `getTemperature()`, `showSplashScreen()`
- Descriptive names indicating action
- Getters: `getState()`, `isValid()`, `hasEvent()`
- Setters: `setState()`, `setTemperature()`

### Variables
- camelCase: `relayState`, `lastValidTemp`, `tempReadFailCount`
- No prefix for member variables (rely on context)
- Descriptive names avoiding abbreviations
- Boolean variables: `isValid`, `hasData`, `canActivate`

### Constants
- SCREAMING_SNAKE_CASE: `PIN_SDA`, `TEMP_MAX`, `I2C_OLED_ADDR`
- Descriptive prefixes: `PIN_*`, `I2C_*`, `TEMP_*`, `ACTUATOR_*`
- Clear indication of units: `_MS` for milliseconds, `_INTERVAL` for timing

### Macros
- SCREAMING_SNAKE_CASE: `DEBUG_PRINT`, `ENABLE_SERIAL_DEBUG`
- Conditional compilation: `#ifdef ENABLE_SERIAL_DEBUG`

## Code Organization Principles

### Separation of Concerns
- Each module has single, well-defined responsibility
- **Display module**: Only display operations (rendering, screen management)
- **Actuators module**: Only relay control (PCF8574 I/O, state management)
- **Sensors module**: Only sensor reading (temperature, water level)
- **Input module**: Only user input (encoder, button, buzzer feedback)
- **Menu module**: Only menu logic (navigation, selection, timeout)
- **Safety module**: Only safety checks (interlocks, fault state)

### Minimal Dependencies
- Modules depend on `constants.h` only
- No cross-module dependencies (modules don't include each other)
- `main.cpp` orchestrates module interactions
- Clear interfaces between modules

### Header Guards
All headers use include guards:
```cpp
#ifndef MODULE_H
#define MODULE_H
// ... content ...
#endif
```

### Documentation
- **File-level comments**: Explain purpose, platform, phase
- **Public method documentation**: Purpose, parameters, return values
- **Complex logic**: Inline comments explaining why, not what
- **Safety-critical code**: Detailed comments on timing and behavior
- **Phase documentation**: Complete docs/phase-N-*.md for each phase

## Integration with main.cpp

**Critical Rule**: Every phase MUST integrate with main.cpp

### Global Module Instances
```cpp
// Global objects for all modules
DisplayManager displayManager;
ActuatorManager actuatorManager;
SensorManager sensorManager;
InputManager inputManager;
MenuManager menuManager;
SafetyManager safetyManager;
```

### setup() Function Pattern
```cpp
void setup() {
    // 1. Serial initialization
    Serial.begin(115200);
    
    // 2. I2C initialization
    Wire.begin(PIN_SDA, PIN_SCL);
    
    // 3. Module initialization (order matters)
    displayManager.init();
    actuatorManager.init();
    sensorManager.init();
    inputManager.init();
    menuManager.init();
    safetyManager.init();
    
    // 4. Display splash screen
    displayManager.showSplashScreen();
    
    // 5. Display ready message
    displayManager.showReadyMessage();
}
```

### loop() Function Pattern
```cpp
void loop() {
    // 1. Update sensors (non-blocking)
    sensorManager.update();
    
    // 2. Check safety interlocks (every iteration)
    safetyManager.update();
    
    // 3. Process user input
    inputManager.update();
    
    // 4. Update menu state
    menuManager.update();
    
    // 5. Update display
    displayManager.update();
    
    // 6. Allow ESP8266 background tasks
    yield();
}
```

## Pin Assignment Documentation

Pin assignments documented in `constants.h` with safety notes:

```cpp
// ============================================================================
// PIN ASSIGNMENTS (ESP8266-specific)
// ============================================================================
// I2C pins
#define PIN_SDA 4  // D2 (GPIO4)
#define PIN_SCL 5  // D1 (GPIO5)

// Sensor pins
#define PIN_TEMP_SENSOR 14  // D5 (GPIO14) - safe, no boot issues
#define PIN_WATER_LEVEL 2   // D4 (GPIO2) - has pull-up, safe for active-low sensor

// Rotary encoder pins
#define PIN_ENCODER_CLK 13  // D7 (GPIO13) - safe
#define PIN_ENCODER_DT 15   // D8 (GPIO15) - REQUIRES external 10kΩ pull-down!
#define PIN_ENCODER_SW 0    // D3 (GPIO0) - has pull-up, safe for button

// Output pins
#define PIN_BUZZER 12  // D6 (GPIO12) - safe, no boot issues

// ============================================================================
// CRITICAL BOOT REQUIREMENTS
// ============================================================================
// GPIO15 (D8) MUST have external 10kΩ pull-down resistor to GND for boot
// GPIO0 (D3) must be HIGH during boot (internal pull-up provides this)
// GPIO2 (D4) must be HIGH during boot (internal pull-up provides this)
```

## Active-Low Logic Pattern

For active-low devices (relays, sensors):
- Internal state uses normal logic (0=OFF, 1=ON)
- Inversion handled in I/O layer
- Clear documentation of polarity

```cpp
// Relay control (active-HIGH to energize relay)
// Internal: relayState = 0x00 (all OFF)
// PCF8574: write relayState = 0x00 (all LOW = relays OFF)

// Water level sensor (configurable polarity)
bool pinState = digitalRead(PIN_WATER_LEVEL);
bool waterPresent = WATER_LEVEL_ACTIVE_LOW ? !pinState : pinState;
```

## Error Handling Pattern

- Return `bool` for operations that can fail
- `true` = success, `false` = failure
- Critical failures halt system (infinite loop with `yield()`)
- Non-critical failures logged and recovered

```cpp
if (!displayManager.init()) {
    DEBUG_PRINTLN("ERROR: Display init failed!");
    while (1) { yield(); }  // Halt system, allow ESP8266 background tasks
}
```

## Memory Management

- Prefer stack allocation for small objects
- Use pointers for objects with complex constructors
- Allocate display buffers on heap
- Minimize global variables
- Use `uint8_t` for byte-sized values
- Use `uint16_t` for 16-bit values
- Use `uint32_t` for timestamps (millis())

## Non-Blocking Architecture

All operations use millis()-based timing:

```cpp
// Non-blocking sensor update
uint32_t lastTempUpdate = 0;

void SensorManager::update() {
    if (millis() - lastTempUpdate >= TEMP_UPDATE_INTERVAL) {
        // Read temperature
        lastTempUpdate = millis();
    }
}
```

**Rules**:
- Never use `delay()` anywhere in code
- Always call `yield()` in loops
- Use state machines for time-dependent operations
- Target < 50ms main loop iteration time

## Build Artifacts

`.pio/` directory contains:
- Compiled objects (`.o` files)
- Linked binaries (`.elf`, `.bin` files)
- Downloaded libraries (managed by PlatformIO)
- Build cache for faster recompilation

**Note**: `.pio/` is gitignored except for library dependencies

## Development Protocol

### 6-Step Task Execution Protocol

Every phase follows this mandatory protocol:

1. **Pre-Git Setup**: Create branch, verify clean state
2. **Deep Codebase Analysis**: Read all existing code
3. **Previous Phase Analysis**: Understand what was implemented
4. **Phase Execution**: Implement without deviation, request PlatformIO commands
5. **User Hardware Review**: Explicit testing request, iterate on issues
6. **Git Operations**: Commit, push, merge, cleanup, document

### Phase Documentation

Each phase documented in `docs/phase-N-description.md`:
- Phase objectives and deliverables
- Implementation details
- Pin assignments used
- Memory usage (Flash and SRAM)
- Test results and procedures
- Issues encountered and resolutions
- Notes for next phase

## Safety-Critical Code Patterns

### Interlock Enforcement
```cpp
bool ActuatorManager::canActivateHeater() {
    // Multiple safety checks
    if (!sensorManager.isWaterLevelOK()) return false;
    if (!getState(ACTUATOR_CIRCULATION_PUMP)) return false;
    if (sensorManager.getTemperature() > TEMP_THERMAL_RUNAWAY) return false;
    return true;
}
```

### Fault State Management
```cpp
void SafetyManager::enterFaultState(const char* reason) {
    // Immediate shutdown
    actuatorManager.emergencyShutdown();
    
    // Display error
    displayManager.showError(reason);
    
    // Continuous buzzer
    while (1) {
        digitalWrite(PIN_BUZZER, HIGH);
        delay(1000);
        digitalWrite(PIN_BUZZER, LOW);
        delay(1000);
    }
}
```

## Testing Approach

- **Manual Hardware Testing Only**: No automated tests
- User tests on actual ESP8266 hardware after each phase
- AI agent explicitly requests testing with specific test cases
- Iteration on issues until user confirms working
- Comprehensive phase documentation after completion
