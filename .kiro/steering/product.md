# Product Overview

## Jacuzzi/Bathtub Controller System

ESP8266-based embedded control system for managing jacuzzi/bathtub operations including temperature control, water circulation, massage jets, heating, ozone generation, and lighting. The system prioritizes safety through multiple hardware and software interlocks while providing a user-friendly OLED menu interface.

## Core Functionality

### Temperature Management
- DS18B20 OneWire sensor with 0.1°C display precision
- Configurable target temperature (30-40°C range)
- Automatic heater control with hysteresis (0.5°C default)
- Thermal runaway protection (45°C threshold)
- Manual and automatic heating modes

### Water Level Safety
- Configurable sensor polarity (active-low/active-high)
- 3-sample majority voting for noise immunity
- Prevents all pump and heater operation without water
- Real-time status display with warning indicators

### Actuator Control
8-channel relay system via PCF8574 I2C expander:
- **Circulation Pump**: Primary water circulation (foundation for all water operations)
- **Massage Pump**: Massage jet control
- **Jet Pump**: High-pressure jet control
- **Heater**: 3kW water heater with multiple safety interlocks
- **Ozone Generator**: Water sanitization system
- **Speaker**: Media player/audio system
- **Light**: Jacuzzi lighting control
- **Reserved**: Future expansion

### User Interface
- SH1106 128x64 OLED display (I2C)
- KY-040 rotary encoder with button
- Hierarchical menu system with timeout
- Idle screen showing all system status
- Visual and audio feedback for all interactions
- Settings persistence via EEPROM

### Safety Features
- **Fail-Safe Startup**: All outputs OFF on power-up
- **Water Level Interlock**: Prevents pump/heater operation without water (<100ms response)
- **Heater Interlock**: Requires circulation pump running (<100ms response)
- **Thermal Runaway Protection**: Automatic shutdown above 45°C (<500ms response)
- **Fault State Management**: Sticky fault state requiring power cycle
- **Emergency Shutdown**: Immediate deactivation of all outputs
- **Graceful Degradation**: Partial functionality during non-critical sensor failures

## Development Approach

### Phased Implementation
10 incremental phases with working firmware at each stage:
1. **Phase 1**: Hardware initialization (✅ Complete)
2. **Phase 2**: Sensor integration
3. **Phase 3**: Rotary encoder input
4. **Phase 4**: Basic menu system
5. **Phase 5**: Actuator control
6. **Phase 6**: Safety interlocks
7. **Phase 7**: Settings and EEPROM
8. **Phase 8**: Automatic heater control
9. **Phase 9**: Polish and optimization
10. **Phase 10**: Documentation and testing

### Development Protocol
**6-Step Task Execution Protocol** (mandatory for every phase):
1. Pre-Git setup (branch creation, clean state)
2. Deep codebase analysis (read all existing code)
3. Previous phase analysis (understand what was implemented)
4. Phase execution (implement without deviation, request PlatformIO commands)
5. User hardware review (explicit testing request, iterate on issues)
6. Git operations (commit, push, merge, cleanup, document)

### Testing Approach
- **Manual Hardware Testing Only**: No automated tests
- User tests on actual ESP8266 hardware after each phase
- AI agent explicitly requests testing with specific test cases
- Iteration on issues until user confirms working
- Comprehensive phase documentation after completion

## Target Hardware

### Microcontroller
- **MCU**: ESP8266 (ESP-12E/ESP-12F)
- **Clock**: 80 MHz
- **Flash**: 4 MB (ample space for features)
- **SRAM**: 80 KB (efficient usage required)
- **EEPROM**: 4 KB emulated in Flash

### Display & Input
- **Display**: SH1106 128x64 OLED (I2C address 0x3C)
- **Encoder**: KY-040 rotary encoder (CLK, DT, SW)
- **Buzzer**: Piezo buzzer for audio feedback

### I/O Expansion
- **PCF8574**: I2C I/O expander (address 0x20) for 8 relay outputs
- **Relays**: 8-channel 5V relay module (active-HIGH to energize)

### Sensors
- **Temperature**: DS18B20 OneWire sensor (12-bit resolution)
- **Water Level**: Configurable polarity sensor (simulated via GPIO to GND)

### Critical Hardware Notes
- **GPIO15 (D8)**: Requires external 10kΩ pull-down resistor to GND for boot
- **GPIO0 (D3)**: Has internal pull-up, safe for button input
- **GPIO2 (D4)**: Has internal pull-up, safe for active-low sensor
- **I2C Bus**: Shared between OLED and PCF8574 (SCL=D1/GPIO5, SDA=D2/GPIO4)
- **OneWire**: Requires 4.7kΩ pull-up resistor to 3.3V

## Memory Budget

### Current Usage (Phase 1)
- **Flash**: ~97 KB / 4 MB (2.43%)
- **SRAM**: ~5.1 KB / 80 KB (6.33%)

### Final Estimate (Phase 10)
- **Flash**: ~160 KB / 4 MB (4%)
- **SRAM**: ~21 KB / 80 KB (26%)
- **Available**: Significant headroom for future enhancements

## Key Design Principles

1. **Safety First**: Multiple hardware and software interlocks prevent unsafe conditions
2. **Non-Blocking Architecture**: All operations use millis()-based state machines
3. **Single Source of Truth**: All constants centralized in constants.h
4. **Dynamic Configuration**: Adjustable parameters defined as constants (no code changes needed)
5. **Sensor Polarity Independence**: Code logic adapts to sensor configuration
6. **Memory Efficiency**: PROGMEM strings, optimized data types, minimal globals
7. **Fail-Safe Design**: All outputs default to OFF; safe state on errors
8. **Graceful Degradation**: Partial functionality during non-critical failures
