#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include "traffic_states.h"
#include <msp430fr6989.h>
#include <driverlib.h>

//For testing left and south demand
#define NORTH_LEFT_PIN   BIT4   // P2.4
#define SOUTH_LEFT_PIN   BIT5   // P2.5
void initLeftTurnSensors(void);

// ============================================================================
// PIN DEFINITIONS - SHIFT REGISTER (Traffic LEDs)
// ============================================================================
#define DATA_PIN        BIT0    // P2.0 - Serial data
#define SHIFT_CLK_PIN   BIT1    // P2.1 - Shift clock
#define LATCH_CLK_PIN   BIT2    // P2.2 - Latch clock

// ============================================================================
// PIN DEFINITIONS - MAX7219 (Pedestrian LED Matrices)
// ============================================================================
#define MAT_LATCH_PIN   BIT6    // P9.6 - Matrix latch
#define MAT_DATA_PIN    BIT2    // P4.2 - Matrix data
#define MAT_CLK_PIN     BIT6    // P8.6 - Matrix clock

// ============================================================================
// PIN DEFINITIONS - BUTTONS - Active LOW with pull-ups
// ============================================================================
#define BTN_PED_NORTH           BIT3    // P3.3
#define BTN_PED_WEST            BIT6    // P3.6
#define BTN_PED_SOUTH           BIT0    // P4.0
#define BTN_PED_EAST            BIT1    // P4.1

// ============================================================================
// PIN DEFINITIONS - ACTIVE BUZZERS (P9) - Active HIGH
// One buzzer per matrix corner
// ============================================================================
#define BUZZ_NORTH      BIT0    // P9.0
#define BUZZ_SOUTH      BIT1    // P9.1
#define BUZZ_EAST       BIT2    // P9.2
#define BUZZ_WEST       BIT3    // P9.3

// Buzzer timing in milliseconds
// During WALK: slow steady beep
// During COUNTDOWN: accelerates as counter approaches 0
#define BEEP_ON_MS          40      // beep pulse duration
#define GAP_WALK_MS         700     // walk phase - slow steady beep (1Hz overall)
#define GAP_COUNTDOWN_MAX   700     // counter == 9 -> matches walk pace
#define GAP_COUNTDOWN_MIN   200      // counter == 0 -> very fast, near continuous

// ============================================================================
// PEDESTRIAN MATRIX DEFINITIONS
// ============================================================================
#define NUM_DEVICES     4
#define NUM_OF_DIGIT    12
#define LSBFIRST        0
#define MSBFIRST        1

// Device index to intersection direction mapping
// NOTE: PED_SOUTH and PED_EAST swapped from 1/2 to 2/1
// This corrects the physical matrix positions on the board without
// changing the daisy chain wiring order.
#define PED_NORTH       0
#define PED_SOUTH       2
#define PED_EAST        1
#define PED_WEST        3

// Walk display time (seconds) before countdown begins
#define WALK_TIME_NS    8
#define WALK_TIME_EW    8

typedef enum {
    STATE_HAND,
    STATE_WALK,
    STATE_COUNTDOWN
} State_walking;

// ============================================================================
// IR RECEIVER DEFINITIONS
// ============================================================================
#define CaptureBufferSize 68
#define totalTimings 67
#define UpperLeader 9500
#define LowerLeader 8500
#define NUM_CHANNELS 4

typedef struct {
    volatile uint16_t buffer[CaptureBufferSize];
    volatile uint16_t lastCapture;
    volatile uint8_t frameComplete;
    volatile uint8_t captureIndex;
    volatile uint8_t leaderDetected;
} IR_Channel;

// =========================
// IR REMOTE CODES
// =========================
#define IR_0            0xE916FF00
#define IR_1            0xF30CFF00
#define IR_2            0xE718FF00
#define IR_3            0xA15EFF00
#define IR_4            0xF708FF00
#define IR_5            0xE31CFF00
#define IR_6            0xA55AFF00
#define IR_7            0xBD42FF00
#define IR_8            0xAD52FF00
#define IR_9            0xB54AFF00

#define IR_VolPlus      0xB946FF00
#define IR_VolMinus     0xEA15FF00
#define IR_UpArrow      0xF609FF00
#define IR_DownArrow    0xF807FF00
#define IR_BackButton   0xBB44FF00
#define IR_PauseButton  0xBF40FF00
#define IR_FastFoward   0xBC43FF00
#define IR_FuncStop     0xB847FF00
#define IR_EQ           0xE619FF00
#define IR_STREPT       0xF20DFF00

// ============================================================================
// GLOBAL VARIABLES - TRAFFIC SIGNAL
// ============================================================================
LEDState currentLEDs;
TrafficState currentState    = STATE_NS_GREEN;
OperatingMode currentMode    = MODE_DAYTIME;
volatile uint32_t systemTick = 0;
volatile uint32_t stateTimer = 0;
volatile bool stateExpired   = false;

