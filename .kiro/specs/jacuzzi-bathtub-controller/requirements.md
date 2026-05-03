# Requirements Document: Jacuzzi/Bathtub Controller System

## Introduction

This document specifies the requirements for an ESP8266-based embedded control system for a jacuzzi/bathtub. The system provides safe, user-friendly control of pumps, heating, ozone generation, lighting, and audio through an OLED menu interface with rotary encoder input. The system implements multiple safety interlocks to prevent equipment damage and ensure user safety, while maintaining memory efficiency and responsive operation.

## Task Execution Protocol

**CRITICAL**: The AI agent developing this system MUST follow this 6-step protocol for EVERY development phase:

### Step 1: Pre-Git Setup
1. **Check Current State:**
   - Execute `git status` to check current branch name, uncommitted changes, untracked files
   - Execute `git branch -vv` to display all local branches and tracking information
2. **Update Remote Information:**
   - Execute `git fetch origin` to update remote tracking information
3. **Handle Uncommitted Changes:**
   - IF uncommitted changes exist: stage, commit, push, merge, and delete feature branch
   - Think twice before acting - verify branch names and merge targets
4. **Synchronize Local with Remote:**
   - IF local branch is behind remote: execute `git pull origin <branch>`
   - IF merge conflicts detected: HALT immediately and prompt user
5. **Create Feature Branch:**
   - Execute `git checkout -b <descriptive-branch-name>`
   - Use clear, descriptive branch names matching task number
6. **Verify Clean State:**
   - Execute `git status` to confirm clean working directory
   - Proceed to Step 2 only after verification

### Step 2: Deep Codebase Analysis
- Read all existing source files (src/, include/, lib/, docs/)
- Read platformio.ini configuration
- Read constants.h for current pin mappings and constants
- Understand current system state and dependencies

### Step 3: Analysis of Previous Phase
- Read docs/phase-<N-1>-<description>.md if it exists
- Understand what was implemented in previous phase
- Identify integration points for current phase

### Step 4: Phase Execution
- Implement tasks exactly as specified in tasks.md
- Follow all requirements from requirements.md
- Follow all design patterns from design.md
- NO deviation from specifications
- Request user to run PlatformIO commands when needed:
  - Compilation: "Please run: `pio run`"
  - Upload: "Please run: `pio run --target upload`"
  - Memory check: "Please run: `pio run --target size`"
  - Clean: "Please run: `pio run --target clean`"
- AI agent MUST wait for user's command output before proceeding

### Step 5: User Hardware Review and Feedback
- AI agent MUST explicitly ask user: "**Please test the following on hardware and provide feedback:**"
- List specific test cases for current phase
- Wait for user's test results and feedback
- If issues reported:
    - **IF user requests changes:**
     - Implement changes following same protocol (Steps 2-4)
     - Re-request review after changes
     - Repeat until user is satisfied
   - **IF user approves without changes:**
     - Confirm explicit approval to proceed
     - Move to Step 6 for Git operations

### Step 6: Git Commit, Push, Merge, Cleanup
1. **Document the implementation:**
	- Document every single detail implemented in `docs/*`
	- Prefix the documentation file name with `phase-<N>-<...>`
2. **Verify Current State:**
   - Execute `git status` to check current branch and all modified/created files
   - Execute `git branch -vv` to display branch tracking information
   - Execute `git fetch origin` to update remote tracking information
3. **Stage and Commit Changes:**
   - Review all changes carefully: `git diff`
   - Stage all changes: `git add .`
   - Verify staged changes: `git status`
   - Commit with descriptive message: `git commit -m "feat: [Phase N] Descriptive Phase title and summary"`
   - Use conventional commit format: `feat:`, `fix:`, `refactor:`, `docs:`, etc.
4. **Push Feature Branch:**
   - Push to remote: `git push origin <feature-branch>`
   - Verify push success
   - Confirm remote branch exists: `git branch -r`
5. **Checkout Base Branch:**
   - Checkout main/master: `git checkout main` (or appropriate base branch)
   - Verify clean state: `git status`
   - Pull latest changes: `git pull origin main`
6. **Merge Feature Branch:**
   - **CRITICAL: Think twice before merging**
   - Verify you are on correct base branch: `git branch`
   - Merge feature branch: `git merge <feature-branch>`
   - **IF merge conflicts occur:** HALT immediately, prompt user to resolve conflicts manually
   - **IF merge successful:** Verify merged changes: `git log --oneline -5`
7. **Push Merged Changes:**
   - Push to remote: `git push origin main`
   - Verify push success
   - Confirm remote is updated: `git log origin/main --oneline -5`
8. **Delete Feature Branch (Local and Remote):**
   - **CRITICAL: Verify merge success before deleting**
   - Confirm feature branch is fully merged: `git branch --merged`
   - Delete local branch: `git branch -d <feature-branch>`
   - Delete remote branch: `git push origin --delete <feature-branch>`
   - Verify deletion: `git branch -a` (feature branch should not appear)
9. **Final Synchronization Verification:**
   - Execute `git status` - should show clean working directory
   - Execute `git branch -vv` - should show main branch in sync with origin
   - Execute `git log --oneline -5` - should show recent commit
   - Confirm local and remote are synchronized
