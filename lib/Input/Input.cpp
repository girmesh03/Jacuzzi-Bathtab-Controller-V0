// ============================================================================
// INPUT.CPP - User Input Management Implementation
// ============================================================================
// Implements rotary encoder reading with debouncing and buzzer feedback
// Non-blocking operation using millis()-based timing
//
// Target Platform: ESP8266 (ESP-12E/ESP-12F)
// Phase 3: Rotary Encoder Input
// ============================================================================

#include "Input.h"
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
// BUZZER STATE MANAGEMENT (File Scope)
// ============================================================================
static bool buzzerActive = false;
static uint32_t buzzerStartTime = 0;
static uint16_t buzzerDuration = 0;

// ============================================================================
// BUZZER CONTROL FUNCTIONS
// ============================================================================

/**
 * @brief Start buzzer for specified duration
 * @param duration Duration in milliseconds
 */
void startBuzzer(uint16_t duration)
{
    buzzerActive = true;
    buzzerStartTime = millis();
    buzzerDuration = duration;
    digitalWrite(PIN_BUZZER, HIGH);
    
    DEBUG_PRINT("Buzzer ON for ");
    DEBUG_PRINT(duration);
    DEBUG_PRINTLN("ms");
}

/**
 * @brief Update buzzer state (non-blocking)
 * Call this every loop iteration to handle buzzer timing
 */
void updateBuzzer()
{
    if (buzzerActive)
    {
        if (millis() - buzzerStartTime >= buzzerDuration)
        {
            digitalWrite(PIN_BUZZER, LOW);
            buzzerActive = false;
            DEBUG_PRINTLN("Buzzer OFF");
        }
    }
}

// ============================================================================
// INPUT MANAGER IMPLEMENTATION
// ============================================================================

/**
 * @brief Initialize input hardware (encoder and buzzer)
 * 
 * Configures GPIO pins for rotary encoder and buzzer.
 * Reads initial pin states for edge detection.
 * 
 * CRITICAL: PIN_ENCODER_DT (GPIO15/D8) requires external 10kΩ pull-down
 * resistor to GND for proper ESP8266 boot! Without this resistor, the
 * ESP8266 may fail to boot or enter flash programming mode.
 */
void InputManager::init()
{
    DEBUG_PRINTLN("Initializing input manager...");
    
    // ========================================================================
    // CONFIGURE ROTARY ENCODER PINS
    // ========================================================================
    // CLK (D7 / GPIO13) - Rotation detection, safe pin
    pinMode(PIN_ENCODER_CLK, INPUT_PULLUP);
    
    // DT (D8 / GPIO15) - Direction detection
    // **CRITICAL**: Requires external 10kΩ pull-down resistor to GND!
    // GPIO15 must be LOW during boot for normal operation.
    // Internal pull-up is enabled here for encoder operation, but external
    // pull-down ensures proper boot behavior.
    pinMode(PIN_ENCODER_DT, INPUT_PULLUP);
    
    // SW (D3 / GPIO0) - Button press, safe with internal pull-up
    pinMode(PIN_ENCODER_SW, INPUT_PULLUP);
    
    DEBUG_PRINTLN("Encoder pins configured:");
    DEBUG_PRINT("  CLK (D7/GPIO13): INPUT_PULLUP");
    DEBUG_PRINTLN(" - safe");
    DEBUG_PRINT("  DT  (D8/GPIO15): INPUT_PULLUP");
    DEBUG_PRINTLN(" - REQUIRES 10kΩ pull-down!");
    DEBUG_PRINT("  SW  (D3/GPIO0):  INPUT_PULLUP");
    DEBUG_PRINTLN(" - safe");
    
    // ========================================================================
    // CONFIGURE BUZZER PIN
    // ========================================================================
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);  // Ensure buzzer is OFF initially
    
    DEBUG_PRINTLN("Buzzer pin (D6/GPIO12) configured: OUTPUT, initially LOW");
    
    // ========================================================================
    // READ INITIAL PIN STATES
    // ========================================================================
    lastCLK = digitalRead(PIN_ENCODER_CLK);
    lastDT = digitalRead(PIN_ENCODER_DT);
    lastSW = digitalRead(PIN_ENCODER_SW);
    
    DEBUG_PRINT("Initial pin states: CLK=");
    DEBUG_PRINT(lastCLK ? "HIGH" : "LOW");
    DEBUG_PRINT(", DT=");
    DEBUG_PRINT(lastDT ? "HIGH" : "LOW");
    DEBUG_PRINT(", SW=");
    DEBUG_PRINTLN(lastSW ? "HIGH" : "LOW");
    
    // ========================================================================
    // INITIALIZE TIMING VARIABLES
    // ========================================================================
    lastEncoderTime = 0;
    lastButtonTime = 0;
    
    // ========================================================================
    // INITIALIZE EVENT QUEUE
    // ========================================================================
    pendingEvent = ENCODER_NONE;
    
    DEBUG_PRINTLN("Input manager initialized successfully");
}

