#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include "traffic_states.h"

// ============================================================================
// PIN DEFINITIONS
// ============================================================================

// Shift register control (P2)
#define DATA_PIN        BIT0    // P2.0
#define SHIFT_CLK_PIN   BIT1    // P2.1
#define LATCH_CLK_PIN   BIT2    // P2.2

// Mode selection buttons (P3) - Active LOW with pull-ups
#define BTN_MODE_DAYTIME      BIT0    // P3.0 - Normal/Daytime mode
#define BTN_MODE_HIGH_TRAFFIC BIT1    // P3.1 - High Traffic mode
#define BTN_MODE_NIGHT        BIT2    // P3.2 - Night mode

// Pedestrian buttons (P3) - Active LOW with pull-ups
#define BTN_PED_NORTH   BIT3    // P3.3
#define BTN_PED_SOUTH   BIT4    // P3.4
#define BTN_PED_EAST    BIT5    // P3.5
#define BTN_PED_WEST    BIT6    // P3.6

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

LEDState currentLEDs;
TrafficState currentState = STATE_NS_GREEN;
OperatingMode currentMode = MODE_DAYTIME;
volatile uint32_t systemTick = 0;
volatile uint32_t stateTimer = 0;
volatile bool stateExpired = false;

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================

void System_init(void);
void GPIO_init(void);
void Timer_init(void);
void shiftOut32bits(uint8_t *data);
OperatingMode checkModeButtons(void);
void handleModeChange(OperatingMode newMode);

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;

    // Initialize system
    System_init();
    GPIO_init();
    Timer_init();

    // Disable GPIO power-on default high-impedance mode
    PM5CTL0 &= ~LOCKLPM5;

    // Initialize LED state
    initLEDState(&currentLEDs);

    // Start with all red
    setAllRed(&currentLEDs);
    shiftOut32bits(currentLEDs.byte);

    // Enable global interrupts
    __enable_interrupt();

    // Initial state setup
    currentState = STATE_NS_GREEN;
    currentMode = MODE_DAYTIME;
    stateTimer = getStateDuration(currentState);

    // Main loop
    while(1) {
        // Check for mode button presses
        OperatingMode requestedMode = checkModeButtons();

        // Handle mode change if requested
        if (requestedMode != currentMode) {
            handleModeChange(requestedMode);
        }

        // Check if state timer expired
        if (stateExpired) {
            stateExpired = false;

            // Move to next state
            currentState = getNextState(currentState, currentMode);

            // Handle state wraparound based on mode
            if (currentMode == MODE_DAYTIME) {
                if (currentState > STATE_RETURN_TO_START) {
                    currentState = STATE_NS_GREEN;
                }
            }
            else if (currentMode == MODE_HIGH_TRAFFIC) {
                if (currentState < STATE_N_PRIORITY_START || currentState > STATE_RETURN_HT) {
                    currentState = STATE_N_PRIORITY_START;
                }
            }
            else if (currentMode == MODE_NIGHT) {
                // Toggle between flash on/off
                if (currentState == STATE_NIGHT_FLASH_ON) {
                    currentState = STATE_NIGHT_FLASH_OFF;
                } else {
                    currentState = STATE_NIGHT_FLASH_ON;
                }
            }
            else if (currentMode == MODE_EMERGENCY) {
                currentState = STATE_EMERGENCY_HOLD;
            }

            // Reload timer for new state
            stateTimer = getStateDuration(currentState);
        }

        // Execute current state and update LEDs
        executeState(&currentLEDs, currentState);
        shiftOut32bits(currentLEDs.byte);

        // Enter low power mode until next interrupt
        __bis_SR_register(LPM0_bits + GIE);
        __no_operation();
    }
}

// ============================================================================
// INITIALIZATION FUNCTIONS
// ============================================================================

void System_init(void) {
    // Configure clock to 1 MHz (default MCLK from DCO)
    // For MSP430FR6989, default is already ~1MHz
}

void GPIO_init(void) {
    // Configure P2.0, P2.1, P2.2 as outputs (shift register control)
    P2DIR |= (DATA_PIN | SHIFT_CLK_PIN | LATCH_CLK_PIN);
    P2OUT &= ~(DATA_PIN | SHIFT_CLK_PIN | LATCH_CLK_PIN);  // Start low

    // Configure P3.0-P3.6 as inputs with pull-ups (buttons)
    P3DIR &= ~(BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT |
               BTN_PED_NORTH | BTN_PED_SOUTH | BTN_PED_EAST | BTN_PED_WEST);

    P3REN |= (BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT |
              BTN_PED_NORTH | BTN_PED_SOUTH | BTN_PED_EAST | BTN_PED_WEST);

    P3OUT |= (BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT |
              BTN_PED_NORTH | BTN_PED_SOUTH | BTN_PED_EAST | BTN_PED_WEST);  // Pull-up

    // Enable interrupts on mode buttons (for immediate response)
    P3IES |= (BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT);  // High-to-low edge
    P3IE |= (BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT);   // Enable interrupts
    P3IFG &= ~(BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT); // Clear flags
}