10. **Cleanup Verification:**
   - Verify no orphaned branches: `git branch -a`
   - Verify no uncommitted changes: `git status`
   - Verify correct branch: `git branch` (should be on main)

## Glossary

- **System**: The complete ESP8266-based jacuzzi/bathtub controller firmware
- **Controller**: The ESP8266 microcontroller (ESP-12E/ESP-12F module)
- **OLED_Display**: 1.3" 128x64 I2C OLED display at address 0x3C, connected to SCL=D1 (GPIO5), SDA=D2 (GPIO4)
- **Rotary_Encoder**: KY-040 rotary encoder with CLK=D7 (GPIO13), DT=D8 (GPIO15), SW=D3 (GPIO0)
- **PCF8574**: I2C I/O expander at address 0x20 controlling all 8 relay outputs, connected to SCL=D1, SDA=D2
- **Circulation_Pump**: Primary water circulation pump (relay-controlled via PCF8574 bit 0, N.O.)
- **Massage_Pump**: Massage jet pump (relay-controlled via PCF8574 bit 1, N.O.)
- **Jet_Pump**: High-pressure jet pump (relay-controlled via PCF8574 bit 2, N.O.)
- **Heater**: 3 kW water heater (5V/30A relay-controlled via PCF8574 bit 3, N.O.)
- **Ozone_Generator**: Water sanitization ozone generator (relay-controlled via PCF8574 bit 4, N.O.)
- **Speaker**: Media player/speaker system (relay-controlled via PCF8574 bit 5, N.O.)
- **Light**: Jacuzzi lighting system (relay-controlled via PCF8574 bit 6, N.O.)
- **Temp_Sensor**: DS18B20 One-Wire temperature sensor on pin D5 (GPIO14)
- **Water_Level_Sensor**: Simulated water level sensor on pin D4 (GPIO2) with internal pull-up enabled. When pin is connected to GND = water present (Water_Level_OK=TRUE), when pin is HIGH (floating) = no water (Water_Level_OK=FALSE)
- **Buzzer**: Simple piezo buzzer on pin D6 (GPIO12)
- **Main_Menu**: Top-level OLED menu system for user navigation
- **Idle_Screen**: Default display shown when no user interaction occurs
- **Fault_State**: System state entered when safety violations or critical errors occur
- **Thermal_Runaway**: Condition where temperature exceeds safe operating limits
- **Water_Level_OK**: Condition where Water_Level_Sensor pin reads LOW (connected to GND), indicating sufficient water present
- **Interlock**: Safety mechanism preventing unsafe actuator combinations
- **Non_Blocking**: Code execution pattern using millis() without delay() calls
- **PROGMEM**: Flash memory storage for constant data to preserve SRAM
- **constants_h**: Single header file (constants.h) containing all system constants with dynamic configuration support
- **Phase**: Incremental development stage producing working main.cpp following 6-step protocol
- **Serial_Debug**: Diagnostic output conditionally compiled via ENABLE_SERIAL_DEBUG flag
- **Dynamic_Configuration**: All adjustable parameters (temperature increments, timeout steps, etc.) defined as constants in constants.h to allow easy modification without code changes
- **Developer/AI agent**: Referes to the one who is responsible for the developement of this project

## Requirements

### Requirement 1: Hardware Platform and Constraints

**User Story:** As a developer, I want the system to operate on ESP8266 hardware, so that the firmware has sufficient memory and processing power for all features.

#### Acceptance Criteria

1. THE System SHALL execute on ESP8266 (ESP-12E or ESP-12F module) hardware
2. THE System SHALL have access to 4 MB of Flash memory
3. THE System SHALL have access to 80 KB of SRAM
4. THE System SHALL use PlatformIO as the build and development environment
5. THE System SHALL compile with memory optimization flags (-Os -flto)
6. THE System SHALL maintain efficient memory usage despite increased available resources

### Requirement 2: I2C Device Communication

**User Story:** As a developer, I want reliable I2C communication with multiple devices on ESP8266-specific pins, so that the OLED display and relay controller operate without conflicts.

#### Acceptance Criteria

1. THE System SHALL communicate with OLED_Display at I2C address 0x3C using SCL=D1 (GPIO5) and SDA=D2 (GPIO4)
2. THE System SHALL communicate with PCF8574 at I2C address 0x20 using SCL=D1 (GPIO5) and SDA=D2 (GPIO4)
3. THE System SHALL prevent I2C bus conflicts between OLED_Display and PCF8574
4. THE System SHALL implement I2C error detection and recovery
5. WHEN an I2C communication error occurs, THE System SHALL log the error and retry the operation
6. THE System SHALL use Wire.begin(SDA_PIN, SCL_PIN) to initialize I2C with correct ESP8266 pins

### Requirement 3: Actuator Control via PCF8574

**User Story:** As a developer, I want centralized relay control through the I2C expander, so that all actuators can be managed efficiently.

#### Acceptance Criteria

