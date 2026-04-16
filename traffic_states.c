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

void setAllYellow(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, N_COMBO_YELLOW, true);
    setLED(state, N_THRU_YELLOW, true);
    setLED(state, S_COMBO_YELLOW, true);
    setLED(state, S_THRU_YELLOW, true);
    setLED(state, S_RIGHT_YELLOW, true);
    setLED(state, W_THRU_YELLOW, true);
    setLED(state, W_RIGHT_YELLOW, true);
    setLED(state, E_THRU_LEFT_YELLOW, true);
    setLED(state, E_THRU_RIGHT_YELLOW, true);
}

void setLED(LEDState *state, uint8_t ledNumber, bool on) {
    uint8_t byteIndex;
    uint8_t bitIndex;

    if (ledNumber >= 32) return;

    byteIndex = ledNumber / 8;
    bitIndex  = ledNumber % 8;

    if (on) {
        state->byte[byteIndex] |=  (1 << bitIndex);
    } else {
        state->byte[byteIndex] &= ~(1 << bitIndex);
    }
}

bool getLED(LEDState *state, uint8_t ledNumber) {
    uint8_t byteIndex;
    uint8_t bitIndex;

    if (ledNumber >= 32) return false;

    byteIndex = ledNumber / 8;
    bitIndex  = ledNumber % 8;

    return (state->byte[byteIndex] & (1 << bitIndex)) != 0;
}

// ============================================================================
// DAYTIME MODE STATES (0-18)
// ============================================================================

