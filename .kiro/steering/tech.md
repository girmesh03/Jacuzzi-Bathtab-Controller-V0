# Technology Stack

## Build System

**PlatformIO** - Modern embedded development platform for ESP8266

### Platform Configuration
- **Platform**: espressif8266
- **Board**: esp12e (ESP-12E/ESP-12F module)
- **Framework**: Arduino
- **Monitor Speed**: 115200 baud
- **Upload Speed**: 921600 baud (fast upload for development)

### Build Flags
- `-Os` - Optimize for size (DO NOT use `-flto` - causes compilation errors on ESP8266)
- `-DENABLE_SERIAL_DEBUG` - Enable debug output (remove for production builds)

### Source Filter
```ini
build_src_filter =
    +<*>                    # Include all source files
    +<../lib/*/*.cpp>       # Include library implementations
```

## Core Libraries

### Display & Graphics
- **Adafruit SH110X** (^2.1.14) - SH1106 OLED driver with I2C support
- **Adafruit GFX Library** (^1.11.0) - Graphics primitives and fonts
- **Adafruit BusIO** (^1.14.0) - I2C/SPI abstraction layer

### Sensors
- **OneWire** (^2.3.7) - OneWire protocol for DS18B20
- **DallasTemperature** (^3.11.0) - DS18B20 temperature sensor driver with error handling

### I/O
- **Wire** (Arduino built-in) - I2C communication for OLED and PCF8574
- **EEPROM** (ESP8266 built-in) - Settings persistence (4 KB emulated in Flash)
- Custom PCF8574 wrapper for relay control

## Common Commands

### Build & Upload
```bash
# Build project (AI agent requests user to run)
pio run

# Upload to device (AI agent requests user to run)
pio run --target upload

# Check memory usage (AI agent requests user to run)
pio run --target size

# Clean build artifacts
pio run --target clean
```

### Monitoring & Debugging
```bash
# Open serial monitor at 115200 baud
pio device monitor

# Monitor with specific baud rate
pio device monitor -b 115200

# Build, upload, and monitor (combined workflow)
pio run -t upload && pio device monitor
```

### Project Management
```bash
# Update all libraries to latest compatible versions
pio pkg update

# List installed libraries with versions
pio pkg list

# Install specific library version
pio pkg install "adafruit/Adafruit SH110X@^2.1.14"

# Remove unused libraries
pio pkg uninstall <library-name>
```

## Memory Optimization

### Flash Memory (4 MB available)
- Use `F()` macro for all string literals to store in PROGMEM
- Use `PROGMEM` for constant arrays and lookup tables
- Store menu strings in PROGMEM: `const char STR_NAME[] PROGMEM = "Text";`
- Enable `-Os` optimization flag (size optimization)
- Avoid `-flto` flag (causes linker errors on ESP8266)
- **Current usage**: ~97 KB / 4 MB (2.43%) - Phase 1
- **Final estimate**: ~160 KB / 4 MB (4%) - Phase 10

### SRAM (80 KB available)
- Use `uint8_t` for 8-bit values instead of `int`
- Use `uint16_t` for 16-bit values instead of `int`
- Use `uint32_t` for 32-bit values (timestamps with millis())
- Minimize global variables
- Use stack allocation for temporary data
- Prefer pointer-based allocation for large objects (display buffer)
- Use bit fields for boolean flags: `uint8_t flags;` with bit masks
- **Current usage**: ~5.1 KB / 80 KB (6.33%) - Phase 1
- **Final estimate**: ~21 KB / 80 KB (26%) - Phase 10

### EEPROM (4 KB emulated in Flash)
- Store settings: target temperature, idle timeout, auto heater mode
- Use checksum validation for data integrity
- Delayed writes (1 second after last change) to minimize wear
- **Current usage**: 32 bytes for settings structure

## Debug Macros

Conditional compilation for debug output (zero overhead when disabled):

```cpp
#ifdef ENABLE_SERIAL_DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif
```

**Usage in code**:
```cpp
DEBUG_PRINTLN("System initialized");
DEBUG_PRINT("Temperature: ");
DEBUG_PRINTLN(temp);
```

**Production build**: Remove `-DENABLE_SERIAL_DEBUG` from platformio.ini

## I2C Configuration

ESP8266-specific I2C initialization with explicit pin specification:

```cpp
Wire.begin(PIN_SDA, PIN_SCL);  // MUST specify pins for ESP8266
// SDA = GPIO4 (D2)
// SCL = GPIO5 (D1)
```

**I2C Devices**:
- OLED Display: Address 0x3C
- PCF8574 I/O Expander: Address 0x20

