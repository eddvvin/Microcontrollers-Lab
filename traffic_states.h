#ifndef TRAFFIC_STATES_H
#define TRAFFIC_STATES_H

#include <stdint.h>
#include <stdbool.h>

// Northbound signals
#define NS_MAIN_RED              0
#define NS_MAIN_YELLOW           1
#define NS_MAIN_GREEN            2
#define N_LEFT_RED               3
#define N_LEFT_YELLOW_BLINK      4
#define N_LEFT_YELLOW_STEADY     5
#define N_LEFT_GREEN_ARROW       6
#define S_LEFT_RED               7

// Southbound signals
#define S_LEFT_YELLOW_BLINK      8   // FYA
#define S_LEFT_YELLOW_STEADY     9
#define S_LEFT_GREEN_ARROW      10
#define S_RIGHT_RED             11
#define S_RIGHT_YELLOW          12
#define S_RIGHT_GREEN_BALL      13   // Permissive
#define S_RIGHT_GREEN_ARROW     14   // Protected
#define W_THRU_RED              15

// Westbound & Eastbound Through
#define W_THRU_YELLOW           16
#define W_THRU_GREEN            17
#define W_RIGHT_RED             18
#define W_RIGHT_YELLOW          19
#define W_RIGHT_GREEN_BALL      20   // Permissive
#define W_RIGHT_GREEN_ARROW     21   // Protected
#define E_LEFT_RED              22
#define E_LEFT_YELLOW           23

// Eastbound through + spares
#define E_LEFT_GREEN_ARROW      24
#define E_THRU_RED              25
#define E_THRU_YELLOW           26
#define E_THRU_GREEN            27
#define SPARE_LED_1             28
#define SPARE_LED_2             29
#define SPARE_LED_3             30

// State numbering
typedef enum {
    //Normal daytime states(0-17)
    STATE_UNIV_GREEN = 0,
    STATE_UNIV_YELLOW,
    STATE_ALL_RED_1,
    STATE_N_LEFT_GREEN,
    STATE_N_LEFT_YELLOW,
    STATE_ALL_RED_2,
    STATE_S_LEFT_GREEN,
    STATE_S_LEFT_YELLOW,
    STATE_ALL_RED_3,
    STATE_WEST_GREEN,
    STATE_WEST_YELLOW,
    STATE_ALL_RED_4,                 // Split-phase gap
    STATE_EAST_THRU_GREEN,
    STATE_EAST_THRU_YELLOW,
    STATE_ALL_RED_5,
    STATE_E_LEFT_GREEN,
    STATE_E_LEFT_YELLOW,
    STATE_ALL_RED_6,

    // High traffic states(18-36)
    STATE_NORTH_PRIORITY = 18,
    STATE_N_ARROW_ENDING,
    STATE_N_THRU_BONUS,
    STATE_N_THRU_YELLOW,
    STATE_ALL_RED_HT_1,
    STATE_SOUTH_GREEN_HT,
    STATE_SOUTH_YELLOW_HT,
    STATE_ALL_RED_HT_2,
    STATE_S_LEFT_GREEN_HT,
    STATE_S_LEFT_YELLOW_HT,
    STATE_ALL_RED_HT_3,
    STATE_WEST_GREEN_HT,
    STATE_WEST_YELLOW_HT,
    STATE_ALL_RED_HT_4,
    STATE_EAST_THRU_GREEN_HT,
    STATE_EAST_THRU_YELLOW_HT,
    STATE_ALL_RED_HT_5,
    STATE_E_LEFT_GREEN_HT,
    STATE_E_LEFT_YELLOW_HT,
    
    //night mode states(37-38)
    STATE_NIGHT_OFF = 37,
    STATE_NIGHT_ON,


    // Emergency override states(39-42)
    STATE_EMERG_WARNING = 39,
    STATE_EMERG_HOLD,
    STATE_EMERG_CLEARING,
    STATE_EMERG_RETURN,
    
    STATE_COUNT = 43
} TrafficState;

// Operating modes
typedef enum {
    MODE_DAYTIME = 0,
    MODE_HIGH_TRAFFIC,
    MODE_NIGHT,
    MODE_EMERGENCY
} OperatingMode;

