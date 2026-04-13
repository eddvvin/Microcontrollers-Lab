#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include "traffic_states.h"

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
#define MAT_CLK_PIN     BIT5    // P1.5 - Matrix clock

// ============================================================================
// PIN DEFINITIONS - BUTTONS - Active LOW with pull-ups
// ============================================================================

// No longer using these buttons
//#define BTN_MODE_DAYTIME        BIT0    // P3.0
//#define BTN_MODE_HIGH_TRAFFIC   BIT1    // P3.1
//#define BTN_MODE_NIGHT          BIT2    // P3.2
#define BTN_PED_NORTH           BIT3    // P3.3
#define BTN_PED_WEST            BIT6    // P3.6
#define BTN_PED_SOUTH           BIT0    // P4.0
#define BTN_PED_EAST            BIT1    // P4.1

// ============================================================================
// PEDESTRIAN MATRIX DEFINITIONS
// ============================================================================
#define NUM_DEVICES     4
#define NUM_OF_DIGIT    12
#define LSBFIRST        0
#define MSBFIRST        1

// Device index to intersection direction mapping
#define PED_NORTH       0
#define PED_SOUTH       1
#define PED_EAST        2
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
// GLOBAL VARIABLES - TRAFFIC SIGNAL
// ============================================================================
LEDState currentLEDs;
TrafficState currentState    = STATE_NS_GREEN;
OperatingMode currentMode    = MODE_DAYTIME;
volatile uint32_t systemTick = 0;
volatile uint32_t stateTimer = 0;
volatile bool stateExpired   = false;

// ============================================================================
// GLOBAL VARIABLES - PEDESTRIAN MATRICES
// ============================================================================
volatile State_walking state_walking[NUM_DEVICES];
volatile uint8_t walk_counter[NUM_DEVICES];
volatile uint8_t walk_display_time[NUM_DEVICES];
volatile uint8_t tick_1s = 0;

// Walk request flags - set by button press, cleared after start_walk()
volatile bool pedWalkRequest[NUM_DEVICES];

// ============================================================================
// PEDESTRIAN IMAGE TABLE
// 0-9: digits | 10: stop hand | 11: walking person
// ============================================================================
const uint8_t images[NUM_OF_DIGIT][8] = {
    { 0b00111100, 0b01100110, 0b01101110, 0b01110110,   // 0
      0b01100110, 0b01100110, 0b00111100, 0b00000000 },
    { 0b00011000, 0b00111000, 0b00011000, 0b00011000,   // 1
      0b00011000, 0b00011000, 0b00111100, 0b00000000 },
    { 0b00111100, 0b01100110, 0b00000110, 0b00001100,   // 2
      0b00011000, 0b00110000, 0b01111110, 0b00000000 },
    { 0b00111100, 0b01100110, 0b00000110, 0b00011100,   // 3
      0b00000110, 0b01100110, 0b00111100, 0b00000000 },
    { 0b00001100, 0b00011100, 0b00101100, 0b01001100,   // 4
      0b01111110, 0b00001100, 0b00001100, 0b00000000 },
    { 0b01111110, 0b01100000, 0b01111100, 0b00000110,   // 5
      0b00000110, 0b01100110, 0b00111100, 0b00000000 },
    { 0b00111100, 0b01100110, 0b01100000, 0b01111100,   // 6
      0b01100110, 0b01100110, 0b00111100, 0b00000000 },
    { 0b01111110, 0b00000110, 0b00001100, 0b00011000,   // 7
      0b00110000, 0b00110000, 0b00110000, 0b00000000 },
    { 0b00111100, 0b01100110, 0b01100110, 0b00111100,   // 8
      0b01100110, 0b01100110, 0b00111100, 0b00000000 },
    { 0b00111100, 0b01100110, 0b01100110, 0b00111110,   // 9
      0b00000110, 0b01100110, 0b00111100, 0b00000000 },
    { 0b00111100, 0b00111100, 0b00111100, 0b00111100,   // 10 - stop hand
      0b00111100, 0b00111100, 0b00000000, 0b00011000 },
    { 0b00011000, 0b00111100, 0b00011000, 0b01111110,   // 11 - walking person
      0b00011000, 0b00100100, 0b01000010, 0b00000000 }
};

// ============================================================================
// FUNCTION PROTOTYPES - TRAFFIC SIGNAL
// ============================================================================
void System_init(void);
void GPIO_init(void);
void Timer_init(void);
void shiftOut32bits(uint8_t *data);
OperatingMode checkModeButtons(void);
void handleModeChange(OperatingMode newMode);
void checkPedButtons(void);
void triggerPedWalk(TrafficState state);

// ============================================================================
// FUNCTION PROTOTYPES - PEDESTRIAN MATRICES
// ============================================================================
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

