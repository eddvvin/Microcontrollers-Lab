#include <msp430.h>
#include <msp430fr6989.h>
#include <stdint.h>

#define NUM_of_digit 12
#define LSBFIRST 0
#define MSBFIRST 1
#define NUM_DEVICES 4

// ---------- Buzzer config ----------
// Active buzzer on P9.2 (J1 pin 2). + to P9.2, - to GND.
#define BUZZER_BIT       BIT2
#define BUZZER_ON()      (P9OUT |=  BUZZER_BIT)
#define BUZZER_OFF()     (P9OUT &= ~BUZZER_BIT)

// Beep timing (Timer_A0 on ACLK = 32768 Hz)
#define BEEP_ON_TICKS       1311U   // ~40 ms
#define GAP_SLOW_TICKS     16384U   // ~500 ms
#define GAP_FAST_TICKS       820U   // ~25 ms

typedef enum {
    STATE_HAND,
    STATE_WALK,
    STATE_COUNTDOWN
} State_walking;

volatile State_walking state_walking[NUM_DEVICES];
volatile uint8_t walk_counter[NUM_DEVICES];
volatile uint8_t walk_display_time[NUM_DEVICES];
volatile uint8_t tick_1s = 0;
volatile uint8_t buzzer_phase_on = 0;

void pinInit(void){
    //Latch Pin -> P9.6
    P9OUT |= BIT6;
    P9DIR |= BIT6;
    P9SEL1 &= ~BIT6;
    P9SEL0 &= ~BIT6;
    //Data Pin -> P4.2
    P4OUT &= ~BIT2;
    P4DIR |= BIT2;
    P4SEL1 &= ~BIT2;
    P4SEL0 &= ~BIT2;
    //Clock Pin -> P1.5
    P1OUT &= ~BIT5;
    P1DIR |= BIT5;
    P1SEL1 &= ~BIT5;
    P1SEL0 &= ~BIT5;
    //Buzzer -> P9.2
    P9OUT &= ~BUZZER_BIT;
    P9DIR |=  BUZZER_BIT;
    P9SEL1 &= ~BUZZER_BIT;
    P9SEL0 &= ~BUZZER_BIT;
}

void clockpinLow(void)  { P1OUT &= ~BIT5; }
void clockpinHigh(void) { P1OUT |=  BIT5; }
void latchpinLow(void)  { P9OUT &= ~BIT6; }
void latchpinHigh(void) { P9OUT |=  BIT6; }
void datapinLow(void)   { P4OUT &= ~BIT2; }
void datapinHigh(void)  { P4OUT |=  BIT2; }

void shiftOut(uint8_t bitOrder, uint16_t value){
    uint16_t i;
    for(i = 0; i < 16; i++){
        if(bitOrder == LSBFIRST){
            if(value & 0x0001) datapinHigh(); else datapinLow();
            value >>= 1;
        } else {
            if(value & 0x8000) datapinHigh(); else datapinLow();
            value <<= 1;
        }
        __delay_cycles(500);
        clockpinHigh();
        __delay_cycles(500);
        clockpinLow();
    }
}

void sendoutspiPacket_all(uint8_t address, uint8_t data[NUM_DEVICES]){
    int i;
    latchpinLow();
    for(i = NUM_DEVICES - 1; i >= 0; i--){
        uint16_t packet = (address << 8) | data[i];
        shiftOut(MSBFIRST, packet);
    }
    latchpinHigh();
}

void ledmatrixInit(){
    uint8_t data[NUM_DEVICES];
    int i;
    for(i=0;i<NUM_DEVICES;i++) data[i]=0x00;
    sendoutspiPacket_all(0x0C, data);
    sendoutspiPacket_all(0x0F, data);
    sendoutspiPacket_all(0x09, data);
    for(i=0;i<NUM_DEVICES;i++) data[i]=0x07;
    sendoutspiPacket_all(0x0B, data);
    for(i=0;i<NUM_DEVICES;i++) data[i]=0x0F;
    sendoutspiPacket_all(0x0A, data);
    for(i=0;i<NUM_DEVICES;i++) data[i]=0x01;
    sendoutspiPacket_all(0x0C, data);
}

const uint8_t images[NUM_of_digit][8] = {
    {0b00111100,0b01100110,0b01101110,0b01110110,0b01100110,0b01100110,0b00111100,0b00000000}, // 0
    {0b00011000,0b00111000,0b00011000,0b00011000,0b00011000,0b00011000,0b00111100,0b00000000}, // 1
    {0b00111100,0b01100110,0b00000110,0b00001100,0b00011000,0b00110000,0b01111110,0b00000000}, // 2
    {0b00111100,0b01100110,0b00000110,0b00011100,0b00000110,0b01100110,0b00111100,0b00000000}, // 3
    {0b00001100,0b00011100,0b00101100,0b01001100,0b01111110,0b00001100,0b00001100,0b00000000}, // 4
    {0b01111110,0b01100000,0b01111100,0b00000110,0b00000110,0b01100110,0b00111100,0b00000000}, // 5
    {0b00111100,0b01100110,0b01100000,0b01111100,0b01100110,0b01100110,0b00111100,0b00000000}, // 6
    {0b01111110,0b00000110,0b00001100,0b00011000,0b00110000,0b00110000,0b00110000,0b00000000}, // 7
    {0b00111100,0b01100110,0b01100110,0b00111100,0b01100110,0b01100110,0b00111100,0b00000000}, // 8
    {0b00111100,0b01100110,0b01100110,0b00111110,0b00000110,0b01100110,0b00111100,0b00000000}, // 9
    {0b00111100,0b00111100,0b00111100,0b00111100,0b00111100,0b00111100,0b00000000,0b00011000}, // 10 - Hand
    {0b00011000,0b00111100,0b00011000,0b01111110,0b00011000,0b00100100,0b01000010,0b00000000}  // 11 - Walking
};