/* ============================================================================
 * EMERGENCY SAVE/RESTORE
 * ========================================================================= */
volatile TrafficState savedState;
volatile OperatingMode savedMode;
volatile bool inEmergency;

// ============================================================================
// HALL EFFECT SENSOR DEMAND FLAGS
// Set by Port_2 ISR when a vehicle is detected in left turn lane
// Currently used as latch flags - sensors are also polled directly in
// getNextState() for phase-skip logic in traffic_states.c
// ============================================================================
volatile bool northLeftDemand = false;
volatile bool southLeftDemand = false;

// ============================================================================
// GLOBAL VARIABLES - PEDESTRIAN MATRICES
// ============================================================================
volatile State_walking state_walking[NUM_DEVICES];
volatile uint8_t walk_counter[NUM_DEVICES];
volatile uint8_t walk_display_time[NUM_DEVICES];
volatile uint8_t tick_1s = 0;
volatile uint16_t msCount = 0;

// Walk request flags - set by button press, cleared after start_walk()
volatile bool pedWalkRequest[NUM_DEVICES];

// Tracks whether a button-press extension has already been used this phase
// Cleared at the start of each new vehicle green phase via triggerPedWalk()
// Limits each pedestrian to one walk-extension per green phase
volatile bool pedExtendUsed[NUM_DEVICES];

// ============================================================================
// GLOBAL VARIABLES - BUZZERS
// Each buzzer is independently driven from the 1ms Timer_B0 ISR.
// timerMs[i]    counts milliseconds within the current beep cycle
// phaseOn[i]    true = currently in BEEP_ON_MS pulse, false = in gap
// ============================================================================
volatile uint16_t buzzTimerMs[NUM_DEVICES];
volatile bool     buzzPhaseOn[NUM_DEVICES];

// ============================================================================
// GLOBAL VARIABLES - IR RECEIVERS
// ============================================================================
IR_Channel ir[NUM_CHANNELS];
volatile uint32_t result[NUM_CHANNELS];

// ============================================================================
// PEDESTRIAN IMAGE TABLE
// 0-9: digits | 10: stop hand | 11: walking person
// ============================================================================
const uint8_t images[NUM_OF_DIGIT][8] = {
    { 0b00000000, 0b01111100, 0b11111110, 0b10100010,
      0b10010010, 0b11111110, 0b01111100, 0b00000000 },  // 0
    { 0b00000000, 0b00000000, 0b00000010, 0b11111110,
      0b11111110, 0b01000010, 0b00000000, 0b00000000 },  // 1
    { 0b00000000, 0b01100010, 0b11110010, 0b10011010,
      0b10001110, 0b11000110, 0b01000010, 0b00000000 },  // 2
    { 0b00000000, 0b01101100, 0b11111110, 0b10010010,
      0b10010010, 0b11000110, 0b01000100, 0b00000000 },  // 3
    { 0b00000000, 0b00001000, 0b11111110, 0b11111110,
      0b01001000, 0b00101000, 0b00011000, 0b00000000 },  // 4
    { 0b00000000, 0b10011100, 0b10111110, 0b10100010,
      0b10100010, 0b11100110, 0b11100100, 0b00000000 },  // 5
    { 0b00000000, 0b01001100, 0b11011110, 0b10010010,
      0b10010010, 0b11111110, 0b01111100, 0b00000000 },  // 6
    { 0b00000000, 0b11000000, 0b11100000, 0b10110000,
      0b10011110, 0b10001110, 0b10000000, 0b00000000 },  // 7
    { 0b00000000, 0b01101100, 0b11111110, 0b10010010,
      0b10010010, 0b11111110, 0b01101100, 0b00000000 },  // 8
    { 0b00000000, 0b01111100, 0b11111110, 0b10010010,
      0b10010010, 0b11110110, 0b01100100, 0b00000000 },  // 9
    { 0b00000000, 0b00000000, 0b11111100, 0b11111101,
      0b11111101, 0b11111100, 0b00000000, 0b00000000 },  // 10 - stop hand
    { 0b00000000, 0b00010010, 0b01010100, 0b11111000,
      0b11111000, 0b01010100, 0b00010010, 0b00000000 }   // 11 - walking person
};

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================
void System_init(void);
void GPIO_init(void);
void Timer_init(void);
void shiftOut32bits(uint8_t *data);
OperatingMode checkModeButtons(volatile uint32_t *result);
void handleModeChange(OperatingMode newMode);
void checkPedButtons(void);
void triggerPedWalk(TrafficState state);

