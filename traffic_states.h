#ifndef TRAFFIC_STATES_H
#define TRAFFIC_STATES_H

#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// LED BIT ASSIGNMENTS (Based on Physical Intersection Model - 31 LEDs Total)
// ============================================================================

// --- NORTH APPROACH (Bits 0-6) ---
// Left signal head: 4 LEDs (Combo: Left arrow + Through signals)
#define N_LEFT_GREEN_ARROW       0  // U1 QA (Pin 15)
#define N_COMBO_RED              1  // U1 QB (Pin 1)
#define N_COMBO_YELLOW           2  // U1 QC (Pin 2)
#define N_COMBO_GREEN            3  // U1 QD (Pin 3)

// Right signal head: 3 LEDs (Through only)
#define N_THRU_RED               4  // U1 QE (Pin 4)
#define N_THRU_YELLOW            5  // U1 QF (Pin 5)
#define N_THRU_GREEN             6  // U1 QG (Pin 6)

// --- SOUTH APPROACH (Bits 7-17) ---
// Left signal head: 4 LEDs (Combo: Left arrow + Through signals)
#define S_LEFT_GREEN_ARROW       7  // U1 QH (Pin 7)
#define S_COMBO_RED              8  // U2 QA (Pin 15)
#define S_COMBO_YELLOW           9  // U2 QB (Pin 1)
#define S_COMBO_GREEN            10 // U2 QC (Pin 2)

// Center signal head: 3 LEDs (Through only)
#define S_THRU_RED               11 // U2 QD (Pin 3)
#define S_THRU_YELLOW            12 // U2 QE (Pin 4)
#define S_THRU_GREEN             13 // U2 QF (Pin 5)

// Right turn signal head: 4 LEDs
#define S_RIGHT_RED              14 // U2 QG (Pin 6)
#define S_RIGHT_YELLOW           15 // U2 QH (Pin 7)
#define S_RIGHT_GREEN_BALL       16 // U3 QA (Pin 15)
#define S_RIGHT_GREEN_ARROW      17 // U3 QB (Pin 1)

// --- WEST APPROACH (Bits 18-24) ---
// Through signal head: 3 LEDs
#define W_THRU_RED               18 // U3 QC (Pin 2)
#define W_THRU_YELLOW            19 // U3 QD (Pin 3)
#define W_THRU_GREEN             20 // U3 QE (Pin 4)

// Right turn signal head: 4 LEDs
#define W_RIGHT_RED              21 // U3 QF (Pin 5)
#define W_RIGHT_YELLOW           22 // U3 QG (Pin 6)
#define W_RIGHT_GREEN_BALL       23 // U3 QH (Pin 7)
#define W_RIGHT_GREEN_ARROW      24 // U4 QA (Pin 15)

// --- EAST APPROACH (Bits 25-30) ---
// Left through signal head: 3 LEDs
#define E_THRU_LEFT_RED          25 // U4 QB (Pin 1)
#define E_THRU_LEFT_YELLOW       26 // U4 QC (Pin 2)
#define E_THRU_LEFT_GREEN        27 // U4 QD (Pin 3)

// Right through signal head: 3 LEDs
#define E_THRU_RIGHT_RED         28 // U4 QE (Pin 4)
#define E_THRU_RIGHT_YELLOW      29 // U4 QF (Pin 5)
#define E_THRU_RIGHT_GREEN       30 // U4 QG (Pin 6)

// --- SPARE OUTPUT ---
#define SPARE_OUTPUT             31 // U4 QH (Pin 7) - Available for future use

// ============================================================================
// STATE DEFINITIONS
// ============================================================================

// State numbering
typedef enum {
    // Daytime mode states (0-18) - 19 total
    STATE_NS_GREEN = 0,              // North-South through green
    STATE_NS_YELLOW,                 // North-South through yellow
    STATE_ALL_RED_1,                 // Clearance interval
    STATE_N_LEFT_GREEN,              // North left turn protected
    STATE_N_LEFT_YELLOW,             // North left turn yellow
    STATE_ALL_RED_2,                 // Clearance interval
    STATE_S_LEFT_GREEN,              // South left turn protected
    STATE_S_LEFT_YELLOW,             // South left turn yellow
    STATE_ALL_RED_3,                 // Clearance interval
    STATE_W_THRU_GREEN,
    STATE_W_THRU_YELLOW,
    STATE_ALL_RED_4,                 // Clearance interval
    STATE_E_THRU_GREEN,              // East through (if needed separately)
    STATE_E_THRU_YELLOW,             // East through yellow
    STATE_ALL_RED_5,                 // Clearance interval
    STATE_W_RIGHT_GREEN,             // West right turn (if protected phase needed)
    STATE_W_RIGHT_YELLOW,            // West right turn yellow
    STATE_ALL_RED_6,                 // Final clearance
    STATE_RETURN_TO_START,           // Return to beginning

    // High Traffic mode states (19-37) - 19 total
    STATE_N_PRIORITY_START = 19,     // Northbound priority begins
    STATE_N_PRIORITY_GREEN,          // Northbound through green (25s)
    STATE_N_PRIORITY_YELLOW,         // Northbound through yellow
    STATE_ALL_RED_HT_1,              // Clearance
    STATE_N_LEFT_GREEN_HT,           // North left turn
    STATE_N_LEFT_YELLOW_HT,          // North left yellow
    STATE_ALL_RED_HT_2,              // Clearance
    STATE_S_GREEN_HT,                // South through green
    STATE_S_YELLOW_HT,               // South through yellow
    STATE_ALL_RED_HT_3,              // Clearance
    STATE_S_LEFT_GREEN_HT,           // South left turn
    STATE_S_LEFT_YELLOW_HT,          // South left yellow
    STATE_ALL_RED_HT_4,              // Clearance
    STATE_W_THRU_GREEN_HT,
    STATE_W_THRU_YELLOW_HT,
    STATE_ALL_RED_HT_5,              // Clearance
    STATE_W_RIGHT_GREEN_HT,          // West right (if needed)
    STATE_W_RIGHT_YELLOW_HT,         // West right yellow
    STATE_RETURN_HT,                 // Return to start

    // Night mode states (38-40) - 3 total
    STATE_NIGHT_FLASH_ON = 38,       // Main streets yellow ON, side streets red ON
    STATE_NIGHT_FLASH_OFF,           // All flash signals OFF
    STATE_NIGHT_TRANSITION,          // Transition state

    // Emergency mode states (41-42) - 2 total
    STATE_EMERGENCY_ALL_RED = 41,    // All red - immediate
    STATE_EMERGENCY_HOLD,            // Hold all red until reset

    STATE_COUNT = 43
} TrafficState;