// ============================================================================
// MAIN FUNCTION
// ============================================================================
int main(void) {
    OperatingMode requestedMode;
    bool ledsNeedUpdate;
    int i;

    WDTCTL = WDTPW | WDTHOLD;

    // MUST unlock GPIO before any peripheral initialization
    PM5CTL0 &= ~LOCKLPM5;

    System_init();
    GPIO_init();
    Timer_init();
    matrixPinInit();
    ledMatrixInit();
    Timer1_init();

    initLEDState(&currentLEDs);
    setAllRed(&currentLEDs);
    shiftOut32bits(currentLEDs.byte);

    // Initialize all pedestrian signals to HAND (stop)
    for (i = 0; i < NUM_DEVICES; i++) {
        state_walking[i]     = STATE_HAND;
        pedWalkRequest[i]    = false;
        walk_counter[i]      = 0;
        walk_display_time[i] = 0;
    }

    // Display initial stop hand on all matrices
    displayPedState();

    __enable_interrupt();

    currentState = STATE_NS_GREEN;
    currentMode  = MODE_DAYTIME;
    stateTimer   = getStateDuration(currentState);

    // Execute initial state
    executeState(&currentLEDs, currentState);
    shiftOut32bits(currentLEDs.byte);

    // Main loop
    while (1) {
        ledsNeedUpdate = false;

        // --- Mode button check ---
        requestedMode = checkModeButtons();
        if (requestedMode != currentMode) {
            handleModeChange(requestedMode);
            ledsNeedUpdate = true;
        }

        // --- Pedestrian button check ---
        // Gated: only registers during the corresponding vehicle green phase
        checkPedButtons();

        // --- State timer expiry ---
        if (stateExpired) {
            stateExpired = false;

            currentState = getNextState(currentState, currentMode);

            // Safety wraparound checks
            if (currentMode == MODE_DAYTIME) {
                if (currentState > STATE_RETURN_TO_START) {
                    currentState = STATE_NS_GREEN;
                }
            }
            else if (currentMode == MODE_HIGH_TRAFFIC) {
                if (currentState < STATE_N_PRIORITY_START ||
                    currentState > STATE_RETURN_HT) {
                    currentState = STATE_N_PRIORITY_START;
                }
            }
            else if (currentMode == MODE_EMERGENCY) {
                currentState = STATE_EMERGENCY_HOLD;
            }
            // Night mode: getNextState handles toggle correctly

            stateTimer = getStateDuration(currentState);

            // Auto-trigger pedestrian walk on vehicle green transitions
            triggerPedWalk(currentState);

            ledsNeedUpdate = true;
        }

        // Only shift out to LEDs when state actually changed
        if (ledsNeedUpdate) {
            executeState(&currentLEDs, currentState);
            shiftOut32bits(currentLEDs.byte);
        }

        // --- Pedestrian matrix update (1 second tick) ---
        if (tick_1s) {
            tick_1s = 0;

            // Service any pending button-triggered walk requests
            for (i = 0; i < NUM_DEVICES; i++) {
                if (pedWalkRequest[i]) {
                    start_walk(i, WALK_TIME_NS);
                    pedWalkRequest[i] = false;
                }
            }

            updatePedStateMachine();
            displayPedState();
        }

        __bis_SR_register(LPM0_bits + GIE);
        __no_operation();
    }
}

// ============================================================================
// PEDESTRIAN BUTTON HANDLER
// Button press only registers when the corresponding vehicle phase is green
// Prevents pedestrians from walking into an active red phase
// ============================================================================
void checkPedButtons(void) {
    uint8_t p3btn = P3IN;
    uint8_t p4btn = P4IN;

    // North - P3.3 - valid during N-S green in Daytime and HT combined green
    if (!(p3btn & BTN_PED_NORTH)) {
        if (currentState == STATE_NS_GREEN      ||
            currentState == STATE_NS_BOTH_GREEN) {
            pedWalkRequest[PED_NORTH] = true;
        }
    }

    // South - P4.0 - valid during N-S green in Daytime and HT combined green
    if (!(p4btn & BTN_PED_SOUTH)) {
        if (currentState == STATE_NS_GREEN      ||
            currentState == STATE_NS_BOTH_GREEN) {
            pedWalkRequest[PED_SOUTH] = true;
        }
    }

    // East - P4.1 - valid during East split phase in both modes
    if (!(p4btn & BTN_PED_EAST)) {
        if (currentState == STATE_E_THRU_GREEN    ||
            currentState == STATE_E_THRU_GREEN_HT) {
            pedWalkRequest[PED_EAST] = true;
        }
    }

    // West - P3.6 - valid during West split phase in both modes
    if (!(p3btn & BTN_PED_WEST)) {
        if (currentState == STATE_W_THRU_GREEN    ||
            currentState == STATE_W_THRU_GREEN_HT) {
            pedWalkRequest[PED_WEST] = true;
        }
    }
}