void matrixPinInit(void);
void matrixClockLow(void);
void matrixClockHigh(void);
void matrixLatchLow(void);
void matrixLatchHigh(void);
void matrixDataLow(void);
void matrixDataHigh(void);
void matrixShiftOut(uint8_t bitOrder, uint16_t value);
void sendMatrixPacket(uint8_t address, uint8_t data[NUM_DEVICES]);
void ledMatrixInit(void);
void sendMatrixImage(uint8_t digits[NUM_DEVICES]);
void updatePedStateMachine(void);
void displayPedState(void);
void start_walk(uint8_t device, uint8_t walkTime);
void Timer1_init(void);

uint32_t decodeNEC(volatile uint16_t *buffer);
void handleCapture(IR_Channel *ch, uint16_t currentcapture);
void initTimerA0Capture(void);
void initTimerA1Capture(void);
void initTimerAContinuousMode(void);
void initPins(void);
void InitIRChannels(void);
void TimerEnableCCRI(void);

void buzzerInit(void);
void setBuzzerPin(uint8_t device, bool on);
uint16_t buzzerGapForDevice(uint8_t device);
void serviceBuzzers(void);

// Returns true if any pedestrian matrix is currently in WALK or COUNTDOWN
// Used to block traffic phase transitions until all pedestrians finish crossing
bool anyPedestrianActive(void);

// ============================================================================
// MAIN FUNCTION
// ============================================================================
int main(void) {
    OperatingMode requestedMode;
    bool ledsNeedUpdate;
    int i;

    WDTCTL = WDTPW | WDTHOLD;

    PM5CTL0 &= ~LOCKLPM5;

    System_init();
    GPIO_init();
    initLeftTurnSensors();
    Timer_init();
    matrixPinInit();
    ledMatrixInit();
    buzzerInit();

    initLEDState(&currentLEDs);
    setAllRed(&currentLEDs);
    shiftOut32bits(currentLEDs.byte);

    for (i = 0; i < NUM_DEVICES; i++) {
        state_walking[i]     = STATE_HAND;
        pedWalkRequest[i]    = false;
        pedExtendUsed[i]     = false;
        walk_counter[i]      = 0;
        walk_display_time[i] = 0;
        buzzTimerMs[i]       = 0;
        buzzPhaseOn[i]       = false;
    }

    displayPedState();

    InitIRChannels();
    initPins();
    initTimerA0Capture();
    initTimerA1Capture();
    TimerEnableCCRI();
    initTimerAContinuousMode();
    __enable_interrupt();

    currentState = STATE_NS_GREEN;
    currentMode  = MODE_DAYTIME;
    stateTimer   = getStateDuration(currentState);

    executeState(&currentLEDs, currentState);
    shiftOut32bits(currentLEDs.byte);

    while (1) {
        for (unsigned int j = 0; j < NUM_CHANNELS; j++) {
            if (ir[j].frameComplete) {
                result[j] = decodeNEC(ir[j].buffer);
                ir[j].frameComplete = 0;
                ir[j].leaderDetected = 0;
            }
        }

        ledsNeedUpdate = false;

        requestedMode = checkModeButtons(result);
        for (unsigned int j = 0; j < NUM_CHANNELS; j++) result[j] = 0;
        if (requestedMode != currentMode) {
            handleModeChange(requestedMode);
            ledsNeedUpdate = true;
        }

        checkPedButtons();

        if (stateExpired) {
            // CRITICAL: do not advance state if any pedestrian is still
            // walking or counting down. Hold the green phase until all
            // pedestrians have finished crossing (state returns to HAND).
            // Emergency mode bypasses this check - safety override always wins.
            if (!inEmergency && anyPedestrianActive()) {
                // Reload stateTimer with a small holdover so we keep
                // checking each loop iteration without spinning
                stateTimer = 1000;  // 1 second holdover, will retry
                stateExpired = false;
            }
            else {
                stateExpired = false;

                if (inEmergency) {
                if (currentState == STATE_EMERGENCY_ALL_RED) {
                    currentState = STATE_EMERGENCY_HOLD;
                    stateTimer   = getStateDuration(STATE_EMERGENCY_HOLD);
                    ledsNeedUpdate = true;
                }
                else {
                    inEmergency  = false;
                    currentMode  = savedMode;
                    currentState = savedState;
                    stateTimer   = getStateDuration(savedState);
                    ledsNeedUpdate = true;
                }
            }
            else {
                currentState = getNextState(currentState, currentMode);

                if (currentMode == MODE_DAYTIME) {
                    if (currentState > STATE_RETURN_TO_START)
                        currentState = STATE_NS_GREEN;
                }
                else if (currentMode == MODE_HIGH_TRAFFIC) {
                    if (currentState < STATE_N_PRIORITY_START ||
                        currentState > STATE_RETURN_HT)
                        currentState = STATE_N_PRIORITY_START;
                }

                stateTimer = getStateDuration(currentState);
                triggerPedWalk(currentState);
                ledsNeedUpdate = true;
            }
            }  // end of !anyPedestrianActive else branch
        }

        if (ledsNeedUpdate) {
            executeState(&currentLEDs, currentState);
            shiftOut32bits(currentLEDs.byte);
        }

        // 1-second pedestrian update
        if (tick_1s) {
            tick_1s = 0;

            for (i = 0; i < NUM_DEVICES; i++) {
                if (pedWalkRequest[i]) {
                    start_walk(i, WALK_TIME_NS);
                    pedWalkRequest[i] = false;
                }
            }

            updatePedStateMachine();
            displayPedState();
        }

        // Service buzzers - runs every loop iteration (woken by 1ms tick)
        serviceBuzzers();

        __bis_SR_register(LPM0_bits + GIE);
        __no_operation();
    }
}