// Operating modes
typedef enum {
    MODE_DAYTIME = 0,
    MODE_HIGH_TRAFFIC,
    MODE_NIGHT,
    MODE_EMERGENCY
} OperatingMode;

// ============================================================================
// STATE TIMING (milliseconds)
// ============================================================================

// Daytime mode timing
#define TIME_NS_GREEN           15000  // 15 seconds
#define TIME_N_LEFT_GREEN        8000  // 8 seconds
#define TIME_S_LEFT_GREEN        8000  // 8 seconds
#define TIME_W_THRU_GREEN       15000
#define TIME_W_RIGHT_GREEN       8000  // 8 seconds (if protected)

#define TIME_YELLOW              3000  // 3 seconds
#define TIME_ALL_RED             2000  // 2 seconds

// High Traffic mode timing
#define TIME_N_PRIORITY         25000  // 25 seconds (extended northbound)
#define TIME_NS_GREEN_HT        15000  // 15 seconds (standard for south)
#define TIME_N_LEFT_GREEN_HT     8000  // 8 seconds
#define TIME_S_LEFT_GREEN_HT     8000  // 8 seconds
#define TIME_W_THRU_GREEN_HT    15000
#define TIME_W_RIGHT_GREEN_HT    8000  // 8 seconds

// Night mode timing
#define TIME_NIGHT_FLASH_ON      1000  // 1 second ON
#define TIME_NIGHT_FLASH_OFF     1000  // 1 second OFF

// Emergency mode timing
#define TIME_EMERGENCY_HOLD      5000  // 5 seconds minimum (can be indefinite)

// ============================================================================
// DATA STRUCTURES
// ============================================================================

// Hardware representation: 4 bytes for 4 shift register chips (32 bits, 31 used)
typedef struct {
    uint8_t byte[4];  // byte[0]=U1, byte[1]=U2, byte[2]=U3, byte[3]=U4
} LEDState;

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================

// LED state management
void initLEDState(LEDState *state);
void clearAllLEDs(LEDState *state);
void setAllRed(LEDState *state);
void setAllYellow(LEDState *state);

// Individual LED control
void setLED(LEDState *state, uint8_t ledNumber, bool on);
bool getLED(LEDState *state, uint8_t ledNumber);

// State execution
void executeState(LEDState *state, TrafficState currentState);
TrafficState getNextState(TrafficState currentState, OperatingMode mode);
uint32_t getStateDuration(TrafficState state);

// Daytime mode state functions (0-18)
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

// High Traffic mode state functions (19-37)
void setState_19_NPriorityStart(LEDState *state);
void setState_20_NPriorityGreen(LEDState *state);
void setState_21_NPriorityYellow(LEDState *state);
void setState_22_AllRedHT1(LEDState *state);
void setState_23_NLeftGreenHT(LEDState *state);
void setState_24_NLeftYellowHT(LEDState *state);
void setState_25_AllRedHT2(LEDState *state);
void setState_26_SGreenHT(LEDState *state);
void setState_27_SYellowHT(LEDState *state);
void setState_28_AllRedHT3(LEDState *state);
void setState_29_SLeftGreenHT(LEDState *state);
void setState_30_SLeftYellowHT(LEDState *state);
void setState_31_AllRedHT4(LEDState *state);
void setState_32_WThruGreenHT(LEDState *state);
void setState_33_WThruYellowHT(LEDState *state);
void setState_34_AllRedHT5(LEDState *state);
void setState_35_WRightGreenHT(LEDState *state);
void setState_36_WRightYellowHT(LEDState *state);
void setState_37_ReturnHT(LEDState *state);

// Night mode state functions (38-40)
void setState_38_NightFlashOn(LEDState *state);
void setState_39_NightFlashOff(LEDState *state);
void setState_40_NightTransition(LEDState *state);

// Emergency mode state functions (41-42)
void setState_41_EmergencyAllRed(LEDState *state);
void setState_42_EmergencyHold(LEDState *state);

#endif // TRAFFIC_STATES_H