void sendimageSPI_multi(uint8_t digits[NUM_DEVICES]){
    uint8_t row, i;
    uint8_t row_data[NUM_DEVICES];
    for(row = 0; row < 8; row++){
        for(i = 0; i < NUM_DEVICES; i++){
            row_data[i] = images[digits[i]][row];
        }
        sendoutspiPacket_all(row+1, row_data);
    }
}

void timer1_init(void){
    TA1CTL = TASSEL_1 | MC_1 | TACLR;
    TA1CCR0 = 32768 - 1;
    TA1CCTL0 = CCIE;
}

void timer0_init(void){
    TA0CTL = TASSEL_1 | MC_0 | TACLR;
    TA0CCR0 = GAP_SLOW_TICKS;
    TA0CCTL0 = CCIE;
}

static uint8_t compute_buzzer_state(uint16_t *gap_ticks_out){
    int i;
    uint8_t any_active = 0;
    uint8_t any_walk = 0;
    uint8_t any_countdown = 0;
    uint8_t min_counter = 9;

    for(i = 0; i < NUM_DEVICES; i++){
        if(state_walking[i] == STATE_WALK){
            any_active = 1;
            any_walk = 1;
        } else if(state_walking[i] == STATE_COUNTDOWN){
            any_active = 1;
            any_countdown = 1;
            if(walk_counter[i] < min_counter) min_counter = walk_counter[i];
        }
    }

    if(!any_active) return 0;

    if(any_walk && !any_countdown){
        *gap_ticks_out = GAP_SLOW_TICKS;
        return 1;
    }

    uint32_t span = (uint32_t)(GAP_SLOW_TICKS - GAP_FAST_TICKS);
    uint16_t gap = (uint16_t)(GAP_FAST_TICKS + (span * min_counter) / 9U);
    *gap_ticks_out = gap;
    return 1;
}

static void update_buzzer(void){
    uint16_t gap;
    uint8_t active = compute_buzzer_state(&gap);

    if(!active){
        TA0CTL &= ~MC_3;
        TA0CTL |=  TACLR;
        BUZZER_OFF();
        buzzer_phase_on = 0;
        return;
    }

    if((TA0CTL & MC_3) == 0){
        buzzer_phase_on = 0;
        BUZZER_OFF();
        TA0CCR0 = gap;
        TA0CTL = TASSEL_1 | MC_1 | TACLR;
        TA0CCTL0 = CCIE;
    }
}

void update_state_machine(void){
    int i;
    for(i = 0; i < NUM_DEVICES; i++){
        switch(state_walking[i]){
        case STATE_WALK:
            if(walk_display_time[i] > 0) walk_display_time[i]--;
            if(walk_display_time[i] == 0){
                state_walking[i] = STATE_COUNTDOWN;
                walk_counter[i] = 9;
            }
            break;
        case STATE_COUNTDOWN:
            if(walk_counter[i] > 0){
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

void display_state(void){
    uint8_t display_all[NUM_DEVICES];
    int i;
    for(i = 0; i < NUM_DEVICES; i++){
        switch(state_walking[i]){
        case STATE_HAND:      display_all[i] = 10; break;
        case STATE_WALK:      display_all[i] = 11; break;
        case STATE_COUNTDOWN: display_all[i] = walk_counter[i]; break;
        }
    }
    sendimageSPI_multi(display_all);
}

void start_walk(uint8_t device){
    if(state_walking[device] == STATE_HAND){
        state_walking[device] = STATE_WALK;
        walk_display_time[device] = 5;
    }
}

int main(void){
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;       // unlock GPIO (FR6989 boots locked)

    pinInit();
    ledmatrixInit();
    timer1_init();
    timer0_init();

    __enable_interrupt();

    int i;
    for(i = 0; i < NUM_DEVICES; i++){
        state_walking[i] = STATE_HAND;
    }

    start_walk(0);
    start_walk(1);
    start_walk(2);

    while(1){
        if(tick_1s){
            tick_1s = 0;
            update_state_machine();
            display_state();
            update_buzzer();
        }
    }
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void){
    tick_1s = 1;
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void){
    if(buzzer_phase_on){
        BUZZER_OFF();
        buzzer_phase_on = 0;
        uint16_t gap;
        if(compute_buzzer_state(&gap)){
            TA0CCR0 = gap;
        } else {
            TA0CTL &= ~MC_3;
            TA0CTL |= TACLR;
        }
    } else {
        uint16_t gap;
        if(compute_buzzer_state(&gap)){
            BUZZER_ON();
            buzzer_phase_on = 1;
            TA0CCR0 = BEEP_ON_TICKS;
        } else {
            TA0CTL &= ~MC_3;
            TA0CTL |= TACLR;
            BUZZER_OFF();
        }
    }
}