// ============================================================================
// AUTO PEDESTRIAN TRIGGER
// Automatically starts walk cycle when vehicle phase transitions to green
// Called every time state changes - only fires on green states
// ============================================================================
void triggerPedWalk(TrafficState state) {
    switch (state) {
        case STATE_NS_GREEN:
            start_walk(PED_NORTH, WALK_TIME_NS);
            start_walk(PED_SOUTH, WALK_TIME_NS);
            break;

        case STATE_W_THRU_GREEN:
            start_walk(PED_WEST, WALK_TIME_EW);
            break;

        case STATE_E_THRU_GREEN:
            start_walk(PED_EAST, WALK_TIME_EW);
            break;

        case STATE_NS_BOTH_GREEN:
            start_walk(PED_NORTH, WALK_TIME_NS);
            start_walk(PED_SOUTH, WALK_TIME_NS);
            break;

        case STATE_W_THRU_GREEN_HT:
            start_walk(PED_WEST, WALK_TIME_EW);
            break;

        case STATE_E_THRU_GREEN_HT:
            start_walk(PED_EAST, WALK_TIME_EW);
            break;

        default:
            break;
    }
}

// ============================================================================
// PEDESTRIAN STATE MACHINE UPDATE (called every 1 second)
// ============================================================================
void updatePedStateMachine(void) {
    int i;
    for (i = 0; i < NUM_DEVICES; i++) {
        switch (state_walking[i]) {
            case STATE_WALK:
                if (walk_display_time[i] > 0) {
                    walk_display_time[i]--;
                }
                if (walk_display_time[i] == 0) {
                    state_walking[i] = STATE_COUNTDOWN;
                    walk_counter[i]  = 9;
                }
                break;

            case STATE_COUNTDOWN:
                if (walk_counter[i] > 0) {
                    walk_counter[i]--;
                } else {
                    state_walking[i] = STATE_HAND;
                }
                break;

            default:
                break;
        }
    }
}

// ============================================================================
// PEDESTRIAN DISPLAY UPDATE
// ============================================================================
void displayPedState(void) {
    uint8_t display_all[NUM_DEVICES];
    int i;
    for (i = 0; i < NUM_DEVICES; i++) {
        switch (state_walking[i]) {
            case STATE_HAND:      display_all[i] = 10;                break;
            case STATE_WALK:      display_all[i] = 11;                break;
            case STATE_COUNTDOWN: display_all[i] = walk_counter[i];   break;
            default:              display_all[i] = 10;                break;
        }
    }
    sendMatrixImage(display_all);
}

// walkTime: seconds to show walking person icon before countdown
void start_walk(uint8_t device, uint8_t walkTime) {
    if (state_walking[device] == STATE_HAND) {
        state_walking[device]     = STATE_WALK;
        walk_display_time[device] = walkTime;
    }
}

// ============================================================================
// INITIALIZATION
// ============================================================================

void System_init(void) {
    // 1 MHz default DCO - no changes needed for MSP430FR6989
}

void GPIO_init(void) {
    // Shift register outputs (P2)
    P2DIR |=  (DATA_PIN | SHIFT_CLK_PIN | LATCH_CLK_PIN);
    P2OUT &= ~(DATA_PIN | SHIFT_CLK_PIN | LATCH_CLK_PIN);

    // P3: mode buttons + North and West pedestrian buttons
    P3DIR &= ~(BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT |
               BTN_PED_NORTH | BTN_PED_WEST);
    P3REN |=  (BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT |
               BTN_PED_NORTH | BTN_PED_WEST);
    P3OUT |=  (BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT |
               BTN_PED_NORTH | BTN_PED_WEST);

    // P4.0 and P4.1: South and East pedestrian buttons
    // P4.2 is reserved for matrix data line - do not touch
    P4DIR &= ~(BTN_PED_SOUTH | BTN_PED_EAST);
    P4REN |=  (BTN_PED_SOUTH | BTN_PED_EAST);
    P4OUT |=  (BTN_PED_SOUTH | BTN_PED_EAST);

    // Mode buttons get interrupt-driven response
    // Pedestrian buttons are polled in main loop - gated by vehicle phase
    P3IES |=  (BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT);
    P3IE  |=  (BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT);
    P3IFG &= ~(BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT);
}

void Timer_init(void) {
    // Timer_A0: 1ms tick for traffic state machine
    // SMCLK (1MHz) / 8 = 125kHz, CCR0=125 -> 1kHz -> 1ms
    TA0CTL   = TASSEL__SMCLK | MC__UP | ID__8;
    TA0CCR0  = 125;
    TA0CCTL0 = CCIE;
}