/**
 * @brief Update input state (non-blocking)
 * 
 * Reads encoder and button states, detects events with debouncing,
 * and manages buzzer timing. Call this every loop iteration.
 * 
 * Encoder rotation detection uses edge detection on CLK signal:
 * - Falling edge on CLK triggers direction check
 * - Direction determined by DT state at falling edge
 * - DT HIGH = Clockwise, DT LOW = Counter-clockwise
 * 
 * Button detection uses falling edge on SW signal.
 * 
 * All events are debounced with ENCODER_DEBOUNCE_TIME (50ms).
 */
void InputManager::update()
{
    // ========================================================================
    // ENCODER ROTATION DETECTION
    // ========================================================================
    uint8_t currentCLK = digitalRead(PIN_ENCODER_CLK);
    uint8_t currentDT = digitalRead(PIN_ENCODER_DT);
    
    // Detect CLK falling edge (transition from HIGH to LOW)
    if (currentCLK != lastCLK && currentCLK == LOW)
    {
        // Check debounce time
        if (millis() - lastEncoderTime > ENCODER_DEBOUNCE_TIME)
        {
            // Determine rotation direction by reading DT state
            if (currentDT == HIGH)
            {
                // Clockwise rotation
                pendingEvent = ENCODER_CW;
                startBuzzer(BUZZER_BEEP_SHORT);
                
                DEBUG_PRINTLN("Encoder: Clockwise rotation detected");
            }
            else
            {
                // Counter-clockwise rotation
                pendingEvent = ENCODER_CCW;
                startBuzzer(BUZZER_BEEP_SHORT);
                
                DEBUG_PRINTLN("Encoder: Counter-clockwise rotation detected");
            }
            
            lastEncoderTime = millis();
        }
    }
    
    // Update last CLK state for next edge detection
    lastCLK = currentCLK;
    
    // ========================================================================
    // BUTTON PRESS DETECTION
    // ========================================================================
    uint8_t currentSW = digitalRead(PIN_ENCODER_SW);
    
    // Detect SW falling edge (button press)
    if (currentSW != lastSW && currentSW == LOW)
    {
        // Check debounce time
        if (millis() - lastButtonTime > ENCODER_DEBOUNCE_TIME)
        {
            pendingEvent = ENCODER_BUTTON;
            startBuzzer(BUZZER_BEEP_SHORT);
            
            DEBUG_PRINTLN("Encoder: Button press detected");
            
            lastButtonTime = millis();
        }
    }
    
    // Update last SW state for next edge detection
    lastSW = currentSW;
    
    // ========================================================================
    // UPDATE BUZZER STATE
    // ========================================================================
    updateBuzzer();
}

/**
 * @brief Check if an encoder event is pending
 * @return true if event is available, false otherwise
 */
bool InputManager::hasEvent()
{
    return pendingEvent != ENCODER_NONE;
}

/**
 * @brief Get and clear pending encoder event
 * @return EncoderEvent (ENCODER_NONE if no event pending)
 */
EncoderEvent InputManager::getEvent()
{
    EncoderEvent event = pendingEvent;
    pendingEvent = ENCODER_NONE;
    return event;
}