// State timing (milliseconds)
#define TIME_UNIV_GREEN         20000
#define TIME_N_LEFT_GREEN       12000
#define TIME_S_LEFT_GREEN       12000
#define TIME_WEST_GREEN         15000
#define TIME_EAST_THRU_GREEN    15000
#define TIME_E_LEFT_GREEN       8000

#define TIME_YELLOW             4000
#define TIME_YELLOW_ARROW       3000
#define TIME_ALL_RED            2000

#define TIME_N_PRIORITY         30000
#define TIME_N_ARROW_END        2000
#define TIME_N_THRU_BONUS       8000
#define TIME_N_THRU_YELLOW_HT   4000
#define TIME_SOUTH_GREEN_HT     25000
#define TIME_S_LEFT_GREEN_HT    10000
#define TIME_WEST_GREEN_HT      8000
#define TIME_EAST_THRU_HT       8000

#define TIME_NIGHT_FLASH        1000

#define TIME_EMERG_WARNING      2000
#define TIME_EMERG_HOLD_MIN     4000
#define TIME_EMERG_CLEARING     2000
#define TIME_EMERG_RETURN       1500

// Hardware representation: 4 bytes for 4 shift register chips
typedef struct {
    uint8_t byte[4];
} LEDState;

void initLEDState(LEDState *state);
void clearAllLEDs(LEDState *state);
void setAllRed(LEDState *state);
void setAllYellow(LEDState *state);

void setLED(LEDState *state, uint8_t ledNumber, bool on);
bool getLED(LEDState *state, uint8_t ledNumber);

// Daytime Normal States (0-17)
void setState_0_UnivGreen(LEDState *state);
void setState_1_UnivYellow(LEDState *state);
void setState_2_AllRed1(LEDState *state);
void setState_3_NLeftGreen(LEDState *state);
void setState_4_NLeftYellow(LEDState *state);
void setState_5_AllRed2(LEDState *state);
void setState_6_SLeftGreen(LEDState *state);
void setState_7_SLeftYellow(LEDState *state);
void setState_8_AllRed3(LEDState *state);
void setState_9_WestGreen(LEDState *state);
void setState_10_WestYellow(LEDState *state);
void setState_11_AllRed4(LEDState *state);
void setState_12_EastThruGreen(LEDState *state);
void setState_13_EastThruYellow(LEDState *state);
void setState_14_AllRed5(LEDState *state);
void setState_15_ELeftGreen(LEDState *state);
void setState_16_ELeftYellow(LEDState *state);
void setState_17_AllRed6(LEDState *state);

// High Traffic States (18-36)
void setState_18_NorthPriority(LEDState *state);
void setState_19_NArrowEnding(LEDState *state);
void setState_20_NThruBonus(LEDState *state);
void setState_21_NThruYellow(LEDState *state);
void setState_22_AllRedHT1(LEDState *state);
void setState_23_SouthGreenHT(LEDState *state);
void setState_24_SouthYellowHT(LEDState *state);
void setState_25_AllRedHT2(LEDState *state);
void setState_26_SLeftGreenHT(LEDState *state);
void setState_27_SLeftYellowHT(LEDState *state);
void setState_28_AllRedHT3(LEDState *state);
void setState_29_WestGreenHT(LEDState *state);
void setState_30_WestYellowHT(LEDState *state);
void setState_31_AllRedHT4(LEDState *state);
void setState_32_EastThruGreenHT(LEDState *state);
void setState_33_EastThruYellowHT(LEDState *state);
void setState_34_AllRedHT5(LEDState *state);
void setState_35_ELeftGreenHT(LEDState *state);
void setState_36_ELeftYellowHT(LEDState *state);

// Night Mode States (37-38)
void setState_37_NightOff(LEDState *state);
void setState_38_NightOn(LEDState *state);

// Emergency Override States (39-42)
void setState_39_EmergWarning(LEDState *state);
void setState_40_EmergHold(LEDState *state);
void setState_41_EmergClearing(LEDState *state);
void setState_42_EmergReturn(LEDState *state);

void executeState(LEDState *state, TrafficState currentState);
TrafficState getNextState(TrafficState currentState, OperatingMode mode, 
                         bool vehicleNorthLeft, bool vehicleSouthLeft, 
                         bool vehicleEastLeft, bool emergencyButton);
uint32_t getStateDuration(TrafficState state);

#endif 
