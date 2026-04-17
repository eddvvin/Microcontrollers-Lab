#ifndef TRAFFIC_STATES_H
#define TRAFFIC_STATES_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * LED BIT ASSIGNMENTS (Based on Physical Intersection Model - 31 LEDs Total)
 * ========================================================================= */

/* --- NORTH APPROACH (Bits 0-6) --- */
#define N_LEFT_GREEN_ARROW       0
#define N_COMBO_RED              1
#define N_COMBO_YELLOW           2
#define N_COMBO_GREEN            3
#define N_THRU_RED               4
#define N_THRU_YELLOW            5
#define N_THRU_GREEN             6

/* --- SOUTH APPROACH (Bits 7-17) --- */
#define S_LEFT_GREEN_ARROW       7
#define S_COMBO_RED              8
#define S_COMBO_YELLOW           9
#define S_COMBO_GREEN           10
#define S_THRU_RED              11
#define S_THRU_YELLOW           12
#define S_THRU_GREEN            13
#define S_RIGHT_RED             14
#define S_RIGHT_YELLOW          15
#define S_RIGHT_GREEN_BALL      16
#define S_RIGHT_GREEN_ARROW     17

/* --- WEST APPROACH (Bits 18-24) --- */
#define W_THRU_RED              18
#define W_THRU_YELLOW           19
#define W_THRU_GREEN            20
#define W_RIGHT_RED             21
#define W_RIGHT_YELLOW          22
#define W_RIGHT_GREEN_BALL      23
#define W_RIGHT_GREEN_ARROW     24

/* --- EAST APPROACH (Bits 25-30) --- */
#define E_THRU_LEFT_RED         25
#define E_THRU_LEFT_YELLOW      26
#define E_THRU_LEFT_GREEN       27
#define E_THRU_RIGHT_RED        28
#define E_THRU_RIGHT_YELLOW     29
#define E_THRU_RIGHT_GREEN      30

/* --- SPARE OUTPUT --- */
#define SPARE_OUTPUT            31

/* ============================================================================
 * STATE DEFINITIONS
 * ========================================================================= */

typedef enum {
    /* --- DAYTIME MODE (0-18) - 19 states --- */
    STATE_NS_GREEN = 0,
    STATE_NS_YELLOW,
    STATE_ALL_RED_1,
    STATE_N_LEFT_GREEN,
    STATE_N_LEFT_YELLOW,
    STATE_ALL_RED_2,
    STATE_S_LEFT_GREEN,
    STATE_S_LEFT_YELLOW,
    STATE_ALL_RED_3,
    STATE_W_THRU_GREEN,
    STATE_W_THRU_YELLOW,
    STATE_ALL_RED_4,
    STATE_E_THRU_GREEN,
    STATE_E_THRU_YELLOW,
    STATE_ALL_RED_5,
    STATE_W_RIGHT_GREEN,
    STATE_W_RIGHT_YELLOW,
    STATE_ALL_RED_6,
    STATE_RETURN_TO_START,

    /* --- HIGH TRAFFIC MODE (19-35) - 17 states --- */
    STATE_N_PRIORITY_START = 19,
    STATE_N_SOLO_GREEN,
    STATE_S_LEFT_DURING_N,
    STATE_NS_BOTH_GREEN,
    STATE_N_LEFT_DURING_S,
    STATE_NS_HT_YELLOW,
    STATE_ALL_RED_HT_1,
    STATE_W_THRU_GREEN_HT,
    STATE_W_THRU_YELLOW_HT,
    STATE_ALL_RED_HT_2,
    STATE_E_THRU_GREEN_HT,
    STATE_E_THRU_YELLOW_HT,
    STATE_ALL_RED_HT_3,
    STATE_W_RIGHT_GREEN_HT,
    STATE_W_RIGHT_YELLOW_HT,
    STATE_ALL_RED_HT_4,
    STATE_RETURN_HT,

    /* --- NIGHT MODE (38-40) - 3 states --- */
    STATE_NIGHT_FLASH_ON = 38,
    STATE_NIGHT_FLASH_OFF,
    STATE_NIGHT_TRANSITION,

    /* --- EMERGENCY MODE (41-42) - 2 states --- */
    STATE_EMERGENCY_ALL_RED = 41,
    STATE_EMERGENCY_HOLD,

    STATE_COUNT = 43
} TrafficState;

typedef enum {
    MODE_DAYTIME = 0,
    MODE_HIGH_TRAFFIC,
    MODE_NIGHT,
    MODE_EMERGENCY
} OperatingMode;

/* ============================================================================
 * STATE TIMING (milliseconds)
 * ========================================================================= */