// ============================================================================
// PEDESTRIAN BUTTON HANDLER
// Two behaviors based on current matrix state:
//
//   1. STATE_HAND during a green phase: queue a fresh walk request
//   2. STATE_WALK with extension unused: reset walk timer to full duration
//      (one extension per green phase per direction)
//
// Presses during STATE_COUNTDOWN are ignored - too late to extend.
// ============================================================================
void checkPedButtons(void) {
    uint8_t p3btn = P3IN;
    uint8_t p4btn = P4IN;

    // North - P3.3
    if (!(p3btn & BTN_PED_NORTH)) {
        if (state_walking[PED_NORTH] == STATE_WALK &&
            !pedExtendUsed[PED_NORTH]) {
            walk_display_time[PED_NORTH] = WALK_TIME_NS;
            pedExtendUsed[PED_NORTH] = true;
        }
        else if (currentState == STATE_NS_GREEN ||
                 currentState == STATE_NS_BOTH_GREEN) {
            pedWalkRequest[PED_NORTH] = true;
        }
    }

    // South - P4.0
    if (!(p4btn & BTN_PED_SOUTH)) {
        if (state_walking[PED_SOUTH] == STATE_WALK &&
            !pedExtendUsed[PED_SOUTH]) {
            walk_display_time[PED_SOUTH] = WALK_TIME_NS;
            pedExtendUsed[PED_SOUTH] = true;
        }
        else if (currentState == STATE_NS_GREEN ||
                 currentState == STATE_NS_BOTH_GREEN) {
            pedWalkRequest[PED_SOUTH] = true;
        }
    }

    // East - P4.1
    if (!(p4btn & BTN_PED_EAST)) {
        if (state_walking[PED_EAST] == STATE_WALK &&
            !pedExtendUsed[PED_EAST]) {
            walk_display_time[PED_EAST] = WALK_TIME_EW;
            pedExtendUsed[PED_EAST] = true;
        }
        else if (currentState == STATE_E_THRU_GREEN ||
                 currentState == STATE_E_THRU_GREEN_HT) {
            pedWalkRequest[PED_EAST] = true;
        }
    }

    // West - P3.6
    if (!(p3btn & BTN_PED_WEST)) {
        if (state_walking[PED_WEST] == STATE_WALK &&
            !pedExtendUsed[PED_WEST]) {
            walk_display_time[PED_WEST] = WALK_TIME_EW;
            pedExtendUsed[PED_WEST] = true;
        }
        else if (currentState == STATE_W_THRU_GREEN ||
                 currentState == STATE_W_THRU_GREEN_HT) {
            pedWalkRequest[PED_WEST] = true;
        }
    }
}

// ============================================================================
// AUTO PEDESTRIAN TRIGGER
// Called on every state transition - only fires walk on green states
// Also resets the per-phase extension flag for affected directions so each
// pedestrian gets a fresh extension opportunity at the start of every green
// ============================================================================
void triggerPedWalk(TrafficState state) {
    switch (state) {
        case STATE_NS_GREEN:
        case STATE_NS_BOTH_GREEN:
            pedExtendUsed[PED_NORTH] = false;
            pedExtendUsed[PED_SOUTH] = false;
            start_walk(PED_NORTH, WALK_TIME_NS);
            start_walk(PED_SOUTH, WALK_TIME_NS);
            break;
        case STATE_W_THRU_GREEN:
        case STATE_W_THRU_GREEN_HT:
            pedExtendUsed[PED_WEST] = false;
            start_walk(PED_WEST, WALK_TIME_EW);
            break;
        case STATE_E_THRU_GREEN:
        case STATE_E_THRU_GREEN_HT:
            pedExtendUsed[PED_EAST] = false;
            start_walk(PED_EAST, WALK_TIME_EW);
            break;
        default:
            break;
    }
}

