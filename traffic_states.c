/*
 * traffic_states.c
 * 
 * Implementation of all 43 traffic signal states
 * 
 * Author: Edwin Sanchez
 * Date: February 2026
 */

#include "traffic_states.h"
#include <string.h>

// ============================================
// HELPER FUNCTIONS
// ============================================

/**
 * Initialize LED state structure
 */
void initLEDState(LEDState *state) {
    memset(state->byte, 0, 4);
}

/**
 * Clear all LEDs (all off)
 */
void clearAllLEDs(LEDState *state) {
    memset(state->byte, 0, 4);
}

/**
 * Set all signals to RED
 */
void setAllRed(LEDState *state) {
    clearAllLEDs(state);
    
    // Set all red LEDs
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * Set all signals to YELLOW (emergency warning)
 */
void setAllYellow(LEDState *state) {
    clearAllLEDs(state);
    
    // Set all yellow LEDs
    setLED(state, NS_MAIN_YELLOW, true);
    setLED(state, N_LEFT_YELLOW_STEADY, true);
    setLED(state, S_LEFT_YELLOW_STEADY, true);
    setLED(state, S_RIGHT_YELLOW, true);
    setLED(state, W_THRU_YELLOW, true);
    setLED(state, W_RIGHT_YELLOW, true);
    setLED(state, E_THRU_YELLOW, true);
    setLED(state, E_LEFT_YELLOW, true);
}

/**
 * Set individual LED on or off
 */
void setLED(LEDState *state, uint8_t ledNumber, bool on) {
    if (ledNumber >= 32) return;  // Out of range
    
    uint8_t byteIndex = ledNumber / 8;
    uint8_t bitIndex = ledNumber % 8;
    
    if (on) {
        state->byte[byteIndex] |= (1 << bitIndex);
    } else {
        state->byte[byteIndex] &= ~(1 << bitIndex);
    }
}

/**
 * Get individual LED state
 */
bool getLED(LEDState *state, uint8_t ledNumber) {
    if (ledNumber >= 32) return false;
    
    uint8_t byteIndex = ledNumber / 8;
    uint8_t bitIndex = ledNumber % 8;
    
    return (state->byte[byteIndex] & (1 << bitIndex)) != 0;
}

// ============================================
// DAYTIME NORMAL MODE STATES (0-17)
// ============================================

/**
 * STATE 0: University Through Green
 * Duration: 30 seconds
 */
void setState_0_UnivGreen(LEDState *state) {
    clearAllLEDs(state);
    
    // NS Main green
    setLED(state, NS_MAIN_GREEN, true);
    
    // North left red (waiting)
    setLED(state, N_LEFT_RED, true);
    
    // South left red (waiting)
    setLED(state, S_LEFT_RED, true);
    
    // South right permissive (green ball)
    setLED(state, S_RIGHT_GREEN_BALL, true);
    
    // West all red
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    
    // East all red
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 1: University Through Yellow
 * Duration: 4 seconds
 */
void setState_1_UnivYellow(LEDState *state) {
    clearAllLEDs(state);
    
    // NS Main yellow
    setLED(state, NS_MAIN_YELLOW, true);
    
    // Left turns red
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    
    // South right yellow
    setLED(state, S_RIGHT_YELLOW, true);
    
    // 8th Street all red
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 2: All Red Safety Gap #1
 * Duration: 2 seconds
 */
void setState_2_AllRed1(LEDState *state) {
    setAllRed(state);
}

/**
 * STATE 3: North Left Turn Protected
 * Duration: 15 seconds
 */
void setState_3_NLeftGreen(LEDState *state) {
    clearAllLEDs(state);
    
    // NS Main red
    setLED(state, NS_MAIN_RED, true);
    
    // North left GREEN ARROW (protected)
    setLED(state, N_LEFT_GREEN_ARROW, true);
    
    // South left red
    setLED(state, S_LEFT_RED, true);
    
    // South right red
    setLED(state, S_RIGHT_RED, true);
    
    // 8th Street all red
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 4: North Left Turn Yellow
 * Duration: 3 seconds
 */
void setState_4_NLeftYellow(LEDState *state) {
    clearAllLEDs(state);
    
    // NS Main red
    setLED(state, NS_MAIN_RED, true);
    
    // North left YELLOW STEADY
    setLED(state, N_LEFT_YELLOW_STEADY, true);
    
    // Everything else red
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 5: All Red Safety Gap #2
 * Duration: 2 seconds
 */
void setState_5_AllRed2(LEDState *state) {
    setAllRed(state);
}

/**
 * STATE 6: South Left Turn Protected
 * Duration: 15 seconds
 */
void setState_6_SLeftGreen(LEDState *state) {
    clearAllLEDs(state);
    
    // NS Main red
    setLED(state, NS_MAIN_RED, true);
    
    // North left red
    setLED(state, N_LEFT_RED, true);
    
    // South left GREEN ARROW (protected)
    setLED(state, S_LEFT_GREEN_ARROW, true);
    
    // South right red
    setLED(state, S_RIGHT_RED, true);
    
    // 8th Street all red
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 7: South Left Turn Yellow
 * Duration: 3 seconds
 */
void setState_7_SLeftYellow(LEDState *state) {
    clearAllLEDs(state);
    
    // NS Main red
    setLED(state, NS_MAIN_RED, true);
    
    // North left red
    setLED(state, N_LEFT_RED, true);
    
    // South left YELLOW STEADY
    setLED(state, S_LEFT_YELLOW_STEADY, true);
    
    // Everything else red
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 8: All Red Safety Gap #3
 * Duration: 2 seconds
 */
void setState_8_AllRed3(LEDState *state) {
    setAllRed(state);
}

/**
 * STATE 9: Westbound 8th Street Green
 * Duration: 20 seconds
 */
void setState_9_WestGreen(LEDState *state) {
    clearAllLEDs(state);
    
    // University all red
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    
    // West through green
    setLED(state, W_THRU_GREEN, true);
    
    // West right permissive (green ball)
    setLED(state, W_RIGHT_GREEN_BALL, true);
    
    // East all red (split-phase!)
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 10: Westbound Yellow
 * Duration: 4 seconds
 */
void setState_10_WestYellow(LEDState *state) {
    clearAllLEDs(state);
    
    // University all red
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    
    // West yellow
    setLED(state, W_THRU_YELLOW, true);
    setLED(state, W_RIGHT_YELLOW, true);
    
    // East all red
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 11: All Red Safety Gap #4 (Split-Phase)
 * Duration: 2 seconds
 */
void setState_11_AllRed4(LEDState *state) {
    setAllRed(state);
}

/**
 * STATE 12: Eastbound Through Green
 * Duration: 20 seconds
 */
void setState_12_EastThruGreen(LEDState *state) {
    clearAllLEDs(state);
    
    // University all red
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    
    // West all red (split-phase!)
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    
    // East through green
    setLED(state, E_THRU_GREEN, true);
    
    // East left red (waiting for protected phase)
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 13: Eastbound Through Yellow
 * Duration: 4 seconds
 */
void setState_13_EastThruYellow(LEDState *state) {
    clearAllLEDs(state);
    
    // University all red
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    
    // West all red
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    
    // East through yellow
    setLED(state, E_THRU_YELLOW, true);
    
    // East left red
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 14: All Red Safety Gap #5
 * Duration: 2 seconds
 */
void setState_14_AllRed5(LEDState *state) {
    setAllRed(state);
}

/**
 * STATE 15: Eastbound Left Turn Protected
 * Duration: 12 seconds
 */
void setState_15_ELeftGreen(LEDState *state) {
    clearAllLEDs(state);
    
    // University all red
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    
    // West all red
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    
    // East through red
    setLED(state, E_THRU_RED, true);
    
    // East left GREEN ARROW (protected)
    setLED(state, E_LEFT_GREEN_ARROW, true);
}

/**
 * STATE 16: Eastbound Left Turn Yellow
 * Duration: 3 seconds
 */
void setState_16_ELeftYellow(LEDState *state) {
    clearAllLEDs(state);
    
    // Everything red except east left
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    
    // East left yellow
    setLED(state, E_LEFT_YELLOW, true);
}

/**
 * STATE 17: All Red Safety Gap #6 (Cycle End)
 * Duration: 2 seconds
 */
void setState_17_AllRed6(LEDState *state) {
    setAllRed(state);
}

// ============================================
// HIGH TRAFFIC MODE STATES (18-36)
// ============================================

/**
 * STATE 18: North Priority Phase (Leading Protected)
 * Duration: 40 seconds
 */
void setState_18_NorthPriority(LEDState *state) {
    clearAllLEDs(state);
    
    // NS Main green (northbound priority!)
    setLED(state, NS_MAIN_GREEN, true);
    
    // North left GREEN ARROW SIMULTANEOUS!
    setLED(state, N_LEFT_GREEN_ARROW, true);
    
    // South all red
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    
    // 8th Street all red
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 19: North Left Arrow Ending
 * Duration: 3 seconds
 */
void setState_19_NArrowEnding(LEDState *state) {
    clearAllLEDs(state);
    
    // NS Main STILL GREEN!
    setLED(state, NS_MAIN_GREEN, true);
    
    // North left arrow YELLOW (ending)
    setLED(state, N_LEFT_YELLOW_STEADY, true);
    
    // Everything else red
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 20: North Through Only (Bonus Time)
 * Duration: 10 seconds
 */
void setState_20_NThruBonus(LEDState *state) {
    clearAllLEDs(state);
    
    // NS Main green (bonus time!)
    setLED(state, NS_MAIN_GREEN, true);
    
    // North left RED now
    setLED(state, N_LEFT_RED, true);
    
    // Everything else red
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 21: North Through Yellow
 * Duration: 5 seconds (longer in high traffic)
 */
void setState_21_NThruYellow(LEDState *state) {
    clearAllLEDs(state);
    
    // NS Main yellow
    setLED(state, NS_MAIN_YELLOW, true);
    
    // Everything else red
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 22: All Red High Traffic #1
 * Duration: 2 seconds
 */
void setState_22_AllRedHT1(LEDState *state) {
    setAllRed(state);
}

/**
 * STATE 23: South Through Green (High Traffic)
 * Duration: 25 seconds
 */
void setState_23_SouthGreenHT(LEDState *state) {
    clearAllLEDs(state);
    
    // NS Main green (southbound only in high traffic)
    setLED(state, NS_MAIN_GREEN, true);
    
    // North left red
    setLED(state, N_LEFT_RED, true);
    
    // South left red (waiting)
    setLED(state, S_LEFT_RED, true);
    
    // South right permissive
    setLED(state, S_RIGHT_GREEN_BALL, true);
    
    // 8th Street all red
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 24: South Through Yellow (High Traffic)
 * Duration: 4 seconds
 */
void setState_24_SouthYellowHT(LEDState *state) {
    clearAllLEDs(state);
    
    // NS Main yellow
    setLED(state, NS_MAIN_YELLOW, true);
    
    // North left red
    setLED(state, N_LEFT_RED, true);
    
    // South left red
    setLED(state, S_LEFT_RED, true);
    
    // South right yellow
    setLED(state, S_RIGHT_YELLOW, true);
    
    // 8th Street all red
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 25: All Red High Traffic #2
 * Duration: 2 seconds
 */
void setState_25_AllRedHT2(LEDState *state) {
    setAllRed(state);
}

/**
 * STATE 26: South Left Turn (High Traffic, Optional)
 * Duration: 12 seconds
 */
void setState_26_SLeftGreenHT(LEDState *state) {
    clearAllLEDs(state);
    
    // NS Main red
    setLED(state, NS_MAIN_RED, true);
    
    // North left red
    setLED(state, N_LEFT_RED, true);
    
    // South left GREEN ARROW
    setLED(state, S_LEFT_GREEN_ARROW, true);
    
    // South right red
    setLED(state, S_RIGHT_RED, true);
    
    // 8th Street all red
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 27: South Left Yellow (High Traffic)
 * Duration: 3 seconds
 */
void setState_27_SLeftYellowHT(LEDState *state) {
    clearAllLEDs(state);
    
    // NS Main red
    setLED(state, NS_MAIN_RED, true);
    
    // North left red
    setLED(state, N_LEFT_RED, true);
    
    // South left YELLOW
    setLED(state, S_LEFT_YELLOW_STEADY, true);
    
    // Everything else red
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 28: All Red High Traffic #3
 * Duration: 2 seconds
 */
void setState_28_AllRedHT3(LEDState *state) {
    setAllRed(state);
}

/**
 * STATE 29: Westbound Green (High Traffic, Reduced)
 * Duration: 15 seconds
 */
void setState_29_WestGreenHT(LEDState *state) {
    clearAllLEDs(state);
    
    // University all red
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    
    // West through green
    setLED(state, W_THRU_GREEN, true);
    
    // West right permissive
    setLED(state, W_RIGHT_GREEN_BALL, true);
    
    // East all red
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 30: Westbound Yellow (High Traffic)
 * Duration: 4 seconds
 */
void setState_30_WestYellowHT(LEDState *state) {
    clearAllLEDs(state);
    
    // University all red
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    
    // West yellow
    setLED(state, W_THRU_YELLOW, true);
    setLED(state, W_RIGHT_YELLOW, true);
    
    // East all red
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 31: All Red High Traffic #4 (Split-Phase)
 * Duration: 2 seconds
 */
void setState_31_AllRedHT4(LEDState *state) {
    setAllRed(state);
}

/**
 * STATE 32: Eastbound Through Green (High Traffic, Reduced)
 * Duration: 15 seconds
 */
void setState_32_EastThruGreenHT(LEDState *state) {
    clearAllLEDs(state);
    
    // University all red
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    
    // West all red
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    
    // East through green
    setLED(state, E_THRU_GREEN, true);
    
    // East left red
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 33: Eastbound Through Yellow (High Traffic)
 * Duration: 4 seconds
 */
void setState_33_EastThruYellowHT(LEDState *state) {
    clearAllLEDs(state);
    
    // University all red
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    
    // West all red
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    
    // East through yellow
    setLED(state, E_THRU_YELLOW, true);
    
    // East left red
    setLED(state, E_LEFT_RED, true);
}

/**
 * STATE 34: All Red High Traffic #5
 * Duration: 2 seconds
 */
void setState_34_AllRedHT5(LEDState *state) {
    setAllRed(state);
}

/**
 * STATE 35: Eastbound Left Turn (High Traffic, Optional)
 * Duration: 12 seconds
 */
void setState_35_ELeftGreenHT(LEDState *state) {
    clearAllLEDs(state);
    
    // Everything red except east left
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    
    // East left GREEN ARROW
    setLED(state, E_LEFT_GREEN_ARROW, true);
}

/**
 * STATE 36: Eastbound Left Yellow & All Red (High Traffic)
 * Duration: 5 seconds (3s yellow + 2s all red combined)
 * Note: Implementation should handle this as two sub-states
 */
void setState_36_ELeftYellowHT(LEDState *state) {
    // First 3 seconds: Yellow
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_YELLOW, true);
    
    // After 3s, transition to all red for 2s
    // (This should be handled in state machine timing logic)
}

// ============================================
// NIGHT MODE STATES (37-38)
// ============================================

/**
 * STATE 37: Night Mode - All Off
 * Duration: 1 second
 */
void setState_37_NightOff(LEDState *state) {
    clearAllLEDs(state);  // All lights off
}

/**
 * STATE 38: Night Mode - Flash On
 * Duration: 1 second
 */
void setState_38_NightOn(LEDState *state) {
    clearAllLEDs(state);
    
    // University: GREEN (proceed with caution)
    setLED(state, NS_MAIN_GREEN, true);
    
    // 8th Street: RED (must stop first)
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    
    // Left turn signals off
    // Right turn signals off
}

// ============================================
// EMERGENCY MODE STATES (39-42)
// ============================================

/**
 * STATE 39: Emergency Warning - All Yellow
 * Duration: 3 seconds
 */
void setState_39_EmergWarning(LEDState *state) {
    setAllYellow(state);
}

/**
 * STATE 40: Emergency Hold - All Red
 * Duration: Until button released (minimum 5 seconds)
 */
void setState_40_EmergHold(LEDState *state) {
    setAllRed(state);
}

/**
 * STATE 41: Emergency Clearing - All Red
 * Duration: 3 seconds
 */
void setState_41_EmergClearing(LEDState *state) {
    setAllRed(state);
}

/**
 * STATE 42: Emergency Return - All Red
 * Duration: 2 seconds
 */
void setState_42_EmergReturn(LEDState *state) {
    setAllRed(state);
}

// ============================================
// STATE EXECUTION DISPATCHER
// ============================================

/**
 * Execute a specific state
 */
void executeState(LEDState *state, TrafficState currentState) {
    switch (currentState) {
        // Daytime mode
        case STATE_UNIV_GREEN:          setState_0_UnivGreen(state); break;
        case STATE_UNIV_YELLOW:         setState_1_UnivYellow(state); break;
        case STATE_ALL_RED_1:           setState_2_AllRed1(state); break;
        case STATE_N_LEFT_GREEN:        setState_3_NLeftGreen(state); break;
        case STATE_N_LEFT_YELLOW:       setState_4_NLeftYellow(state); break;
        case STATE_ALL_RED_2:           setState_5_AllRed2(state); break;
        case STATE_S_LEFT_GREEN:        setState_6_SLeftGreen(state); break;
        case STATE_S_LEFT_YELLOW:       setState_7_SLeftYellow(state); break;
        case STATE_ALL_RED_3:           setState_8_AllRed3(state); break;
        case STATE_WEST_GREEN:          setState_9_WestGreen(state); break;
        case STATE_WEST_YELLOW:         setState_10_WestYellow(state); break;
        case STATE_ALL_RED_4:           setState_11_AllRed4(state); break;
        case STATE_EAST_THRU_GREEN:     setState_12_EastThruGreen(state); break;
        case STATE_EAST_THRU_YELLOW:    setState_13_EastThruYellow(state); break;
        case STATE_ALL_RED_5:           setState_14_AllRed5(state); break;
        case STATE_E_LEFT_GREEN:        setState_15_ELeftGreen(state); break;
        case STATE_E_LEFT_YELLOW:       setState_16_ELeftYellow(state); break;
        case STATE_ALL_RED_6:           setState_17_AllRed6(state); break;
        
        // High traffic mode
        case STATE_NORTH_PRIORITY:      setState_18_NorthPriority(state); break;
        case STATE_N_ARROW_ENDING:      setState_19_NArrowEnding(state); break;
        case STATE_N_THRU_BONUS:        setState_20_NThruBonus(state); break;
        case STATE_N_THRU_YELLOW:       setState_21_NThruYellow(state); break;
        case STATE_ALL_RED_HT_1:        setState_22_AllRedHT1(state); break;
        case STATE_SOUTH_GREEN_HT:      setState_23_SouthGreenHT(state); break;
        case STATE_SOUTH_YELLOW_HT:     setState_24_SouthYellowHT(state); break;
        case STATE_ALL_RED_HT_2:        setState_25_AllRedHT2(state); break;
        case STATE_S_LEFT_GREEN_HT:     setState_26_SLeftGreenHT(state); break;
        case STATE_S_LEFT_YELLOW_HT:    setState_27_SLeftYellowHT(state); break;
        case STATE_ALL_RED_HT_3:        setState_28_AllRedHT3(state); break;
        case STATE_WEST_GREEN_HT:       setState_29_WestGreenHT(state); break;
        case STATE_WEST_YELLOW_HT:      setState_30_WestYellowHT(state); break;
        case STATE_ALL_RED_HT_4:        setState_31_AllRedHT4(state); break;
        case STATE_EAST_THRU_GREEN_HT:  setState_32_EastThruGreenHT(state); break;
        case STATE_EAST_THRU_YELLOW_HT: setState_33_EastThruYellowHT(state); break;
        case STATE_ALL_RED_HT_5:        setState_34_AllRedHT5(state); break;
        case STATE_E_LEFT_GREEN_HT:     setState_35_ELeftGreenHT(state); break;
        case STATE_E_LEFT_YELLOW_HT:    setState_36_ELeftYellowHT(state); break;
        
        // Night mode
        case STATE_NIGHT_OFF:           setState_37_NightOff(state); break;
        case STATE_NIGHT_ON:            setState_38_NightOn(state); break;
        
        // Emergency mode
        case STATE_EMERG_WARNING:       setState_39_EmergWarning(state); break;
        case STATE_EMERG_HOLD:          setState_40_EmergHold(state); break;
        case STATE_EMERG_CLEARING:      setState_41_EmergClearing(state); break;
        case STATE_EMERG_RETURN:        setState_42_EmergReturn(state); break;
        
        default:
            // Unknown state - go to all red for safety
            setAllRed(state);
            break;
    }
}

/**
 * Get the duration for a given state (in milliseconds)
 */
uint32_t getStateDuration(TrafficState state) {
    switch (state) {
        // Daytime normal
        case STATE_UNIV_GREEN:          return TIME_UNIV_GREEN;
        case STATE_UNIV_YELLOW:         return TIME_YELLOW;
        case STATE_ALL_RED_1:           return TIME_ALL_RED;
        case STATE_N_LEFT_GREEN:        return TIME_N_LEFT_GREEN;
        case STATE_N_LEFT_YELLOW:       return TIME_YELLOW_ARROW;
        case STATE_ALL_RED_2:           return TIME_ALL_RED;
        case STATE_S_LEFT_GREEN:        return TIME_S_LEFT_GREEN;
        case STATE_S_LEFT_YELLOW:       return TIME_YELLOW_ARROW;
        case STATE_ALL_RED_3:           return TIME_ALL_RED;
        case STATE_WEST_GREEN:          return TIME_WEST_GREEN;
        case STATE_WEST_YELLOW:         return TIME_YELLOW;
        case STATE_ALL_RED_4:           return TIME_ALL_RED;
        case STATE_EAST_THRU_GREEN:     return TIME_EAST_THRU_GREEN;
        case STATE_EAST_THRU_YELLOW:    return TIME_YELLOW;
        case STATE_ALL_RED_5:           return TIME_ALL_RED;
        case STATE_E_LEFT_GREEN:        return TIME_E_LEFT_GREEN;
        case STATE_E_LEFT_YELLOW:       return TIME_YELLOW_ARROW;
        case STATE_ALL_RED_6:           return TIME_ALL_RED;
        
        // High traffic mode
        case STATE_NORTH_PRIORITY:      return TIME_N_PRIORITY;
        case STATE_N_ARROW_ENDING:      return TIME_N_ARROW_END;
        case STATE_N_THRU_BONUS:        return TIME_N_THRU_BONUS;
        case STATE_N_THRU_YELLOW:       return TIME_N_THRU_YELLOW_HT;
        case STATE_ALL_RED_HT_1:        return TIME_ALL_RED;
        case STATE_SOUTH_GREEN_HT:      return TIME_SOUTH_GREEN_HT;
        case STATE_SOUTH_YELLOW_HT:     return TIME_YELLOW;
        case STATE_ALL_RED_HT_2:        return TIME_ALL_RED;
        case STATE_S_LEFT_GREEN_HT:     return TIME_S_LEFT_GREEN_HT;
        case STATE_S_LEFT_YELLOW_HT:    return TIME_YELLOW_ARROW;
        case STATE_ALL_RED_HT_3:        return TIME_ALL_RED;
        case STATE_WEST_GREEN_HT:       return TIME_WEST_GREEN_HT;
        case STATE_WEST_YELLOW_HT:      return TIME_YELLOW;
        case STATE_ALL_RED_HT_4:        return TIME_ALL_RED;
        case STATE_EAST_THRU_GREEN_HT:  return TIME_EAST_THRU_HT;
        case STATE_EAST_THRU_YELLOW_HT: return TIME_YELLOW;
        case STATE_ALL_RED_HT_5:        return TIME_ALL_RED;
        case STATE_E_LEFT_GREEN_HT:     return TIME_S_LEFT_GREEN_HT;
        case STATE_E_LEFT_YELLOW_HT:    return TIME_YELLOW_ARROW + TIME_ALL_RED;
        
        // Night mode
        case STATE_NIGHT_OFF:           return TIME_NIGHT_FLASH;
        case STATE_NIGHT_ON:            return TIME_NIGHT_FLASH;
        
        // Emergency mode
        case STATE_EMERG_WARNING:       return TIME_EMERG_WARNING;
        case STATE_EMERG_HOLD:          return TIME_EMERG_HOLD_MIN;
        case STATE_EMERG_CLEARING:      return TIME_EMERG_CLEARING;
        case STATE_EMERG_RETURN:        return TIME_EMERG_RETURN;
        
        default:
            return TIME_ALL_RED;  // Safe default
    }
}