/* Daytime */
#define TIME_NS_GREEN           15000
#define TIME_N_LEFT_GREEN        8000
#define TIME_S_LEFT_GREEN        8000
#define TIME_W_THRU_GREEN       15000
#define TIME_E_THRU_GREEN       15000
#define TIME_W_RIGHT_GREEN       8000
#define TIME_YELLOW              3000
#define TIME_ALL_RED             2000

/* High Traffic */
#define TIME_N_SOLO_GREEN       10000
#define TIME_S_LEFT_DURING_N     8000
#define TIME_NS_BOTH_GREEN      25000
#define TIME_N_LEFT_DURING_S     8000
#define TIME_W_THRU_GREEN_HT   15000
#define TIME_E_THRU_GREEN_HT   15000
#define TIME_W_RIGHT_GREEN_HT   8000

/* Night */
#define TIME_NIGHT_FLASH_ON     1000
#define TIME_NIGHT_FLASH_OFF    1000

/* Emergency */
#define TIME_EMERGENCY_HOLD     5000

/* ============================================================================
 * LED STATE STRUCTURE
 * ========================================================================= */

typedef struct {
    uint8_t byte[4];
} LEDState;

/* ============================================================================
 * HALL EFFECT SENSOR DEMAND FLAGS
 * Set in main.c by sampling P2.4/P2.5, read in getNextState
 * ========================================================================= */

extern volatile bool northLeftDemand;
extern volatile bool southLeftDemand;

/* ============================================================================
 * BUZZER CONTROL FLAGS
 * [0]=North  [1]=South  [2]=East  [3]=West
 * ========================================================================= */

extern volatile bool buzzerActive[4];

/* ============================================================================
 * PEDESTRIAN WALK REQUEST FLAGS
 * ========================================================================= */

extern volatile bool pedWalkRequest[4];

/* ============================================================================
 * FUNCTION PROTOTYPES
 * ========================================================================= */

/* Helper functions */
void initLEDState(LEDState *state);
void clearAllLEDs(LEDState *state);
void setAllRed(LEDState *state);
void setAllYellow(LEDState *state);
void setLED(LEDState *state, uint8_t ledNumber, bool on);
bool getLED(LEDState *state, uint8_t ledNumber);

/* State machine core */
void executeState(LEDState *state, TrafficState currentState);
uint32_t getStateDuration(TrafficState state);
TrafficState getNextState(TrafficState currentState, OperatingMode mode);

/* Daytime state functions (0-18) */
void setState_0_NSGreen(LEDState *state);
void setState_1_NSYellow(LEDState *state);
void setState_2_AllRed1(LEDState *state);
void setState_3_NLeftGreen(LEDState *state);
void setState_4_NLeftYellow(LEDState *state);
void setState_5_AllRed2(LEDState *state);
void setState_6_SLeftGreen(LEDState *state);
void setState_7_SLeftYellow(LEDState *state);
void setState_8_AllRed3(LEDState *state);
void setState_9_WThruGreen(LEDState *state);
void setState_10_WThruYellow(LEDState *state);
void setState_11_AllRed4(LEDState *state);
void setState_12_EThruGreen(LEDState *state);
void setState_13_EThruYellow(LEDState *state);
void setState_14_AllRed5(LEDState *state);
void setState_15_WRightGreen(LEDState *state);
void setState_16_WRightYellow(LEDState *state);
void setState_17_AllRed6(LEDState *state);
void setState_18_ReturnStart(LEDState *state);

/* High Traffic state functions (19-35) */
void setState_19_NPriorityStart(LEDState *state);
void setState_20_NSoloGreen(LEDState *state);
void setState_21_SLeftDuringN(LEDState *state);
void setState_22_NSBothGreen(LEDState *state);
void setState_23_NLeftDuringS(LEDState *state);
void setState_24_NSHTYellow(LEDState *state);
void setState_25_AllRedHT1(LEDState *state);
void setState_26_WThruGreenHT(LEDState *state);
void setState_27_WThruYellowHT(LEDState *state);
void setState_28_AllRedHT2(LEDState *state);
void setState_29_EThruGreenHT(LEDState *state);
void setState_30_EThruYellowHT(LEDState *state);
void setState_31_AllRedHT3(LEDState *state);
void setState_32_WRightGreenHT(LEDState *state);
void setState_33_WRightYellowHT(LEDState *state);
void setState_34_AllRedHT4(LEDState *state);
void setState_35_ReturnHT(LEDState *state);

/* Night state functions (38-40) */
void setState_38_NightFlashOn(LEDState *state);
void setState_39_NightFlashOff(LEDState *state);
void setState_40_NightTransition(LEDState *state);

/* Emergency state functions (41-42) */
void setState_41_EmergencyAllRed(LEDState *state);
void setState_42_EmergencyHold(LEDState *state);

#endif /* TRAFFIC_STATES_H */