1. THE System SHALL control Circulation_Pump via PCF8574 relay output
2. THE System SHALL control Massage_Pump via PCF8574 relay output
3. THE System SHALL control Jet_Pump via PCF8574 relay output
4. THE System SHALL control Heater via PCF8574 relay output
5. THE System SHALL control Ozone_Generator via PCF8574 relay output
6. THE System SHALL control Speaker via PCF8574 relay output
7. THE System SHALL control Light via PCF8574 relay output
8. THE System SHALL initialize all PCF8574 outputs to OFF (relay open) state on startup

### Requirement 4: Temperature Sensing

**User Story:** As a user, I want continuous temperature monitoring, so that I can see the current water temperature and the system can regulate heating.

#### Acceptance Criteria

1. THE System SHALL read temperature from Temp_Sensor using One-Wire protocol on pin D5 (GPIO14)
2. THE System SHALL update temperature readings at least once every 2 seconds
3. THE System SHALL display current temperature on OLED_Display with 0.1°C precision
4. WHEN Temp_Sensor reading fails, THE System SHALL display "TEMP ERROR" on OLED_Display
5. THE System SHALL detect Temp_Sensor disconnection within 5 seconds
6. THE System SHALL use GPIO14 which is safe for DS18B20 (no boot interference)

### Requirement 5: Water Level Detection

**User Story:** As a user, I want the system to detect water level, so that pumps and heater never operate without sufficient water.

#### Acceptance Criteria

1. THE System SHALL read Water_Level_Sensor state from pin D4 (GPIO2) with internal pull-up enabled
2. THE System SHALL interpret LOW signal (pin connected to GND) as Water_Level_OK=TRUE condition
3. THE System SHALL interpret HIGH signal (pin floating/open) as Water_Level_OK=FALSE condition
4. THE System SHALL update water level status at least once every 500 milliseconds
5. THE System SHALL display water level status on OLED_Display
6. THE System SHALL use GPIO2 which has built-in pull-up resistor suitable for this application
7. THE System SHALL implement logic that is independent of sensor polarity (active-low vs active-high) through constants.h configuration

### Requirement 6: Water Level Interlock (Critical Safety)

**User Story:** As a user, I want the system to prevent actuator operation without water, so that equipment is not damaged and safety is maintained.

#### Acceptance Criteria

1. WHEN Water_Level_OK is FALSE, THE System SHALL prevent Circulation_Pump activation
2. WHEN Water_Level_OK is FALSE, THE System SHALL prevent Massage_Pump activation
3. WHEN Water_Level_OK is FALSE, THE System SHALL prevent Jet_Pump activation
4. WHEN Water_Level_OK is FALSE, THE System SHALL prevent Heater activation
5. WHEN Water_Level_OK transitions from TRUE to FALSE, THE System SHALL immediately deactivate Circulation_Pump
6. WHEN Water_Level_OK transitions from TRUE to FALSE, THE System SHALL immediately deactivate Massage_Pump
7. WHEN Water_Level_OK transitions from TRUE to FALSE, THE System SHALL immediately deactivate Jet_Pump
8. WHEN Water_Level_OK transitions from TRUE to FALSE, THE System SHALL immediately deactivate Heater
9. WHEN Water_Level_OK is FALSE, THE System SHALL display "LOW WATER" warning on OLED_Display
10. WHEN Water_Level_OK is FALSE and user attempts actuator activation, THE System SHALL sound Buzzer for 200 milliseconds

### Requirement 7: Heater Safety Interlock (Critical Safety)

**User Story:** As a user, I want the heater to operate only when circulation and water level are adequate, so that the heater never runs dry and causes fire hazard.

#### Acceptance Criteria

1. WHEN Circulation_Pump is OFF, THE System SHALL prevent Heater activation
2. WHEN Water_Level_OK is FALSE, THE System SHALL prevent Heater activation
3. WHEN Circulation_Pump transitions to OFF state, THE System SHALL immediately deactivate Heater within 100 milliseconds
4. WHEN Water_Level_OK transitions to FALSE, THE System SHALL immediately deactivate Heater within 100 milliseconds
5. WHEN user attempts Heater activation without Circulation_Pump running, THE System SHALL display "START PUMP FIRST" message
6. WHEN user attempts Heater activation without Water_Level_OK, THE System SHALL display "LOW WATER" message
7. WHEN Heater interlock prevents activation, THE System SHALL sound Buzzer for 200 milliseconds

### Requirement 8: Thermal Runaway Protection (Critical Safety)

**User Story:** As a user, I want automatic shutdown if temperature exceeds safe limits, so that overheating cannot cause equipment damage or injury.

#### Acceptance Criteria

1. THE System SHALL define maximum safe temperature threshold in constants_h
2. WHEN temperature exceeds maximum safe threshold, THE System SHALL enter Fault_State within 500 milliseconds
3. WHEN entering Fault_State due to Thermal_Runaway, THE System SHALL deactivate all PCF8574 outputs immediately
4. WHEN in Fault_State due to Thermal_Runaway, THE System SHALL display "OVERHEAT FAULT" on OLED_Display
5. WHEN in Fault_State due to Thermal_Runaway, THE System SHALL sound Buzzer continuously with 1 second on/off pattern
6. WHEN in Fault_State, THE System SHALL ignore all user input except system reset
7. THE System SHALL require power cycle to exit Fault_State