void Timer_init(void) {
    // Configure Timer_A0 for 1ms interrupts
    // Using SMCLK (1 MHz) / 8 = 125 kHz
    TA0CTL = TASSEL__SMCLK | MC__UP | ID__8;  // SMCLK, Up mode, divide by 8
    TA0CCR0 = 125;  // 125 kHz / 125 = 1 kHz (1ms period)
    TA0CCTL0 = CCIE;  // Enable interrupt
}

// ============================================================================
// MODE CONTROL FUNCTIONS
// ============================================================================

OperatingMode checkModeButtons(void) {
    // Read button states (active LOW)
    uint8_t buttonState = P3IN;

    // Check which mode button is pressed (LOW = pressed)
    if (!(buttonState & BTN_MODE_NIGHT)) {
        return MODE_NIGHT;
    }
    else if (!(buttonState & BTN_MODE_HIGH_TRAFFIC)) {
        return MODE_HIGH_TRAFFIC;
    }
    else if (!(buttonState & BTN_MODE_DAYTIME)) {
        return MODE_DAYTIME;
    }

    // No button pressed, return current mode
    return currentMode;
}

void handleModeChange(OperatingMode newMode) {
    currentMode = newMode;

    // Transition to appropriate starting state for new mode
    switch (newMode) {
        case MODE_DAYTIME:
            currentState = STATE_NS_GREEN;
            break;

        case MODE_HIGH_TRAFFIC:
            currentState = STATE_N_PRIORITY_START;
            break;

        case MODE_NIGHT:
            currentState = STATE_NIGHT_FLASH_ON;
            break;

        case MODE_EMERGENCY:
            currentState = STATE_EMERGENCY_ALL_RED;
            break;
    }

    // Reset state timer for new state
    stateTimer = getStateDuration(currentState);
    stateExpired = false;
}

// ============================================================================
// SHIFT REGISTER CONTROL - FIXED FOR C89 COMPILER
// ============================================================================

void shiftOut32bits(uint8_t *data) {
    int16_t byte_idx, bit_idx;  // Declared as 16-bit to avoid ULP warning and infinite loop

    // Lower latch to prepare for new data
    P2OUT &= ~LATCH_CLK_PIN;
    __delay_cycles(1);

    // Shift out 32 bits (4 bytes), MSB first
    // Start from byte[3] (U4) down to byte[0] (U1)
    for (byte_idx = 3; byte_idx >= 0; byte_idx--) {
        for (bit_idx = 7; bit_idx >= 0; bit_idx--) {
            // Set data line
            if (data[byte_idx] & (1 << bit_idx)) {
                P2OUT |= DATA_PIN;
            } else {
                P2OUT &= ~DATA_PIN;
            }

            // Pulse shift clock
            P2OUT |= SHIFT_CLK_PIN;
            __delay_cycles(1);
            P2OUT &= ~SHIFT_CLK_PIN;
            __delay_cycles(1);
        }
    }

    // Raise latch to update all outputs simultaneously
    P2OUT |= LATCH_CLK_PIN;
    __delay_cycles(1);
    P2OUT &= ~LATCH_CLK_PIN;
}

// ============================================================================
// INTERRUPT SERVICE ROUTINES
// ============================================================================

// Timer A0 interrupt - 1ms tick
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0_ISR(void) {
    systemTick++;

    // Decrement state timer
    if (stateTimer > 0) {
        stateTimer--;
        if (stateTimer == 0) {
            stateExpired = true;
            __bic_SR_register_on_exit(LPM0_bits);  // Wake up main loop
        }
    }
}

// Port 3 interrupt - Mode button presses
#pragma vector=PORT3_VECTOR
__interrupt void Port_3_ISR(void) {
    // Check which button caused interrupt
    if (P3IFG & (BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT)) {
        // Debounce by clearing flag and waking up main loop
        // Main loop will check button state
        P3IFG &= ~(BTN_MODE_DAYTIME | BTN_MODE_HIGH_TRAFFIC | BTN_MODE_NIGHT);
        __bic_SR_register_on_exit(LPM0_bits);  // Wake up main loop
    }

    // Clear all Port 3 interrupt flags
    P3IFG = 0;
}
