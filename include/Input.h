// ============================================================================
// INPUT.H - User Input Management Module
// ============================================================================
// Handles rotary encoder input (KY-040) and buzzer feedback for the Jacuzzi
// Controller System. Implements debouncing, edge detection, and non-blocking
// buzzer control.
//
// Target Platform: ESP8266 (ESP-12E/ESP-12F)
// Phase 3: Rotary Encoder Input
// ============================================================================

#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#include "Constants.h"

// ============================================================================
// ENCODER EVENT ENUMERATION
// ============================================================================
enum EncoderEvent {
    ENCODER_NONE,      // No event
    ENCODER_CW,        // Clockwise rotation
    ENCODER_CCW,       // Counter-clockwise rotation
    ENCODER_BUTTON     // Button press
};

// ============================================================================
// INPUT MANAGER CLASS
// ============================================================================
class InputManager {
public:
    void init();
    void update();  // Non-blocking, call every loop
    
    bool hasEvent();
    EncoderEvent getEvent();
    
private:
    // Encoder state tracking
    uint8_t lastCLK;
    uint8_t lastDT;
    uint8_t lastSW;
    
    // Debouncing timing
    uint32_t lastEncoderTime;
    uint32_t lastButtonTime;
    
    // Event queue (single event)
    EncoderEvent pendingEvent;
};

#endif // INPUT_H