### Requirement 9: Fail-Safe Startup

**User Story:** As a developer, I want all outputs OFF at startup, so that unexpected actuator activation never occurs during initialization.

#### Acceptance Criteria

1. THE System SHALL initialize all PCF8574 outputs to OFF state before entering main loop
2. THE System SHALL complete sensor initialization before enabling user control
3. THE System SHALL display "INITIALIZING..." message during startup
4. THE System SHALL sound Buzzer once for 100 milliseconds when initialization completes successfully
5. WHEN initialization fails, THE System SHALL enter Fault_State and display error message

### Requirement 10: Rotary Encoder Input

**User Story:** As a user, I want to navigate menus using a rotary encoder, so that I can control all system functions through an intuitive interface.

#### Acceptance Criteria

1. THE System SHALL read Rotary_Encoder CLK signal from pin D7 (GPIO13)
2. THE System SHALL read Rotary_Encoder DT signal from pin D8 (GPIO15) with external 10kΩ pull-down resistor
3. THE System SHALL read Rotary_Encoder SW (button) signal from pin D3 (GPIO0) with internal pull-up enabled
4. WHEN Rotary_Encoder rotates clockwise, THE System SHALL move menu selection down
5. WHEN Rotary_Encoder rotates counter-clockwise, THE System SHALL move menu selection up
6. WHEN Rotary_Encoder SW is pressed, THE System SHALL activate selected menu item
7. THE System SHALL debounce Rotary_Encoder inputs with minimum 50 millisecond delay
8. WHEN Rotary_Encoder SW is pressed, THE System SHALL sound Buzzer for 50 milliseconds
9. THE System SHALL use GPIO13, GPIO15, and GPIO0 which are safe for encoder operation (GPIO15 requires external pull-down)

### Requirement 11: OLED Display Interface

**User Story:** As a user, I want a clear visual interface, so that I can see system status and navigate menus easily.

#### Acceptance Criteria

1. THE System SHALL initialize OLED_Display at I2C address 0x3C during startup
2. THE System SHALL update OLED_Display at minimum 10 frames per second during menu navigation
3. THE System SHALL display Main_Menu with selectable items
4. THE System SHALL highlight currently selected menu item
5. THE System SHALL display current temperature on all screens
6. THE System SHALL display water level status icon on all screens
7. THE System SHALL use visually pleasing layout with appropriate fonts and spacing

### Requirement 12: Main Menu Structure

**User Story:** As a user, I want a hierarchical menu system, so that I can access all control functions in an organized manner.

#### Acceptance Criteria

1. THE System SHALL provide Main_Menu with the following items: "Circulation Pump", "Massage Pump", "Jet Pump", "Heater", "Ozone", "Speaker", "Light", "Settings"
2. WHEN menu item is selected, THE System SHALL display item-specific control screen
3. WHEN no user input occurs for 30 seconds, THE System SHALL return to Idle_Screen
4. THE System SHALL allow navigation back to Main_Menu from any sub-menu
5. THE System SHALL wrap menu selection from last item to first item and vice versa
6. When the Circulation Pump turned OFF, The Heater MUST be turned OFF

### Requirement 13: Idle Screen Display

**User Story:** As a user, I want an attractive idle screen, so that the system looks professional when not in active use.

#### Acceptance Criteria

1. THE System SHALL display Idle_Screen when no user interaction occurs for 30 seconds
2. THE Idle_Screen SHALL display current temperature prominently
3. THE Idle_Screen SHALL display water level status
4. THE Idle_Screen SHALL display status of all active actuators
5. WHEN Rotary_Encoder is rotated or pressed on Idle_Screen, THE System SHALL return to Main_Menu

### Requirement 14: Individual Actuator Control

**User Story:** As a user, I want independent control of each actuator, so that I can customize my jacuzzi experience.

#### Acceptance Criteria

1. THE System SHALL provide ON/OFF toggle control for Speaker
2. THE System SHALL provide ON/OFF toggle control for Light
3. THE System SHALL provide ON/OFF toggle control for Ozone_Generator
4. THE System SHALL provide ON/OFF toggle control for Massage_Pump (subject to Water_Level_OK)
5. THE System SHALL provide ON/OFF toggle control for Jet_Pump (subject to Water_Level_OK)
6. THE System SHALL provide ON/OFF toggle control for Circulation_Pump (subject to Water_Level_OK)
7. WHEN actuator state changes, THE System SHALL update OLED_Display within 200 milliseconds
8. WHEN actuator state changes, THE System SHALL sound Buzzer for 50 milliseconds

### Requirement 15: Heater Control with Interlocks

**User Story:** As a user, I want to control the heater safely, so that water is heated only when conditions are safe.

#### Acceptance Criteria