// ============================================================================
// PEDESTRIAN STATE MACHINE UPDATE
// ============================================================================
void updatePedStateMachine(void) {
    int i;
    for (i = 0; i < NUM_DEVICES; i++) {
        switch (state_walking[i]) {
            case STATE_WALK:
                if (walk_display_time[i] > 0) walk_display_time[i]--;
                if (walk_display_time[i] == 0) {
                    state_walking[i] = STATE_COUNTDOWN;
                    walk_counter[i]  = 9;
                }
                break;

            case STATE_COUNTDOWN:
                if (walk_counter[i] > 0) walk_counter[i]--;
                else state_walking[i] = STATE_HAND;
                break;

            default:
                break;
        }
    }
}

void displayPedState(void) {
    uint8_t display_all[NUM_DEVICES];
    int i;
    for (i = 0; i < NUM_DEVICES; i++) {
        switch (state_walking[i]) {
            case STATE_HAND:      display_all[i] = 10;              break;
            case STATE_WALK:      display_all[i] = 11;              break;
            case STATE_COUNTDOWN: display_all[i] = walk_counter[i]; break;
            default:              display_all[i] = 10;              break;
        }
    }
    sendMatrixImage(display_all);
}

void start_walk(uint8_t device, uint8_t walkTime) {
    if (state_walking[device] == STATE_HAND) {
        state_walking[device]     = STATE_WALK;
        walk_display_time[device] = walkTime;
    }
}

// ============================================================================
// PEDESTRIAN ACTIVITY CHECK
// Returns true if ANY direction is currently walking or counting down.
// Used to block traffic phase transition until pedestrians finish crossing.
// ============================================================================
bool anyPedestrianActive(void) {
    uint8_t i;
    for (i = 0; i < NUM_DEVICES; i++) {
        if (state_walking[i] == STATE_WALK ||
            state_walking[i] == STATE_COUNTDOWN) {
            return true;
        }
    }
    return false;
}

// ============================================================================
// BUZZER CONTROL - 4 INDEPENDENT BUZZERS
//
// Each buzzer mirrors the state of its corresponding pedestrian matrix:
//   STATE_HAND      -> silent
//   STATE_WALK      -> slow steady beep, 40ms on, 500ms gap
//   STATE_COUNTDOWN -> accelerating beep
//                      counter 9: 500ms gap (matches walk pace)
//                      counter 0: 25ms gap (near continuous)
//
// All timing driven from 1ms Timer_B0 ISR via buzzTimerMs[] counter.
// ============================================================================
void buzzerInit(void) {
    P9DIR |=  (BUZZ_NORTH | BUZZ_SOUTH | BUZZ_EAST | BUZZ_WEST);
    P9OUT &= ~(BUZZ_NORTH | BUZZ_SOUTH | BUZZ_EAST | BUZZ_WEST);
    P9SEL0 &= ~(BUZZ_NORTH | BUZZ_SOUTH | BUZZ_EAST | BUZZ_WEST);
    P9SEL1 &= ~(BUZZ_NORTH | BUZZ_SOUTH | BUZZ_EAST | BUZZ_WEST);
}

void setBuzzerPin(uint8_t device, bool on) {
    uint8_t mask;
    switch (device) {
        case PED_NORTH: mask = BUZZ_NORTH; break;
        case PED_SOUTH: mask = BUZZ_SOUTH; break;
        case PED_EAST:  mask = BUZZ_EAST;  break;
        case PED_WEST:  mask = BUZZ_WEST;  break;
        default: return;
    }
    if (on) P9OUT |= mask;
    else    P9OUT &= ~mask;
}

// Returns the current gap (off-time) in ms for this device based on its state
// Returns 0 when buzzer should be silent
uint16_t buzzerGapForDevice(uint8_t device) {
    uint32_t span;
    uint16_t gap;

    switch (state_walking[device]) {
        case STATE_HAND:
            return 0;  // silent

        case STATE_WALK:
            return GAP_WALK_MS;

        case STATE_COUNTDOWN:
            // Linear interpolation between MAX (counter=9) and MIN (counter=0)
            // gap = MIN + (MAX - MIN) * counter / 9
            span = (uint32_t)(GAP_COUNTDOWN_MAX - GAP_COUNTDOWN_MIN);
            gap = (uint16_t)(GAP_COUNTDOWN_MIN +
                             (span * walk_counter[device]) / 9U);
            return gap;

        default:
            return 0;
    }
}

void serviceBuzzers(void) {
    uint8_t i;
    uint16_t gap;

    for (i = 0; i < NUM_DEVICES; i++) {
        gap = buzzerGapForDevice(i);

        // Silent state - turn off and reset
        if (gap == 0) {
            setBuzzerPin(i, false);
            buzzPhaseOn[i] = false;
            buzzTimerMs[i] = 0;
            continue;
        }

        // Active state - manage beep on/off cycle
        if (buzzPhaseOn[i]) {
            // Currently in pulse - check if pulse duration elapsed
            if (buzzTimerMs[i] >= BEEP_ON_MS) {
                setBuzzerPin(i, false);
                buzzPhaseOn[i] = false;
                buzzTimerMs[i] = 0;
            }
        } else {
            // Currently in gap - check if gap duration elapsed
            if (buzzTimerMs[i] >= gap) {
                setBuzzerPin(i, true);
                buzzPhaseOn[i] = true;
                buzzTimerMs[i] = 0;
            }
        }
    }
}