void setState_0_NSGreen(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, N_COMBO_GREEN, true);
    setLED(state, N_THRU_GREEN, true);
    setLED(state, S_COMBO_GREEN, true);
    setLED(state, S_THRU_GREEN, true);
    setLED(state, S_RIGHT_GREEN_BALL, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

void setState_1_NSYellow(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, N_COMBO_YELLOW, true);
    setLED(state, N_THRU_YELLOW, true);
    setLED(state, S_COMBO_YELLOW, true);
    setLED(state, S_THRU_YELLOW, true);
    setLED(state, S_RIGHT_YELLOW, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

void setState_2_AllRed1(LEDState *state)  { setAllRed(state); }

void setState_3_NLeftGreen(LEDState *state) {
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

void setState_4_NLeftYellow(LEDState *state) {
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

void setState_5_AllRed2(LEDState *state)  { setAllRed(state); }

void setState_6_SLeftGreen(LEDState *state) {
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

void setState_7_SLeftYellow(LEDState *state) {
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

void setState_8_AllRed3(LEDState *state)  { setAllRed(state); }

void setState_9_WThruGreen(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, W_THRU_GREEN, true);
    setLED(state, W_RIGHT_GREEN_BALL, true);
    setLED(state, E_THRU_LEFT_RED, true);     // East RED - CRITICAL
    setLED(state, E_THRU_RIGHT_RED, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
}

void setState_10_WThruYellow(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, W_THRU_YELLOW, true);
    setLED(state, W_RIGHT_YELLOW, true);
    setLED(state, E_THRU_LEFT_RED, true);     // East RED - CRITICAL
    setLED(state, E_THRU_RIGHT_RED, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
}

void setState_11_AllRed4(LEDState *state)  { setAllRed(state); }

void setState_12_EThruGreen(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, E_THRU_LEFT_GREEN, true);
    setLED(state, E_THRU_RIGHT_GREEN, true);
    setLED(state, W_THRU_RED, true);          // West RED - CRITICAL
    setLED(state, W_RIGHT_RED, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
}

void setState_13_EThruYellow(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, E_THRU_LEFT_YELLOW, true);
    setLED(state, E_THRU_RIGHT_YELLOW, true);
    setLED(state, W_THRU_RED, true);          // West RED - CRITICAL
    setLED(state, W_RIGHT_RED, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
}

void setState_14_AllRed5(LEDState *state)  { setAllRed(state); }

void setState_15_WRightGreen(LEDState *state) {
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

void setState_16_WRightYellow(LEDState *state) {
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

void setState_17_AllRed6(LEDState *state)     { setAllRed(state); }
void setState_18_ReturnStart(LEDState *state)  { setAllRed(state); }

// ============================================================================
// HIGH TRAFFIC MODE STATES (19-35)
// Sequence: N solo (10s) -> S left during N (8s) -> Both green (25s) ->
//           N left during S (8s) -> Both yellow (3s) -> All red -> E/W split
// ============================================================================

void setState_19_NPriorityStart(LEDState *state) { setAllRed(state); }

// State 20: North solo green - South not yet active
void setState_20_NSoloGreen(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, N_COMBO_GREEN, true);
    setLED(state, N_THRU_GREEN, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

// State 21: South left arrow while North still green
void setState_21_SLeftDuringN(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, N_COMBO_GREEN, true);
    setLED(state, N_THRU_GREEN, true);
    setLED(state, S_LEFT_GREEN_ARROW, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

// State 22: Both North and South green together
void setState_22_NSBothGreen(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, N_COMBO_GREEN, true);
    setLED(state, N_THRU_GREEN, true);
    setLED(state, S_COMBO_GREEN, true);
    setLED(state, S_THRU_GREEN, true);
    setLED(state, S_RIGHT_GREEN_BALL, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

// State 23: North left arrow while South still green
void setState_23_NLeftDuringS(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, N_LEFT_GREEN_ARROW, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_GREEN, true);
    setLED(state, S_THRU_GREEN, true);
    setLED(state, S_RIGHT_GREEN_BALL, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

// State 24: Both North and South yellow together
void setState_24_NSHTYellow(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, N_COMBO_YELLOW, true);
    setLED(state, N_THRU_YELLOW, true);
    setLED(state, S_COMBO_YELLOW, true);
    setLED(state, S_THRU_YELLOW, true);
    setLED(state, S_RIGHT_YELLOW, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

void setState_25_AllRedHT1(LEDState *state) { setAllRed(state); }

// State 26: West through green - split phase
void setState_26_WThruGreenHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, W_THRU_GREEN, true);
    setLED(state, W_RIGHT_GREEN_BALL, true);
    setLED(state, E_THRU_LEFT_RED, true);     // East RED - CRITICAL
    setLED(state, E_THRU_RIGHT_RED, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
}

void setState_27_WThruYellowHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, W_THRU_YELLOW, true);
    setLED(state, W_RIGHT_YELLOW, true);
    setLED(state, E_THRU_LEFT_RED, true);     // East RED - CRITICAL
    setLED(state, E_THRU_RIGHT_RED, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
}

void setState_28_AllRedHT2(LEDState *state) { setAllRed(state); }

// State 29: East through green - split phase
void setState_29_EThruGreenHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, E_THRU_LEFT_GREEN, true);
    setLED(state, E_THRU_RIGHT_GREEN, true);
    setLED(state, W_THRU_RED, true);          // West RED - CRITICAL
    setLED(state, W_RIGHT_RED, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
}

void setState_30_EThruYellowHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, E_THRU_LEFT_YELLOW, true);
    setLED(state, E_THRU_RIGHT_YELLOW, true);
    setLED(state, W_THRU_RED, true);          // West RED - CRITICAL
    setLED(state, W_RIGHT_RED, true);
    setLED(state, N_COMBO_RED, true);
    setLED(state, N_THRU_RED, true);
    setLED(state, S_COMBO_RED, true);
    setLED(state, S_THRU_RED, true);
    setLED(state, S_RIGHT_RED, true);
}

void setState_31_AllRedHT3(LEDState *state) { setAllRed(state); }

void setState_32_WRightGreenHT(LEDState *state) {
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

void setState_33_WRightYellowHT(LEDState *state) {
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

void setState_34_AllRedHT4(LEDState *state)  { setAllRed(state); }
void setState_35_ReturnHT(LEDState *state)    { setAllRed(state); }

// ============================================================================
// NIGHT MODE STATES (38-40)
// ============================================================================

void setState_38_NightFlashOn(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, N_COMBO_YELLOW, true);
    setLED(state, N_THRU_YELLOW, true);
    setLED(state, S_COMBO_YELLOW, true);
    setLED(state, S_THRU_YELLOW, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_LEFT_RED, true);
    setLED(state, E_THRU_RIGHT_RED, true);
}

void setState_39_NightFlashOff(LEDState *state)   { clearAllLEDs(state); }
void setState_40_NightTransition(LEDState *state)  { setAllRed(state); }

// ============================================================================
// EMERGENCY MODE STATES (41-42)
// ============================================================================

void setState_41_EmergencyAllRed(LEDState *state) { setAllRed(state); }
void setState_42_EmergencyHold(LEDState *state)   { setAllRed(state); }

// ============================================================================
// STATE EXECUTION
// ============================================================================

void executeState(LEDState *state, TrafficState currentState) {
    switch (currentState) {
        // Daytime (0-18)
        case STATE_NS_GREEN:          setState_0_NSGreen(state);       break;
        case STATE_NS_YELLOW:         setState_1_NSYellow(state);      break;
        case STATE_ALL_RED_1:         setState_2_AllRed1(state);       break;
        case STATE_N_LEFT_GREEN:      setState_3_NLeftGreen(state);    break;
        case STATE_N_LEFT_YELLOW:     setState_4_NLeftYellow(state);   break;
        case STATE_ALL_RED_2:         setState_5_AllRed2(state);       break;
        case STATE_S_LEFT_GREEN:      setState_6_SLeftGreen(state);    break;
        case STATE_S_LEFT_YELLOW:     setState_7_SLeftYellow(state);   break;
        case STATE_ALL_RED_3:         setState_8_AllRed3(state);       break;
        case STATE_W_THRU_GREEN:      setState_9_WThruGreen(state);    break;
        case STATE_W_THRU_YELLOW:     setState_10_WThruYellow(state);  break;
        case STATE_ALL_RED_4:         setState_11_AllRed4(state);      break;
        case STATE_E_THRU_GREEN:      setState_12_EThruGreen(state);   break;
        case STATE_E_THRU_YELLOW:     setState_13_EThruYellow(state);  break;
        case STATE_ALL_RED_5:         setState_14_AllRed5(state);      break;
        case STATE_W_RIGHT_GREEN:     setState_15_WRightGreen(state);  break;
        case STATE_W_RIGHT_YELLOW:    setState_16_WRightYellow(state); break;
        case STATE_ALL_RED_6:         setState_17_AllRed6(state);      break;
        case STATE_RETURN_TO_START:   setState_18_ReturnStart(state);  break;

        // High Traffic (19-35)
        case STATE_N_PRIORITY_START:  setState_19_NPriorityStart(state); break;
        case STATE_N_SOLO_GREEN:      setState_20_NSoloGreen(state);     break;
        case STATE_S_LEFT_DURING_N:   setState_21_SLeftDuringN(state);   break;
        case STATE_NS_BOTH_GREEN:     setState_22_NSBothGreen(state);    break;
        case STATE_N_LEFT_DURING_S:   setState_23_NLeftDuringS(state);   break;
        case STATE_NS_HT_YELLOW:      setState_24_NSHTYellow(state);     break;
        case STATE_ALL_RED_HT_1:      setState_25_AllRedHT1(state);      break;
        case STATE_W_THRU_GREEN_HT:   setState_26_WThruGreenHT(state);   break;
        case STATE_W_THRU_YELLOW_HT:  setState_27_WThruYellowHT(state);  break;
        case STATE_ALL_RED_HT_2:      setState_28_AllRedHT2(state);      break;
        case STATE_E_THRU_GREEN_HT:   setState_29_EThruGreenHT(state);   break;
        case STATE_E_THRU_YELLOW_HT:  setState_30_EThruYellowHT(state);  break;
        case STATE_ALL_RED_HT_3:      setState_31_AllRedHT3(state);      break;
        case STATE_W_RIGHT_GREEN_HT:  setState_32_WRightGreenHT(state);  break;
        case STATE_W_RIGHT_YELLOW_HT: setState_33_WRightYellowHT(state); break;
        case STATE_ALL_RED_HT_4:      setState_34_AllRedHT4(state);      break;
        case STATE_RETURN_HT:         setState_35_ReturnHT(state);        break;

        // Night (38-40)
        case STATE_NIGHT_FLASH_ON:    setState_38_NightFlashOn(state);    break;
        case STATE_NIGHT_FLASH_OFF:   setState_39_NightFlashOff(state);   break;
        case STATE_NIGHT_TRANSITION:  setState_40_NightTransition(state); break;

        // Emergency (41-42)
        case STATE_EMERGENCY_ALL_RED: setState_41_EmergencyAllRed(state); break;
        case STATE_EMERGENCY_HOLD:    setState_42_EmergencyHold(state);   break;

        default: setAllRed(state); break;
    }
}

// ============================================================================
// STATE DURATION
// ============================================================================

uint32_t getStateDuration(TrafficState state) {
    switch (state) {
        // Daytime
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
        case STATE_E_THRU_GREEN:      return TIME_E_THRU_GREEN;
        case STATE_E_THRU_YELLOW:     return TIME_YELLOW;
        case STATE_ALL_RED_5:         return TIME_ALL_RED;
        case STATE_W_RIGHT_GREEN:     return TIME_W_RIGHT_GREEN;
        case STATE_W_RIGHT_YELLOW:    return TIME_YELLOW;
        case STATE_ALL_RED_6:         return TIME_ALL_RED;
        case STATE_RETURN_TO_START:   return TIME_ALL_RED;

        // High Traffic
        case STATE_N_PRIORITY_START:  return TIME_ALL_RED;
        case STATE_N_SOLO_GREEN:      return TIME_N_SOLO_GREEN;
        case STATE_S_LEFT_DURING_N:   return TIME_S_LEFT_DURING_N;
        case STATE_NS_BOTH_GREEN:     return TIME_NS_BOTH_GREEN;
        case STATE_N_LEFT_DURING_S:   return TIME_N_LEFT_DURING_S;
        case STATE_NS_HT_YELLOW:      return TIME_YELLOW;
        case STATE_ALL_RED_HT_1:      return TIME_ALL_RED;
        case STATE_W_THRU_GREEN_HT:   return TIME_W_THRU_GREEN_HT;
        case STATE_W_THRU_YELLOW_HT:  return TIME_YELLOW;
        case STATE_ALL_RED_HT_2:      return TIME_ALL_RED;
        case STATE_E_THRU_GREEN_HT:   return TIME_E_THRU_GREEN_HT;
        case STATE_E_THRU_YELLOW_HT:  return TIME_YELLOW;
        case STATE_ALL_RED_HT_3:      return TIME_ALL_RED;
        case STATE_W_RIGHT_GREEN_HT:  return TIME_W_RIGHT_GREEN_HT;
        case STATE_W_RIGHT_YELLOW_HT: return TIME_YELLOW;
        case STATE_ALL_RED_HT_4:      return TIME_ALL_RED;
        case STATE_RETURN_HT:         return TIME_ALL_RED;

        // Night
        case STATE_NIGHT_FLASH_ON:    return TIME_NIGHT_FLASH_ON;
        case STATE_NIGHT_FLASH_OFF:   return TIME_NIGHT_FLASH_OFF;
        case STATE_NIGHT_TRANSITION:  return TIME_ALL_RED;

        // Emergency
        case STATE_EMERGENCY_ALL_RED: return TIME_ALL_RED;
        case STATE_EMERGENCY_HOLD:    return TIME_EMERGENCY_HOLD;

        default: return TIME_ALL_RED;
    }
}

// ============================================================================
// NEXT STATE LOGIC
// ============================================================================

TrafficState getNextState(TrafficState currentState, OperatingMode mode) {
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