1. THE System SHALL provide Heater control menu item
2. WHEN Heater menu item is selected and Circulation_Pump is OFF, THE System SHALL display "START PUMP FIRST" message
3. WHEN Heater menu item is selected and Water_Level_OK is FALSE, THE System SHALL display "LOW WATER" message
4. WHEN Heater menu item is selected and all interlocks are satisfied, THE System SHALL allow ON/OFF toggle
5. WHEN Heater is activated, THE System SHALL display "HEATING" indicator on all screens
6. WHEN Heater interlock condition fails during operation, THE System SHALL deactivate Heater and display warning

### Requirement 16: Buzzer Feedback

**User Story:** As a user, I want audio feedback for interactions and warnings, so that I receive confirmation and alerts without looking at the display.

#### Acceptance Criteria

1. THE System SHALL control Buzzer via pin D6 (GPIO12)
2. WHEN user presses Rotary_Encoder button, THE System SHALL sound Buzzer for 50 milliseconds
3. WHEN actuator state changes, THE System SHALL sound Buzzer for 50 milliseconds
4. WHEN safety interlock prevents action, THE System SHALL sound Buzzer for 200 milliseconds
5. WHEN Fault_State is entered, THE System SHALL sound Buzzer continuously with 1 second on/off pattern
6. THE System SHALL implement non-blocking Buzzer control using millis()
7. THE System SHALL use GPIO12 which is safe for buzzer operation (no boot interference)

### Requirement 17: Non-Blocking Architecture

**User Story:** As a developer, I want the entire system to be non-blocking, so that sensor monitoring, display updates, and user input remain responsive at all times.

#### Acceptance Criteria

1. THE System SHALL use millis() for all timing operations
2. THE System SHALL NOT use delay() function anywhere in the code
3. THE System SHALL implement state machines for all time-dependent operations
4. THE System SHALL check sensor states at least once per main loop iteration
5. THE System SHALL process user input at least once per main loop iteration
6. THE System SHALL update OLED_Display at least once per main loop iteration when content changes

### Requirement 18: Memory Optimization

**User Story:** As a developer, I want efficient memory usage, so that the firmware remains maintainable and responsive despite ESP8266's larger memory capacity.

#### Acceptance Criteria

1. THE System SHALL use uint8_t for all 8-bit values
2. THE System SHALL use uint16_t for all 16-bit values
3. THE System SHALL store all constant strings in PROGMEM
4. THE System SHALL store all constant data arrays in PROGMEM
5. THE System SHALL avoid unnecessary dynamic memory allocation
6. THE System SHALL minimize global variable usage
7. THE System SHALL use bit fields for boolean flags where appropriate
8. THE System SHALL compile with -Os optimization flag
9. THE System SHALL compile with -flto (link-time optimization) flag
10. THE System SHALL maintain efficient code practices even with increased memory availability

### Requirement 19: Single Source of Truth (constants.h)

**User Story:** As a developer, I want all constants in one place with dynamic configuration support, so that configuration changes are easy and consistent without code modifications.

#### Acceptance Criteria

1. THE System SHALL define all pin assignments in constants_h
2. THE System SHALL define all I2C addresses in constants_h
3. THE System SHALL define all timing constants in constants_h
4. THE System SHALL define all temperature thresholds in constants_h
5. THE System SHALL define all menu strings in constants_h using PROGMEM
6. THE System SHALL define all display layout constants in constants_h
7. THE System SHALL contain NO hardcoded pin numbers outside constants_h
8. THE System SHALL contain NO hardcoded I2C addresses outside constants_h
9. THE System SHALL contain NO hardcoded timing values outside constants_h
10. THE System SHALL contain NO hardcoded string literals outside constants_h (except debug messages)
11. THE System SHALL define all adjustable parameters as constants (temperature increment, timeout step, hysteresis value, etc.)
12. THE System SHALL implement sensor polarity configuration (WATER_LEVEL_ACTIVE_LOW) to support different sensor types without code changes
13. THE System SHALL use these dynamic constants throughout the code to ensure changes in constants.h automatically reflect in behavior

### Requirement 20: Serial Debug Gate

**User Story:** As a developer, I want optional serial debugging, so that I can diagnose issues without consuming memory in production builds.

#### Acceptance Criteria

1. THE System SHALL conditionally compile all Serial.print() statements based on ENABLE_SERIAL_DEBUG flag
2. THE System SHALL define ENABLE_SERIAL_DEBUG via PlatformIO build flag
3. WHEN ENABLE_SERIAL_DEBUG is undefined, THE System SHALL exclude all serial debug code from compilation
4. WHEN ENABLE_SERIAL_DEBUG is defined, THE System SHALL output diagnostic messages via Serial at 9600 baud
5. THE System SHALL use preprocessor macros to wrap all debug output

### Requirement 21: Phased Integration with Task Execution Protocol

**User Story:** As a developer, I want incremental development phases following a strict protocol, so that each step produces testable, working firmware with proper version control.

#### Acceptance Criteria

1. THE System SHALL be developed in discrete phases numbered sequentially
2. EACH phase SHALL produce a complete, compilable main.cpp
3. EACH phase SHALL build upon previous phase without breaking existing functionality
4. EACH phase SHALL be documented in docs/phase-<N>-<description>.md file
5. EACH phase SHALL include memory usage estimate (Flash and SRAM)
6. THE System SHALL maintain working state after each phase completion
7. EACH phase SHALL follow the 6-step Task Execution Protocol:
   - Step 1: Pre-Git setup (branch, pull, clean state)
   - Step 2: Deep codebase analysis
   - Step 3: Analysis of previous phase (N-1)
   - Step 4: Phase execution without deviation
   - Step 5: User hardware review and feedback (AI agent MUST request user testing)
   - Step 6: Git commit, push, merge, cleanup