// ============================================================================
// INITIALIZATION
// ============================================================================
void System_init(void) {
    // 1 MHz default DCO
}

void GPIO_init(void) {
    P2DIR |=  (DATA_PIN | SHIFT_CLK_PIN | LATCH_CLK_PIN);
    P2OUT &= ~(DATA_PIN | SHIFT_CLK_PIN | LATCH_CLK_PIN);

    P3DIR &= ~(BTN_PED_NORTH | BTN_PED_WEST);
    P3REN |=  (BTN_PED_NORTH | BTN_PED_WEST);
    P3OUT |=  (BTN_PED_NORTH | BTN_PED_WEST);

    P4DIR &= ~(BTN_PED_SOUTH | BTN_PED_EAST);
    P4REN |=  (BTN_PED_SOUTH | BTN_PED_EAST);
    P4OUT |=  (BTN_PED_SOUTH | BTN_PED_EAST);
}

void Timer_init(void) {
    // Timer_B0: 1ms tick - drives traffic state machine, ped 1s tick, and buzzers
    TB0CTL   = TASSEL__SMCLK | MC__UP | ID__8;
    TB0CCR0  = 125;
    TB0CCTL0 = CCIE;
}

// ============================================================================
// MODE CONTROL
// ============================================================================
OperatingMode checkModeButtons(volatile uint32_t *result) {
    for (int i = 0; i < NUM_CHANNELS; i++) {
        if (result[i] == IR_BackButton) return MODE_EMERGENCY;
    }

    OperatingMode newMode = currentMode;
    for (int i = 0; i < NUM_CHANNELS; i++) {
        if (result[i] == IR_UpArrow)         newMode = MODE_DAYTIME;
        else if (result[i] == IR_DownArrow)  newMode = MODE_NIGHT;
        else if (result[i] == IR_FastFoward) newMode = MODE_HIGH_TRAFFIC;
    }
    return newMode;
}

void handleModeChange(OperatingMode newMode) {
    if (newMode == MODE_EMERGENCY) {
        if (!inEmergency) {
            savedState   = currentState;
            savedMode    = currentMode;
            inEmergency  = true;
            currentMode  = MODE_EMERGENCY;
            currentState = STATE_EMERGENCY_ALL_RED;
            stateTimer   = getStateDuration(STATE_EMERGENCY_ALL_RED);
            stateExpired = false;
        }
        return;
    }

    inEmergency  = false;
    currentMode  = newMode;

    switch (newMode) {
        case MODE_DAYTIME:      currentState = STATE_NS_GREEN;         break;
        case MODE_HIGH_TRAFFIC: currentState = STATE_N_PRIORITY_START; break;
        case MODE_NIGHT:        currentState = STATE_NIGHT_FLASH_ON;   break;
        default: break;
    }

    stateTimer   = getStateDuration(currentState);
    stateExpired = false;
}

// ============================================================================
// MATRIX PIN HELPERS
// ============================================================================
void matrixPinInit(void) {
    P9OUT |=  BIT6; P9DIR |= BIT6;
    P9SEL1 &= ~BIT6; P9SEL0 &= ~BIT6;

    P4OUT &= ~BIT2; P4DIR |= BIT2;
    P4SEL1 &= ~BIT2; P4SEL0 &= ~BIT2;

    P8OUT &= ~BIT6; P8DIR |= BIT6;
    P8SEL1 &= ~BIT6; P8SEL0 &= ~BIT6;
}

void matrixClockLow(void)  { P8OUT &= ~BIT6; }
void matrixClockHigh(void) { P8OUT |=  BIT6; }
void matrixLatchLow(void)  { P9OUT &= ~BIT6; }
void matrixLatchHigh(void) { P9OUT |=  BIT6; }
void matrixDataLow(void)   { P4OUT &= ~BIT2; }
void matrixDataHigh(void)  { P4OUT |=  BIT2; }

void matrixShiftOut(uint8_t bitOrder, uint16_t value) {
    uint16_t i;
    for (i = 0; i < 16; i++) {
        if (bitOrder == LSBFIRST) {
            if (value & 0x0001) matrixDataHigh(); else matrixDataLow();
            value >>= 1;
        } else {
            if (value & 0x8000) matrixDataHigh(); else matrixDataLow();
            value <<= 1;
        }
        __delay_cycles(1);
        matrixClockHigh();
        __delay_cycles(1);
        matrixClockLow();
    }
}