void Timer1_init(void) {
    // Timer_A1: 1 second tick for pedestrian countdown
    // ACLK (32.768kHz), CCR0=32767 -> 1Hz -> 1s
    TA1CTL   = TASSEL_1 | MC_1 | TACLR;
    TA1CCR0  = 32768 - 1;
    TA1CCTL0 = CCIE;
}

// ============================================================================
// MODE CONTROL
// ============================================================================

OperatingMode checkModeButtons(void) {
    uint8_t buttonState = P3IN;

    if (!(buttonState & BTN_MODE_NIGHT))             return MODE_NIGHT;
    else if (!(buttonState & BTN_MODE_HIGH_TRAFFIC)) return MODE_HIGH_TRAFFIC;
    else if (!(buttonState & BTN_MODE_DAYTIME))      return MODE_DAYTIME;

    return currentMode;
}

void handleModeChange(OperatingMode newMode) {
    currentMode = newMode;

    switch (newMode) {
        case MODE_DAYTIME:      currentState = STATE_NS_GREEN;          break;
        case MODE_HIGH_TRAFFIC: currentState = STATE_N_PRIORITY_START;  break;
        case MODE_NIGHT:        currentState = STATE_NIGHT_FLASH_ON;    break;
        case MODE_EMERGENCY:    currentState = STATE_EMERGENCY_ALL_RED; break;
    }

    stateTimer   = getStateDuration(currentState);
    stateExpired = false;
}

// ============================================================================
// MATRIX PIN HELPERS
// ============================================================================

void matrixPinInit(void) {
    // Latch -> P9.6
    P9OUT |=  BIT6; P9DIR |= BIT6;
    P9SEL1 &= ~BIT6; P9SEL0 &= ~BIT6;
    // Data -> P4.2
    P4OUT &= ~BIT2; P4DIR |= BIT2;
    P4SEL1 &= ~BIT2; P4SEL0 &= ~BIT2;
    // Clock -> P1.5
    P1OUT &= ~BIT5; P1DIR |= BIT5;
    P1SEL1 &= ~BIT5; P1SEL0 &= ~BIT5;
}

void matrixClockLow(void)  { P1OUT &= ~BIT5; }
void matrixClockHigh(void) { P1OUT |=  BIT5; }
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
        __delay_cycles(500);
        matrixClockHigh();
        __delay_cycles(500);
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
    sendMatrixPacket(0x0C, data);   // Shutdown register - off
    sendMatrixPacket(0x0F, data);   // Display test off
    sendMatrixPacket(0x09, data);   // No BCD decode

    for (i = 0; i < NUM_DEVICES; i++) data[i] = 0x07;
    sendMatrixPacket(0x0B, data);   // Scan limit - all 8 rows

    for (i = 0; i < NUM_DEVICES; i++) data[i] = 0x0F;
    sendMatrixPacket(0x0A, data);   // Intensity - max brightness

    for (i = 0; i < NUM_DEVICES; i++) data[i] = 0x01;
    sendMatrixPacket(0x0C, data);   // Shutdown register - normal operation
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
// SHIFT REGISTER CONTROL (Traffic signal LEDs)
// ============================================================================

void shiftOut32bits(uint8_t *data) {
    int16_t byte_idx, bit_idx;

    P2OUT &= ~LATCH_CLK_PIN;
    __delay_cycles(1);

    for (byte_idx = 3; byte_idx >= 0; byte_idx--) {
        for (bit_idx = 7; bit_idx >= 0; bit_idx--) {
            if (data[byte_idx] & (1 << bit_idx)) {
                P2OUT |=  DATA_PIN;
            } else {
                P2OUT &= ~DATA_PIN;
            }
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
// INTERRUPT SERVICE ROUTINES
// ============================================================================

// Timer_A0 - 1ms tick (traffic signal state machine)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR(void) {
    systemTick++;
    if (stateTimer > 0) {
        stateTimer--;
        if (stateTimer == 0) {
            stateExpired = true;
            __bic_SR_register_on_exit(LPM0_bits);
        }
    }
}

// Timer_A1 - 1 second tick (pedestrian countdown)
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A1_ISR(void) {
    tick_1s = 1;
    __bic_SR_register_on_exit(LPM0_bits);
}

// Port 3 - Mode button interrupts (pedestrian buttons are polled)
#pragma vector=PORT3_VECTOR
__interrupt void Port_3_ISR(void) {
    if (P3IFG & (BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT)) {
        P3IFG &= ~(BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT);
        __bic_SR_register_on_exit(LPM0_bits);
    }
    P3IFG = 0;
}