8. THE AI agent SHALL request user to run PlatformIO commands and wait for output
9. THE AI agent SHALL NOT proceed to next phase until user confirms current phase is working

### Requirement 22: PlatformIO Configuration

**User Story:** As a developer, I want proper PlatformIO configuration for ESP8266, so that the project builds correctly with all dependencies.

#### Acceptance Criteria

1. THE System SHALL define platform as "espressif8266" in platformio.ini
2. THE System SHALL define board as "esp12e" in platformio.ini
3. THE System SHALL define framework as "arduino" in platformio.ini
4. THE System SHALL include required libraries in lib_deps: Adafruit SH110X, OneWire, DallasTemperature, PCF8574 library
5. THE System SHALL define build_flags including -Os, -flto, and ENABLE_SERIAL_DEBUG (optional)
6. THE System SHALL define monitor_speed as 115200 in platformio.ini (ESP8266 default)
7. THE System SHALL use build_src_filter to exclude unused example files
8. THE System SHALL configure upload_speed appropriately for ESP8266 (e.g., 921600)
9. THE Developer SHALL request user to run all PlatformIO commands (pio run, pio run --target upload, pio run --target size)

### Requirement 23: Temperature Display Precision

**User Story:** As a user, I want temperature displayed with appropriate precision, so that I can monitor heating accurately.

#### Acceptance Criteria

1. THE System SHALL display temperature with 0.1°C precision
2. THE System SHALL format temperature as "XX.X°C" on OLED_Display
3. WHEN temperature is below 10°C, THE System SHALL format as "X.X°C"
4. THE System SHALL update displayed temperature within 500 milliseconds of sensor reading change

### Requirement 24: Water Level Status Display

**User Story:** As a user, I want clear water level indication, so that I immediately know if water level is insufficient.

#### Acceptance Criteria

1. THE System SHALL display water level status icon on all screens
2. WHEN Water_Level_OK is TRUE, THE System SHALL display filled water drop icon
3. WHEN Water_Level_OK is FALSE, THE System SHALL display empty water drop icon with "LOW WATER" text
4. WHEN Water_Level_OK is FALSE, THE System SHALL flash water level warning at 1 Hz rate

### Requirement 25: Actuator Status Indicators

**User Story:** As a user, I want to see which actuators are currently active, so that I know the system state at a glance.

#### Acceptance Criteria

1. THE System SHALL display status indicators for all actuators on Idle_Screen
2. WHEN Circulation_Pump is ON, THE System SHALL display "PUMP" indicator
3. WHEN Heater is ON, THE System SHALL display "HEAT" indicator
4. WHEN Massage_Pump is ON, THE System SHALL display "MASSAGE" indicator
5. WHEN Jet_Pump is ON, THE System SHALL display "JET" indicator
6. WHEN Ozone_Generator is ON, THE System SHALL display "O3" indicator
7. WHEN Speaker is ON, THE System SHALL display "AUDIO" indicator
8. WHEN Light is ON, THE System SHALL display "LIGHT" indicator

### Requirement 26: Menu Navigation Responsiveness

**User Story:** As a user, I want immediate menu response, so that the interface feels smooth and professional.

#### Acceptance Criteria

1. WHEN Rotary_Encoder is rotated, THE System SHALL update menu selection within 50 milliseconds
2. WHEN Rotary_Encoder button is pressed, THE System SHALL activate menu item within 50 milliseconds
3. THE System SHALL provide visual feedback for menu selection changes within 50 milliseconds
4. THE System SHALL prevent menu selection changes faster than 100 milliseconds (debouncing)

### Requirement 27: Error Recovery

**User Story:** As a user, I want the system to recover from transient errors, so that temporary issues don't require system restart.

#### Acceptance Criteria

1. WHEN Temp_Sensor read fails, THE System SHALL retry reading after 1 second
2. WHEN I2C communication fails, THE System SHALL retry operation up to 3 times
3. WHEN I2C communication fails 3 times consecutively, THE System SHALL display "I2C ERROR" message
4. WHEN Temp_Sensor is disconnected and then reconnected, THE System SHALL resume normal operation within 5 seconds
5. THE System SHALL NOT enter Fault_State for transient sensor read failures

### Requirement 28: Power-On Self Test

**User Story:** As a developer, I want startup diagnostics, so that hardware issues are detected before normal operation begins.

#### Acceptance Criteria

1. THE System SHALL verify OLED_Display communication during startup
2. THE System SHALL verify PCF8574 communication during startup
3. THE System SHALL verify Temp_Sensor presence during startup
4. WHEN any hardware verification fails, THE System SHALL display specific error message
5. WHEN any hardware verification fails, THE System SHALL enter Fault_State
6. WHEN all hardware verification succeeds, THE System SHALL sound Buzzer once and proceed to Idle_Screen