void sendMatrixPacket(uint8_t address, uint8_t data[NUM_DEVICES]) {
    int i;
    matrixLatchLow();
    for (i = NUM_DEVICES - 1; i >= 0; i--) {
        uint16_t packet = ((uint16_t)address << 8) | data[i];
        matrixShiftOut(MSBFIRST, packet);
    }
    matrixLatchHigh();
}

void ledMatrixInit(void) {
    uint8_t data[NUM_DEVICES];
    int i;

    for (i = 0; i < NUM_DEVICES; i++) data[i] = 0x00;
    sendMatrixPacket(0x0C, data);
    sendMatrixPacket(0x0F, data);
    sendMatrixPacket(0x09, data);

    for (i = 0; i < NUM_DEVICES; i++) data[i] = 0x07;
    sendMatrixPacket(0x0B, data);

    for (i = 0; i < NUM_DEVICES; i++) data[i] = 0x0F;
    sendMatrixPacket(0x0A, data);

    for (i = 0; i < NUM_DEVICES; i++) data[i] = 0x01;
    sendMatrixPacket(0x0C, data);
}

void sendMatrixImage(uint8_t digits[NUM_DEVICES]) {
    uint8_t row, i;
    uint8_t row_data[NUM_DEVICES];
    for (row = 0; row < 8; row++) {
        for (i = 0; i < NUM_DEVICES; i++) {
            row_data[i] = images[digits[i]][row];
        }
        sendMatrixPacket(row + 1, row_data);
    }
}

// ============================================================================
// SHIFT REGISTER CONTROL
// ============================================================================
void shiftOut32bits(uint8_t *data) {
    int16_t byte_idx, bit_idx;

    P2OUT &= ~LATCH_CLK_PIN;
    __delay_cycles(1);

    for (byte_idx = 3; byte_idx >= 0; byte_idx--) {
        for (bit_idx = 7; bit_idx >= 0; bit_idx--) {
            if (data[byte_idx] & (1 << bit_idx)) P2OUT |=  DATA_PIN;
            else                                  P2OUT &= ~DATA_PIN;
            P2OUT |=  SHIFT_CLK_PIN;
            __delay_cycles(1);
            P2OUT &= ~SHIFT_CLK_PIN;
            __delay_cycles(1);
        }
    }

    P2OUT |=  LATCH_CLK_PIN;
    __delay_cycles(1);
    P2OUT &= ~LATCH_CLK_PIN;
}

// ============================================================================
// IR RECEIVER HELPERS
// ============================================================================
uint32_t decodeNEC(volatile uint16_t *buffer) {
    uint32_t value = 0;
    int index = 2;
    for (int bit = 0; bit < 32; bit++) {
        uint16_t space = buffer[index + 1];
        if (space > 1500) value |= (1UL << bit);
        index += 2;
    }
    return value;
}

void handleCapture(IR_Channel *ch, uint16_t currentcapture) {
    uint16_t duration = currentcapture - ch->lastCapture;
    ch->lastCapture = currentcapture;

    if (duration > LowerLeader && duration < UpperLeader) {
        ch->leaderDetected = 1;
        ch->captureIndex = 0;
    }

    if (ch->leaderDetected) {
        if (ch->captureIndex < CaptureBufferSize) {
            ch->buffer[ch->captureIndex++] = duration;
        }
        if (ch->captureIndex == totalTimings) {
            ch->frameComplete = 1;
            ch->captureIndex = 0;
            ch->leaderDetected = 0;
        }
    }
}

void initTimerA0Capture(void) {
    Timer_A_initCaptureModeParam capture = {0};
    capture.captureMode = TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE;
    capture.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    capture.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    capture.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    capture.captureOutputMode = TIMER_A_OUTPUTMODE_OUTBITVALUE;

    capture.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0;
    Timer_A_initCaptureMode(TIMER_A0_BASE, &capture);
    capture.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    Timer_A_initCaptureMode(TIMER_A0_BASE, &capture);
    capture.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;
    Timer_A_initCaptureMode(TIMER_A0_BASE, &capture);
}

void initTimerA1Capture(void) {
    Timer_A_initCaptureModeParam capture = {0};
    capture.captureMode = TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE;
    capture.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    capture.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    capture.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    capture.captureOutputMode = TIMER_A_OUTPUTMODE_OUTBITVALUE;
    capture.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    Timer_A_initCaptureMode(TIMER_A1_BASE, &capture);
}

void initTimerAContinuousMode(void) {
    Timer_A_initContinuousModeParam continuousmode = {0};
    continuousmode.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    continuousmode.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    continuousmode.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    continuousmode.timerClear = TIMER_A_DO_CLEAR;
    continuousmode.startTimer = true;

    Timer_A_initContinuousMode(TIMER_A0_BASE, &continuousmode);
    Timer_A_initContinuousMode(TIMER_A1_BASE, &continuousmode);
}