**I2C Error Handling**:
- Retry failed transactions up to 3 times
- Exponential backoff: 10ms, 50ms, 100ms
- Enter fault state after 3 consecutive failures

## ESP8266 Considerations

### Boot Requirements (CRITICAL)
- **GPIO15 (D8)**: MUST have external 10kΩ pull-down resistor to GND for boot
- **GPIO0 (D3)**: Must be HIGH during boot (internal pull-up provides this)
- **GPIO2 (D4)**: Must be HIGH during boot (internal pull-up provides this)
- **GPIO16 (D0)**: Avoid - limited functionality, used for deep sleep wake

### Safe GPIO Pins
- **GPIO4 (D2)**: I2C SDA - safe
- **GPIO5 (D1)**: I2C SCL - safe
- **GPIO12 (D6)**: Buzzer - safe, no boot issues
- **GPIO13 (D7)**: Encoder CLK - safe
- **GPIO14 (D5)**: DS18B20 - safe, no boot issues

### Programming & Operation
- Always call `yield()` in loops to prevent watchdog reset
- Use non-blocking delays with `millis()` (never use `delay()`)
- Main loop should complete in < 50ms for responsive operation
- Avoid GPIO6-GPIO11 (connected to internal flash)

### OneWire on ESP8266
- DS18B20 on GPIO14 (D5)
- Requires 4.7kΩ pull-up resistor to 3.3V (NOT 5V)
- 12-bit resolution: 750ms conversion time
- Non-blocking reads: request temperature, wait, then read

### EEPROM Emulation
- ESP8266 emulates EEPROM in Flash memory
- Must call `EEPROM.begin(size)` before use
- Must call `EEPROM.commit()` after writes
- Limit write frequency to minimize Flash wear

## Development Workflow

### AI Agent Protocol
1. **Request compilation**: "Please run: `pio run`"
2. **Wait for output**: Analyze compilation results
3. **Request upload**: "Please run: `pio run --target upload`"
4. **Wait for confirmation**: User confirms upload success
5. **Request memory check**: "Please run: `pio run --target size`"
6. **Wait for memory usage**: Verify within phase budget

### User Testing Protocol
1. **AI agent requests**: "**Please test the following on hardware and provide feedback:**"
2. **List specific tests**: Detailed test cases for current phase
3. **Wait for feedback**: User tests on actual ESP8266 hardware
4. **Iterate on issues**: Fix problems, recompile, retest
5. **Confirm working**: User explicitly approves before proceeding

## Timing Constraints

### Real-Time Requirements
- **Main loop**: < 50ms per iteration
- **Sensor updates**: Temperature every 2000ms, water level every 500ms
- **Display updates**: Minimum 10 FPS (100ms frame time)
- **Safety checks**: Every loop iteration
- **Interlock response**: < 100ms for water level and heater interlocks
- **Thermal runaway**: < 500ms detection and response

### Non-Blocking Patterns
```cpp
// Temperature reading (non-blocking)
uint32_t lastTempRead = 0;
const uint32_t TEMP_INTERVAL = 2000;

void loop() {
    if (millis() - lastTempRead >= TEMP_INTERVAL) {
        sensors.requestTemperatures();  // Non-blocking request
        lastTempRead = millis();
    }
    yield();  // Allow ESP8266 background tasks
}
```

## Common Issues & Solutions

### Compilation Errors
- **"Wire.h not found"**: Include `<Wire.h>` in source files
- **"undefined reference to Wire"**: Add `Wire.begin()` in setup()
- **Link errors with -flto**: Remove `-flto` flag from platformio.ini

### Upload Failures
- **"Failed to connect"**: Press RESET button on ESP8266 during upload
- **"Timed out waiting for packet header"**: Check USB cable and drivers
- **"espcomm_upload_mem failed"**: Reduce upload speed in platformio.ini

### Runtime Issues
- **Watchdog reset**: Add `yield()` calls in long loops
- **I2C communication failure**: Verify SDA/SCL connections and addresses
- **Temperature reads -127°C**: Check DS18B20 wiring and pull-up resistor
- **Boot loop**: Verify GPIO15 has external 10kΩ pull-down resistor

## References

- [ESP8266 Arduino Core Documentation](https://arduino-esp8266.readthedocs.io/)
- [PlatformIO ESP8266 Platform](https://docs.platformio.org/en/latest/platforms/espressif8266.html)
- [ESP8266 Pinout Reference](https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/)
- [ESP8266 Boot Mode Selection](https://github.com/esp8266/esp8266-wiki/wiki/Boot-Process)