### Requirement 29: Settings Menu

**User Story:** As a user, I want to configure system parameters, so that I can customize behavior to my preferences.

#### Acceptance Criteria

1. THE System SHALL provide Settings menu item in Main_Menu
2. THE Settings menu SHALL include "Target Temp" setting
3. THE Settings menu SHALL include "Idle Timeout" setting
4. THE Settings menu SHALL include "About" information screen
5. WHEN settings are changed, THE System SHALL persist changes across power cycles using EEPROM
6. THE System SHALL display current setting values in Settings menu

### Requirement 30: Target Temperature Control

**User Story:** As a user, I want to set a target temperature, so that the heater automatically maintains desired water temperature.

#### Acceptance Criteria

1. THE System SHALL allow target temperature setting from TEMP_MIN to TEMP_MAX in TEMP_INCREMENT steps (all defined in constants.h)
2. WHEN current temperature is below target temperature minus TEMP_HYSTERESIS, THE System SHALL activate Heater (subject to interlocks)
3. WHEN current temperature reaches target temperature, THE System SHALL deactivate Heater
4. THE System SHALL display target temperature on Heater control screen
5. THE System SHALL implement hysteresis using TEMP_HYSTERESIS constant to prevent rapid cycling
6. THE System SHALL use dynamic constants (TEMP_MIN=30.0°C, TEMP_MAX=40.0°C, TEMP_INCREMENT=0.5°C, TEMP_HYSTERESIS=0.5°C as defaults) allowing easy modification

### Requirement 31: Automatic Heater Control

**User Story:** As a user, I want automatic temperature maintenance, so that I don't need to manually control the heater.

#### Acceptance Criteria

1. WHERE automatic heater mode is enabled, WHEN current temperature is below target temperature minus TEMP_HYSTERESIS, THE System SHALL activate Heater (subject to interlocks)
2. WHERE automatic heater mode is enabled, WHEN current temperature reaches target temperature, THE System SHALL deactivate Heater
3. WHERE automatic heater mode is enabled, THE System SHALL display "AUTO" indicator on Heater control screen
4. THE System SHALL allow user to toggle between manual and automatic heater modes
5. WHERE automatic heater mode is enabled, WHEN Circulation_Pump is OFF, THE System SHALL NOT activate Heater
6. THE System SHALL use TEMP_HYSTERESIS constant for automatic control logic

### Requirement 32: EEPROM Settings Persistence

**User Story:** As a user, I want my settings saved, so that they are retained after power loss.

#### Acceptance Criteria

1. THE System SHALL store target temperature setting in EEPROM
2. THE System SHALL store idle timeout setting in EEPROM
3. THE System SHALL store automatic heater mode setting in EEPROM
4. WHEN settings are changed, THE System SHALL write to EEPROM within 1 second
5. THE System SHALL read settings from EEPROM during startup
6. WHEN EEPROM contains invalid data, THE System SHALL use default values

### Requirement 33: Documentation Requirements

**User Story:** As a developer, I want comprehensive documentation, so that I can understand and maintain the system.

#### Acceptance Criteria

1. EACH phase SHALL be documented in separate markdown file in docs/ directory
2. EACH phase document SHALL include: objectives, implementation details, testing procedure, memory impact
3. THE System SHALL include README.md with project overview and build instructions
4. THE System SHALL include hardware connection diagram in docs/
5. THE System SHALL include menu navigation diagram in docs/
6. THE System SHALL include state machine diagram in docs/

### Requirement 34: Code Organization

**User Story:** As a developer, I want well-organized code, so that modules are easy to locate and maintain.

#### Acceptance Criteria

1. THE System SHALL organize code into logical modules (sensors, actuators, display, menu, safety)
2. EACH module SHALL have corresponding .h and .cpp files
3. THE System SHALL use include guards in all header files
4. THE System SHALL minimize inter-module dependencies
5. THE System SHALL document all public functions with comments describing purpose, parameters, and return values

### Requirement 35: Circulation Pump Priority

**User Story:** As a user, I want the circulation pump to be the foundation of all water-based operations, so that water flow is always established before other actuators engage.

#### Acceptance Criteria

1. THE System SHALL allow Circulation_Pump activation independent of other actuators (subject only to Water_Level_OK)
2. WHEN Circulation_Pump is deactivated, THE System SHALL immediately deactivate Heater
3. WHEN Circulation_Pump is deactivated, THE System SHALL display warning if Heater was active
4. THE System SHALL recommend starting Circulation_Pump before other water-based actuators

### Requirement 36: Ozone Generator Safety

**User Story:** As a user, I want safe ozone generator operation, so that ozone is only generated when water is circulating.

#### Acceptance Criteria

1. THE System SHALL allow Ozone_Generator activation independent of Circulation_Pump state
2. THE System SHALL display recommendation to run Circulation_Pump when activating Ozone_Generator
3. WHEN Ozone_Generator is activated, THE System SHALL display "O3 ACTIVE" warning
4. THE System SHALL allow Ozone_Generator to run for user-defined duration with auto-shutoff

### Requirement 37: Menu Timeout Configuration