void initPins(void) {
    P1DIR &= ~BIT5; P1SEL0 |= BIT5; P1SEL1 |= BIT5;
    P1DIR &= ~BIT6; P1SEL0 |= BIT6; P1SEL1 |= BIT6;
    P1DIR &= ~BIT7; P1SEL0 |= BIT7; P1SEL1 |= BIT7;
    P3DIR &= ~BIT3; P3SEL0 &= ~BIT3; P3SEL1 |= BIT3;
}

void InitIRChannels(void) {
    for (int i = 0; i < NUM_CHANNELS; i++) {
        ir[i].lastCapture = 0;
        ir[i].frameComplete = 0;
        ir[i].captureIndex = 0;
        ir[i].leaderDetected = 0;
        result[i] = 0;
    }
}

void TimerEnableCCRI(void) {
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE,
        TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE,
        TIMER_A_CAPTURECOMPARE_REGISTER_1);
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE,
        TIMER_A_CAPTURECOMPARE_REGISTER_2);
    Timer_A_enableCaptureCompareInterrupt(TIMER_A1_BASE,
        TIMER_A_CAPTURECOMPARE_REGISTER_1);
}

void initLeftTurnSensors(void) {
    P2SEL0 &= ~(NORTH_LEFT_PIN | SOUTH_LEFT_PIN);
    P2SEL1 &= ~(NORTH_LEFT_PIN | SOUTH_LEFT_PIN);
    P2DIR &= ~(NORTH_LEFT_PIN | SOUTH_LEFT_PIN);
    P2REN |= (NORTH_LEFT_PIN | SOUTH_LEFT_PIN);
    P2OUT |= (NORTH_LEFT_PIN | SOUTH_LEFT_PIN);
    P2IES |= (NORTH_LEFT_PIN | SOUTH_LEFT_PIN);
    P2IFG &= ~(NORTH_LEFT_PIN | SOUTH_LEFT_PIN);
    P2IE  |= (NORTH_LEFT_PIN | SOUTH_LEFT_PIN);
}

// ============================================================================
// INTERRUPT SERVICE ROUTINES
// ============================================================================
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_CCR0_ISR(void) {
    uint16_t currentcapture =
        Timer_A_getCaptureCompareCount(TIMER_A0_BASE,
                                       TIMER_A_CAPTURECOMPARE_REGISTER_0);
    handleCapture(&ir[0], currentcapture);
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void) {
    switch (__even_in_range(TA0IV, 4)) {
        case 2: {
            uint16_t currentcapture =
                Timer_A_getCaptureCompareCount(TIMER_A0_BASE,
                                               TIMER_A_CAPTURECOMPARE_REGISTER_1);
            handleCapture(&ir[1], currentcapture);
            break;
        }
        case 4: {
            uint16_t currentcapture =
                Timer_A_getCaptureCompareCount(TIMER_A0_BASE,
                                               TIMER_A_CAPTURECOMPARE_REGISTER_2);
            handleCapture(&ir[2], currentcapture);
            break;
        }
    }
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void) {
    switch (__even_in_range(TA1IV, 2)) {
        case 2: {
            uint16_t current =
                Timer_A_getCaptureCompareCount(TIMER_A1_BASE,
                    TIMER_A_CAPTURECOMPARE_REGISTER_1);
            handleCapture(&ir[3], current);
            break;
        }
    }
}

// Timer_B0 - 1ms tick (traffic + 1s pedestrian + buzzer timing)
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_B0_ISR(void) {
    uint8_t i;
    systemTick++;

    // Increment buzzer timers every 1ms - used by serviceBuzzers()
    for (i = 0; i < NUM_DEVICES; i++) {
        buzzTimerMs[i]++;
    }

    // 1-second tick for pedestrian state machine
    msCount++;
    if (msCount >= 1000) {
        msCount = 0;
        tick_1s = 1;
    }

    // Traffic state timer
    if (stateTimer > 0) {
        stateTimer--;
        if (stateTimer == 0) {
            stateExpired = true;
        }
    }

    // Always wake CPU so main loop can service buzzers every 1ms
    __bic_SR_register_on_exit(LPM0_bits);
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2_ISR(void) {
    if (P2IFG & NORTH_LEFT_PIN) {
        northLeftDemand = true;
        P2IFG &= ~NORTH_LEFT_PIN;
    }
    if (P2IFG & SOUTH_LEFT_PIN) {
        southLeftDemand = true;
        P2IFG &= ~SOUTH_LEFT_PIN;
    }
    P2IFG &= ~(NORTH_LEFT_PIN | SOUTH_LEFT_PIN);
}
