#include "traffic_states.h"
#include <string.h>


// HELPER FUNCTIONS

void initLEDState(LEDState *state) {
    memset(state->byte, 0, 4);
}

void clearAllLEDs(LEDState *state) {
    memset(state->byte, 0, 4);
}

void setAllRed(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setAllYellow(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_YELLOW, true);
    setLED(state, N_LEFT_YELLOW_STEADY, true);
    setLED(state, S_LEFT_YELLOW_STEADY, true);
    setLED(state, S_RIGHT_YELLOW, true);
    setLED(state, W_THRU_YELLOW, true);
    setLED(state, W_RIGHT_YELLOW, true);
    setLED(state, E_THRU_YELLOW, true);
    setLED(state, E_LEFT_YELLOW, true);
}

void setLED(LEDState *state, uint8_t ledNumber, bool on) {
    if (ledNumber >= 32) return;
    
    uint8_t byteIndex = ledNumber / 8;
    uint8_t bitIndex = ledNumber % 8;
    
    if (on) {
        state->byte[byteIndex] |= (1 << bitIndex);
    } else {
        state->byte[byteIndex] &= ~(1 << bitIndex);
    }
}

bool getLED(LEDState *state, uint8_t ledNumber) {
    if (ledNumber >= 32) return false;
    uint8_t byteIndex = ledNumber / 8;
    uint8_t bitIndex = ledNumber % 8;
    return (state->byte[byteIndex] & (1 << bitIndex)) != 0;
}

// Daytime mode states (0-17)

void setState_0_UnivGreen(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_GREEN, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_GREEN_BALL, true); // Permissive turn
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_1_UnivYellow(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_YELLOW, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_YELLOW, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_2_AllRed1(LEDState *state) { setAllRed(state); }

void setState_3_NLeftGreen(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_GREEN_ARROW, true); // Protected
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_4_NLeftYellow(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_YELLOW_STEADY, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_5_AllRed2(LEDState *state) { setAllRed(state); }

void setState_6_SLeftGreen(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_GREEN_ARROW, true); // Protected
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_7_SLeftYellow(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_YELLOW_STEADY, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_8_AllRed3(LEDState *state) { setAllRed(state); }

void setState_9_WestGreen(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_GREEN, true);
    setLED(state, W_RIGHT_GREEN_BALL, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_10_WestYellow(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_YELLOW, true);
    setLED(state, W_RIGHT_YELLOW, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_11_AllRed4(LEDState *state) { setAllRed(state); }

void setState_12_EastThruGreen(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_GREEN, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_13_EastThruYellow(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_YELLOW, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_14_AllRed5(LEDState *state) { setAllRed(state); }

void setState_15_ELeftGreen(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_GREEN_ARROW, true);
}

void setState_16_ELeftYellow(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_YELLOW, true);
}

void setState_17_AllRed6(LEDState *state) { setAllRed(state); }


// High traffic mode states (18-36)


void setState_18_NorthPriority(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_GREEN, true);
    setLED(state, N_LEFT_GREEN_ARROW, true); // Northbound Priority
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_19_NArrowEnding(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_GREEN, true);
    setLED(state, N_LEFT_YELLOW_STEADY, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_20_NThruBonus(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_GREEN, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_21_NThruYellow(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_YELLOW, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_22_AllRedHT1(LEDState *state) { setAllRed(state); }

void setState_23_SouthGreenHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_GREEN, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_GREEN_BALL, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_24_SouthYellowHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_YELLOW, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_YELLOW, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_25_AllRedHT2(LEDState *state) { setAllRed(state); }

void setState_26_SLeftGreenHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_GREEN_ARROW, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_27_SLeftYellowHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_YELLOW_STEADY, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_28_AllRedHT3(LEDState *state) { setAllRed(state); }

void setState_29_WestGreenHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_GREEN, true);
    setLED(state, W_RIGHT_GREEN_BALL, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_30_WestYellowHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_YELLOW, true);
    setLED(state, W_RIGHT_YELLOW, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_31_AllRedHT4(LEDState *state) { setAllRed(state); }

void setState_32_EastThruGreenHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_GREEN, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_33_EastThruYellowHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_YELLOW, true);
    setLED(state, E_LEFT_RED, true);
}

void setState_34_AllRedHT5(LEDState *state) { setAllRed(state); }

void setState_35_ELeftGreenHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_GREEN_ARROW, true);
}

void setState_36_ELeftYellowHT(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_RED, true);
    setLED(state, N_LEFT_RED, true);
    setLED(state, S_LEFT_RED, true);
    setLED(state, S_RIGHT_RED, true);
    setLED(state, W_THRU_RED, true);
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
    setLED(state, E_LEFT_YELLOW, true);
}

// Night mode states (37-38)


void setState_37_NightOff(LEDState *state) { clearAllLEDs(state); }

void setState_38_NightOn(LEDState *state) {
    clearAllLEDs(state);
    setLED(state, NS_MAIN_GREEN, true); // University: Proceed caution
    setLED(state, W_THRU_RED, true);    // 8th St: Stop first
    setLED(state, W_RIGHT_RED, true);
    setLED(state, E_THRU_RED, true);
}


// Emergency override states (39-42)


void setState_39_EmergWarning(LEDState *state) { setAllYellow(state); }
void setState_40_EmergHold(LEDState *state) { setAllRed(state); }
void setState_41_EmergClearing(LEDState *state) { setAllRed(state); }
void setState_42_EmergReturn(LEDState *state) { setAllRed(state); }


// State execution and timing


void executeState(LEDState *state, TrafficState currentState) {
    switch (currentState) {
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
        
        case STATE_NIGHT_OFF:           setState_37_NightOff(state); break;
        case STATE_NIGHT_ON:            setState_38_NightOn(state); break;
        
        case STATE_EMERG_WARNING:       setState_39_EmergWarning(state); break;
        case STATE_EMERG_HOLD:          setState_40_EmergHold(state); break;
        case STATE_EMERG_CLEARING:      setState_41_EmergClearing(state); break;
        case STATE_EMERG_RETURN:        setState_42_EmergReturn(state); break;
        
        default: setAllRed(state); break;
    }
}

uint32_t getStateDuration(TrafficState state) {
    switch (state) {
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
        
        case STATE_NIGHT_OFF:
        case STATE_NIGHT_ON:            return TIME_NIGHT_FLASH;
        
        case STATE_EMERG_WARNING:       return TIME_EMERG_WARNING;
        case STATE_EMERG_HOLD:          return TIME_EMERG_HOLD_MIN;
        case STATE_EMERG_CLEARING:      return TIME_EMERG_CLEARING;
        case STATE_EMERG_RETURN:        return TIME_EMERG_RETURN;
        
        default: return TIME_ALL_RED;
    }
}