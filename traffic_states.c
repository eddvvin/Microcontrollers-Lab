#include "traffic_states.h"
#include <string.h>

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

void initLEDState(LEDState *state) {
    memset(state->byte, 0, 4);
}

void clearAllLEDs(LEDState *state) {
    memset(state->byte, 0, 4);
}

void setAllRed(LEDState *state) {
    clearAllLEDs(state);
    // North
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);

    // South
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);

    // West
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);

    // East
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

void setAllYellow(LEDState *state) {
    clearAllLEDs(state);
    // North
    setLED(state, N_COMBO_YELLOW, true);
    setLED(state, N_THRU_YELLOW, true);

    // South
    setLED(state, S_COMBO_YELLOW, true);
    setLED(state, S_THRU_YELLOW, true);
    setLED(state, S_RIGHT_YELLOW, true);

    // West
    setLED(state, W_THRU_YELLOW, true);
    setLED(state, W_RIGHT_YELLOW, true);

    // East
    setLED(state, E_THRU_LEFT_YELLOW, true);
    setLED(state, E_THRU_RIGHT_YELLOW, true);
}

void setLED(LEDState *state, uint8_t ledNumber, bool on) {
    uint8_t byteIndex;
    uint8_t bitIndex;

    if (ledNumber >= 32) return;

    byteIndex = ledNumber / 8;
    bitIndex = ledNumber % 8;

    if (on) {
        state->byte[byteIndex] |= (1 << bitIndex);
    } else {
        state->byte[byteIndex] &= ~(1 << bitIndex);
    }
}

bool getLED(LEDState *state, uint8_t ledNumber) {
    uint8_t byteIndex;
    uint8_t bitIndex;

    if (ledNumber >= 32) return false;

    byteIndex = ledNumber / 8;
    bitIndex = ledNumber % 8;

    return (state->byte[byteIndex] & (1 << bitIndex)) != 0;
}

// ============================================================================
// DAYTIME MODE STATES (0-18)
// ============================================================================