**User Story:** As a user, I want to configure idle timeout, so that the display returns to idle screen at my preferred interval.

#### Acceptance Criteria

1. THE System SHALL allow idle timeout configuration from IDLE_TIMEOUT_MIN to IDLE_TIMEOUT_MAX in IDLE_TIMEOUT_STEP increments (all defined in constants.h)
2. WHEN no user input occurs for configured timeout duration, THE System SHALL return to Idle_Screen
3. THE System SHALL reset timeout counter on any Rotary_Encoder interaction
4. THE System SHALL display remaining timeout on menu screens (optional)
5. THE System SHALL use dynamic constants (IDLE_TIMEOUT_MIN=10 seconds, IDLE_TIMEOUT_MAX=120 seconds, IDLE_TIMEOUT_STEP=10 seconds as defaults)

### Requirement 38: About Screen

**User Story:** As a user, I want to see system information, so that I know firmware version and hardware configuration.

#### Acceptance Criteria

1. THE System SHALL provide About screen in Settings menu
2. THE About screen SHALL display firmware version
3. THE About screen SHALL display compilation date
4. THE About screen SHALL display memory usage (Flash and SRAM)
5. THE About screen SHALL display uptime since last reset

### Requirement 39: Startup Splash Screen

**User Story:** As a user, I want a professional startup screen, so that the system appears polished during initialization.

#### Acceptance Criteria

1. THE System SHALL display splash screen immediately after OLED_Display initialization
2. THE splash screen SHALL display project name "Jacuzzi Controller"
3. THE splash screen SHALL display firmware version
4. THE splash screen SHALL display for 2 seconds
5. THE System SHALL transition from splash screen to initialization status display

### Requirement 40: Graceful Degradation

**User Story:** As a user, I want the system to remain partially functional during sensor failures, so that I can still control actuators manually.

#### Acceptance Criteria

1. WHEN Temp_Sensor fails, THE System SHALL disable automatic heater control
2. WHEN Temp_Sensor fails, THE System SHALL allow manual actuator control (subject to Water_Level_OK)
3. WHEN Temp_Sensor fails, THE System SHALL display "TEMP SENSOR FAULT" message
4. WHEN Water_Level_Sensor fails (stuck LOW), THE System SHALL allow operation with user confirmation
5. THE System SHALL log all sensor failures for diagnostic purposes (when Serial_Debug enabled)

### Requirement 41: ESP8266 Pin Safety and Boot Considerations

**User Story:** As a developer, I want safe GPIO pin usage on ESP8266, so that the system boots reliably and operates without interference.

#### Acceptance Criteria

1. THE System SHALL use GPIO5 (D1) and GPIO4 (D2) for I2C communication (SCL and SDA respectively)
2. THE System SHALL use GPIO14 (D5) for DS18B20 temperature sensor (safe, no boot issues)
3. THE System SHALL use GPIO12 (D6) for Buzzer (safe, no boot issues)
4. THE System SHALL use GPIO13 (D7) for Rotary Encoder CLK (safe, no boot issues)
5. THE System SHALL use GPIO15 (D8) for Rotary Encoder DT with external 10kΩ pull-down resistor (required for boot)
6. THE System SHALL use GPIO0 (D3) for Rotary Encoder SW button (has internal pull-up, safe for button input)
7. THE System SHALL use GPIO2 (D4) for Water Level Sensor (has internal pull-up, safe for active-low sensor)
8. THE System SHALL avoid using GPIO16 (D0) as it has limited functionality
9. THE System SHALL document pin usage and boot requirements in hardware connection diagram
10. THE System SHALL initialize all GPIO pins in setup() before use

### Requirement 42: Development Protocol

**User Story:** You as a developer, You must respect the 6 steps development protocol.

#### Acceptance Criteria

1. THE Development SHALL follow the 6-step Task Execution Protocol for every phase
2. THE Developer SHALL integrate every single developement phase with main.cpp 
3. THE Developer SHALL request user to run PlatformIO commands: "Please run: `pio run`"
4. THE Developer SHALL wait for user's command output before proceeding
5. THE Developer SHALL explicitly request hardware testing: "**Please test the following on hardware and provide feedback:**"
6. THE Developer SHALL list specific test cases for current phase
7. THE Developer SHALL iterate on issues until user confirms working
8. THE Developer SHALL NOT proceed to next phase without user confirmation
9. THE Developer SHALL run Git operations commands (add, commit, push, merge)
10. THE Developer SHALL document each phase completion in docs/phase-<N>-<description>.md

## Summary

This requirements document defines 42 comprehensive requirements organized into functional areas covering hardware platform (ESP8266), safety interlocks, user interface, control logic, sensors, non-functional requirements, dynamic configuration, pin safety, and AI agent development protocol. Each requirement follows EARS patterns and INCOSE quality rules to ensure clarity, testability, and completeness. The requirements prioritize safety through multiple interlocks while providing a user-friendly interface with efficient memory usage and proper ESP8266 pin management. The system uses dynamic configuration constants to allow easy parameter adjustments without code modifications, and follows a strict 6-step development protocol with user hardware testing at each phase.