// State 0: North-South Green
void setState_0_NSGreen(LEDState *state) {
    clearAllLEDs(state);
    // North through green (both heads)
    setLED(state, N_COMBO_GREEN, true);
    setLED(state, N_THRU_GREEN, true);

    // South through green (combo + center heads)
    setLED(state, S_COMBO_GREEN, true);
    setLED(state, S_THRU_GREEN, true);
    // South right permissive (green ball)
    setLED(state, S_RIGHT_GREEN_BALL, true);

    // East-West all red
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

// State 1: North-South Yellow
void setState_1_NSYellow(LEDState *state) {
    clearAllLEDs(state);
    // North through yellow
    setLED(state, N_COMBO_YELLOW, true);
    setLED(state, N_THRU_YELLOW, true);

    // South through yellow
    setLED(state, S_COMBO_YELLOW, true);
    setLED(state, S_THRU_YELLOW, true);
    setLED(state, S_RIGHT_YELLOW, true);

    // East-West red
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

// State 2: All Red #1
void setState_2_AllRed1(LEDState *state) {
    setAllRed(state);
}

// State 3: North Left Turn Green (Protected)
void setState_3_NLeftGreen(LEDState *state) {
    clearAllLEDs(state);
    // North left green arrow
    setLED(state, N_LEFT_GREEN_ARROW, true);
    // North through red
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);

    // All others red
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

// State 4: North Left Turn Yellow
void setState_4_NLeftYellow(LEDState *state) {
    clearAllLEDs(state);
    // North combo yellow (arrow turns yellow)
    setLED(state, N_COMBO_YELLOW, true);
    setLED(state, N_THRU_RED, true);

    // All others red
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

// State 5: All Red #2
void setState_5_AllRed2(LEDState *state) {
    setAllRed(state);
}

// State 6: South Left Turn Green (Protected)
void setState_6_SLeftGreen(LEDState *state) {
    clearAllLEDs(state);
    // South left green arrow
    setLED(state, S_LEFT_GREEN_ARROW, true);
    // South through red
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);

    // All others red
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

// State 7: South Left Turn Yellow
void setState_7_SLeftYellow(LEDState *state) {
    clearAllLEDs(state);
    // South combo yellow
    setLED(state, S_COMBO_YELLOW, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);

    // All others red
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

// State 8: All Red #3
void setState_8_AllRed3(LEDState *state) {
    setAllRed(state);
}

// REMOVE setState_9_EWGreen entirely, REPLACE with:
void setState_9_WThruGreen(LEDState *state) {
    clearAllLEDs(state);
    // West through green ONLY - SPLIT PHASE
    setLED(state, W_THRU_GREEN, true);
    setLED(state, W_RIGHT_GREEN_BALL, true);   // Permissive right turn

    // East explicitly RED - CRITICAL SAFETY
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);

    // North-South red
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
}

// REMOVE setState_10_EWYellow entirely, REPLACE with:
void setState_10_WThruYellow(LEDState *state) {
    clearAllLEDs(state);
    // West yellow ONLY
    setLED(state, W_THRU_YELLOW, true);
    setLED(state, W_RIGHT_YELLOW, true);

    // East explicitly RED - CRITICAL SAFETY
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);

    // North-South red
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
}

// State 11: All Red #4
void setState_11_AllRed4(LEDState *state) {
    setAllRed(state);
}

// State 12: East Through Green (if needed separately)
void setState_12_EThruGreen(LEDState *state) {
    clearAllLEDs(state);
    // East through green
    setLED(state, E_THRU_LEFT_GREEN, true);
    setLED(state, E_THRU_RIGHT_GREEN, true);

    // All others red
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
}

// State 13: East Through Yellow
void setState_13_EThruYellow(LEDState *state) {
    clearAllLEDs(state);
    // East yellow
    setLED(state, E_THRU_LEFT_YELLOW, true);
    setLED(state, E_THRU_RIGHT_YELLOW, true);

    // All others red
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
}

// State 14: All Red #5
void setState_14_AllRed5(LEDState *state) {
    setAllRed(state);
}

// State 15: West Right Turn Green (if protected phase needed)
void setState_15_WRightGreen(LEDState *state) {
    clearAllLEDs(state);
    // West right green arrow
    setLED(state, W_RIGHT_GREEN_ARROW, true);
    setLED(state, W_THRU_RED, true);

    // All others red
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

// State 16: West Right Turn Yellow
void setState_16_WRightYellow(LEDState *state) {
    clearAllLEDs(state);
    // West right yellow
    setLED(state, W_RIGHT_YELLOW, true);
    setLED(state, W_THRU_RED, true);

    // All others red
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

// State 17: All Red #6
void setState_17_AllRed6(LEDState *state) {
    setAllRed(state);
}

// State 18: Return to Start
void setState_18_ReturnStart(LEDState *state) {
    setAllRed(state);
}

// ============================================================================
// HIGH TRAFFIC MODE STATES (19-37)
// ============================================================================

// State 19: North Priority Start
void setState_19_NPriorityStart(LEDState *state) {
    setAllRed(state);
}

// State 20: North Priority Green (25 seconds - extended)
void setState_20_NPriorityGreen(LEDState *state) {
    clearAllLEDs(state);
    // Northbound gets priority - extended green
    setLED(state, N_COMBO_GREEN, true);
    setLED(state, N_THRU_GREEN, true);
    setLED(state, N_LEFT_GREEN_ARROW, true);  // Both through AND left arrow

    // All others red
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

// State 21: North Priority Yellow
void setState_21_NPriorityYellow(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, N_COMBO_YELLOW, true);
    setLED(state, N_THRU_YELLOW, true);

    // All others red
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

void setState_22_AllRedHT1(LEDState *state) { setAllRed(state); }

void setState_23_NLeftGreenHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, N_LEFT_GREEN_ARROW, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

void setState_24_NLeftYellowHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, N_COMBO_YELLOW, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

void setState_25_AllRedHT2(LEDState *state) { setAllRed(state); }

void setState_26_SGreenHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, S_COMBO_GREEN, true);
    setLED(state, S_THRU_GREEN, true);
    setLED(state, S_RIGHT_GREEN_BALL, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

void setState_27_SYellowHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, S_COMBO_YELLOW, true);
    setLED(state, S_THRU_YELLOW, true);
    setLED(state, S_RIGHT_YELLOW, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

void setState_28_AllRedHT3(LEDState *state) { setAllRed(state); }

void setState_29_SLeftGreenHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, S_LEFT_GREEN_ARROW, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

void setState_30_SLeftYellowHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, S_COMBO_YELLOW, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

void setState_31_AllRedHT4(LEDState *state) { setAllRed(state); }

// REMOVE setState_32_EWGreenHT, REPLACE with:
void setState_32_WThruGreenHT(LEDState *state) {
    clearAllLEDs(state);
    // West through green ONLY - SPLIT PHASE
    setLED(state, W_THRU_GREEN, true);
    setLED(state, W_RIGHT_GREEN_BALL, true);

    // East explicitly RED - CRITICAL SAFETY
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);

    // North-South red
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
}

// REMOVE setState_33_EWYellowHT, REPLACE with:
void setState_33_WThruYellowHT(LEDState *state) {
    clearAllLEDs(state);
    // West yellow ONLY
    setLED(state, W_THRU_YELLOW, true);
    setLED(state, W_RIGHT_YELLOW, true);

    // East explicitly RED - CRITICAL SAFETY
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);

    // North-South red
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
}
void setState_34_AllRedHT5(LEDState *state) { setAllRed(state); }

void setState_35_WRightGreenHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, W_RIGHT_GREEN_ARROW, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

void setState_36_WRightYellowHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, W_RIGHT_YELLOW, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

void setState_37_ReturnHT(LEDState *state) { setAllRed(state); }

// ============================================================================
// NIGHT MODE STATES (38-40)
// ============================================================================

// State 38: Night Flash ON
void setState_38_NightFlashOn(LEDState *state) {
    clearAllLEDs(state);
    // Main streets (University Ave): Yellow flash
    setLED(state, N_COMBO_YELLOW, true);
    setLED(state, N_THRU_YELLOW, true);
    setLED(state, S_COMBO_YELLOW, true);
    setLED(state, S_THRU_YELLOW, true);

    // Side streets (Glenna Goodacre Blvd): Red flash
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

// State 39: Night Flash OFF
void setState_39_NightFlashOff(LEDState *state) {
    clearAllLEDs(state);  // All LEDs off
}

// State 40: Night Transition
void setState_40_NightTransition(LEDState *state) {
    setAllRed(state);
}

// ============================================================================
// EMERGENCY MODE STATES (41-42)
// ============================================================================

// State 41: Emergency All Red
void setState_41_EmergencyAllRed(LEDState *state) {
    setAllRed(state);
}

// State 42: Emergency Hold
void setState_42_EmergencyHold(LEDState *state) {
    setAllRed(state);
}

// ============================================================================
// STATE EXECUTION AND TIMING
// ============================================================================

void executeState(LEDState *state, TrafficState currentState) {
    switch (currentState) {
        // Daytime mode (0-18)
        case STATE_NS_GREEN:          setState_0_NSGreen(state); break;
        case STATE_NS_YELLOW:         setState_1_NSYellow(state); break;
        case STATE_ALL_RED_1:         setState_2_AllRed1(state); break;
        case STATE_N_LEFT_GREEN:      setState_3_NLeftGreen(state); break;
        case STATE_N_LEFT_YELLOW:     setState_4_NLeftYellow(state); break;
        case STATE_ALL_RED_2:         setState_5_AllRed2(state); break;
        case STATE_S_LEFT_GREEN:      setState_6_SLeftGreen(state); break;
        case STATE_S_LEFT_YELLOW:     setState_7_SLeftYellow(state); break;
        case STATE_ALL_RED_3:         setState_8_AllRed3(state); break;
        case STATE_W_THRU_GREEN:     setState_9_WThruGreen(state);    break;
        case STATE_W_THRU_YELLOW:    setState_10_WThruYellow(state);  break;
        case STATE_ALL_RED_4:         setState_11_AllRed4(state); break;
        case STATE_E_THRU_GREEN:      setState_12_EThruGreen(state); break;
        case STATE_E_THRU_YELLOW:     setState_13_EThruYellow(state); break;
        case STATE_ALL_RED_5:         setState_14_AllRed5(state); break;
        case STATE_W_RIGHT_GREEN:     setState_15_WRightGreen(state); break;
        case STATE_W_RIGHT_YELLOW:    setState_16_WRightYellow(state); break;
        case STATE_ALL_RED_6:         setState_17_AllRed6(state); break;
        case STATE_RETURN_TO_START:   setState_18_ReturnStart(state); break;

        // High Traffic mode (19-37)
        case STATE_N_PRIORITY_START:  setState_19_NPriorityStart(state); break;
        case STATE_N_PRIORITY_GREEN:  setState_20_NPriorityGreen(state); break;
        case STATE_N_PRIORITY_YELLOW: setState_21_NPriorityYellow(state); break;
        case STATE_ALL_RED_HT_1:      setState_22_AllRedHT1(state); break;
        case STATE_N_LEFT_GREEN_HT:   setState_23_NLeftGreenHT(state); break;
        case STATE_N_LEFT_YELLOW_HT:  setState_24_NLeftYellowHT(state); break;
        case STATE_ALL_RED_HT_2:      setState_25_AllRedHT2(state); break;
        case STATE_S_GREEN_HT:        setState_26_SGreenHT(state); break;
        case STATE_S_YELLOW_HT:       setState_27_SYellowHT(state); break;
        case STATE_ALL_RED_HT_3:      setState_28_AllRedHT3(state); break;
        case STATE_S_LEFT_GREEN_HT:   setState_29_SLeftGreenHT(state); break;
        case STATE_S_LEFT_YELLOW_HT:  setState_30_SLeftYellowHT(state); break;
        case STATE_ALL_RED_HT_4:      setState_31_AllRedHT4(state); break;
        case STATE_W_THRU_GREEN_HT:  setState_32_WThruGreenHT(state); break;
        case STATE_W_THRU_YELLOW_HT: setState_33_WThruYellowHT(state); break;
        case STATE_ALL_RED_HT_5:      setState_34_AllRedHT5(state); break;
        case STATE_W_RIGHT_GREEN_HT:  setState_35_WRightGreenHT(state); break;
        case STATE_W_RIGHT_YELLOW_HT: setState_36_WRightYellowHT(state); break;
        case STATE_RETURN_HT:         setState_37_ReturnHT(state); break;

        // Night mode (38-40)
        case STATE_NIGHT_FLASH_ON:    setState_38_NightFlashOn(state); break;
        case STATE_NIGHT_FLASH_OFF:   setState_39_NightFlashOff(state); break;
        case STATE_NIGHT_TRANSITION:  setState_40_NightTransition(state); break;

        // Emergency mode (41-42)
        case STATE_EMERGENCY_ALL_RED: setState_41_EmergencyAllRed(state); break;
        case STATE_EMERGENCY_HOLD:    setState_42_EmergencyHold(state); break;

        default: setAllRed(state); break;
    }
}

uint32_t getStateDuration(TrafficState state) {
    switch (state) {
        // Daytime mode timing
        case STATE_NS_GREEN:          return TIME_NS_GREEN;
        case STATE_NS_YELLOW:         return TIME_YELLOW;
        case STATE_ALL_RED_1:         return TIME_ALL_RED;
        case STATE_N_LEFT_GREEN:      return TIME_N_LEFT_GREEN;
        case STATE_N_LEFT_YELLOW:     return TIME_YELLOW;
        case STATE_ALL_RED_2:         return TIME_ALL_RED;
        case STATE_S_LEFT_GREEN:      return TIME_S_LEFT_GREEN;
        case STATE_S_LEFT_YELLOW:     return TIME_YELLOW;
        case STATE_ALL_RED_3:         return TIME_ALL_RED;
        case STATE_W_THRU_GREEN:      return TIME_W_THRU_GREEN;
        case STATE_W_THRU_YELLOW:     return TIME_YELLOW;
        case STATE_ALL_RED_4:         return TIME_ALL_RED;
        case STATE_E_THRU_GREEN:      return TIME_W_THRU_GREEN;
        case STATE_E_THRU_YELLOW:     return TIME_YELLOW;
        case STATE_ALL_RED_5:         return TIME_ALL_RED;
        case STATE_W_RIGHT_GREEN:     return TIME_W_RIGHT_GREEN;
        case STATE_W_RIGHT_YELLOW:    return TIME_YELLOW;
        case STATE_ALL_RED_6:         return TIME_ALL_RED;
        case STATE_RETURN_TO_START:   return TIME_ALL_RED;

        // High Traffic mode timing
        case STATE_N_PRIORITY_START:  return TIME_ALL_RED;
        case STATE_N_PRIORITY_GREEN:  return TIME_N_PRIORITY;
        case STATE_N_PRIORITY_YELLOW: return TIME_YELLOW;
        case STATE_ALL_RED_HT_1:      return TIME_ALL_RED;
        case STATE_N_LEFT_GREEN_HT:   return TIME_N_LEFT_GREEN_HT;
        case STATE_N_LEFT_YELLOW_HT:  return TIME_YELLOW;
        case STATE_ALL_RED_HT_2:      return TIME_ALL_RED;
        case STATE_S_GREEN_HT:        return TIME_NS_GREEN_HT;
        case STATE_S_YELLOW_HT:       return TIME_YELLOW;
        case STATE_ALL_RED_HT_3:      return TIME_ALL_RED;
        case STATE_S_LEFT_GREEN_HT:   return TIME_S_LEFT_GREEN_HT;
        case STATE_S_LEFT_YELLOW_HT:  return TIME_YELLOW;
        case STATE_ALL_RED_HT_4:      return TIME_ALL_RED;
        case STATE_W_THRU_GREEN_HT: return TIME_W_THRU_GREEN_HT;
        case STATE_W_THRU_YELLOW_HT:  return TIME_YELLOW;
        case STATE_ALL_RED_HT_5:      return TIME_ALL_RED;
        case STATE_W_RIGHT_GREEN_HT:  return TIME_W_RIGHT_GREEN_HT;
        case STATE_W_RIGHT_YELLOW_HT: return TIME_YELLOW;
        case STATE_RETURN_HT:         return TIME_ALL_RED;

        // Night mode timing
        case STATE_NIGHT_FLASH_ON:    return TIME_NIGHT_FLASH_ON;
        case STATE_NIGHT_FLASH_OFF:   return TIME_NIGHT_FLASH_OFF;
        case STATE_NIGHT_TRANSITION:  return TIME_ALL_RED;

        // Emergency mode timing
        case STATE_EMERGENCY_ALL_RED: return TIME_ALL_RED;
        case STATE_EMERGENCY_HOLD:    return TIME_EMERGENCY_HOLD;

        default: return TIME_ALL_RED;
    }
}

TrafficState getNextState(TrafficState currentState, OperatingMode mode) {
    // Simple sequential progression

    if (mode == MODE_DAYTIME) {
        if (currentState < STATE_RETURN_TO_START) {
            return (TrafficState)(currentState + 1);
        } else {
            return STATE_NS_GREEN;
        }
    }
    else if (mode == MODE_HIGH_TRAFFIC) {
        if (currentState >= STATE_N_PRIORITY_START && currentState < STATE_RETURN_HT) {
            return (TrafficState)(currentState + 1);
        } else {
            return STATE_N_PRIORITY_START;
        }
    }
    else if (mode == MODE_NIGHT) {
        // Toggle between ON and OFF
        if (currentState == STATE_NIGHT_FLASH_ON) {
            return STATE_NIGHT_FLASH_OFF;
        } else {
            return STATE_NIGHT_FLASH_ON;
        }
    }
    else if (mode == MODE_EMERGENCY) {
        return STATE_EMERGENCY_HOLD;
    }

    return STATE_NS_GREEN;
}
