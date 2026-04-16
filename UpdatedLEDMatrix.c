#include <driverlib.h>
#include <msp430.h>
#include <msp430fr6989.h>
#include <stdio.h>

#define NUM_of_digit 12
#define LSBFIRST 0
#define MSBFIRST 1
#define NUM_DEVICES 4

typedef enum {
    STATE_HAND,
    STATE_WALK,
    STATE_COUNTDOWN
}State_walking;

volatile State_walking state_walking[NUM_DEVICES];
//Countdown value (9->0). Showing the numbers counting down towards 0 to signify u can no longer walk
volatile uint8_t walk_counter[NUM_DEVICES];
// how long walkingman icon is shown (seconds)
volatile uint8_t walk_display_time[NUM_DEVICES];
//Used for every second that has elapsed
volatile uint8_t tick_1s = 0;

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
}

void clockpinLow(void){
    P1OUT &= ~BIT5;
}

void clockpinHigh(void){
    P1OUT |= BIT5;
}

void latchpinLow(void){
    P9OUT &= ~BIT6;
}

void latchpinHigh(void){
    P9OUT |= BIT6;
}

void datapinLow(void){
    P4OUT &= ~BIT2;
}

void datapinHigh(void){
    P4OUT |= BIT2;
}

void shiftOut(uint8_t bitOrder, uint16_t value){
    uint16_t i;

    for(i=0;i<16;i++){
        if(bitOrder == LSBFIRST){
            if(value & 0b0000000000000001){
                datapinHigh();
            }
            else{
                datapinLow();
            }
            value >>= 1;
        }
        else{
            if(value & 0b1000000000000000){
                datapinHigh();
            }
            else{
                datapinLow();
            }
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

    // Send to last device first
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

//Test using a smiley face
const uint8_t images[NUM_of_digit][8] = {
       // 0
    {
        0b00111100,
        0b01100110,
        0b01101110,
        0b01110110,
        0b01100110,
        0b01100110,
        0b00111100,
        0b00000000
    },

    // 1
    {
        0b00011000,
        0b00111000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00011000,
        0b00111100,
        0b00000000
    },

    // 2
    {
        0b00111100,
        0b01100110,
        0b00000110,
        0b00001100,
        0b00011000,
        0b00110000,
        0b01111110,
        0b00000000
    },

    // 3
    {
        0b00111100,
        0b01100110,
        0b00000110,
        0b00011100,
        0b00000110,
        0b01100110,
        0b00111100,
        0b00000000
    },

    // 4
    {
        0b00001100,
        0b00011100,
        0b00101100,
        0b01001100,
        0b01111110,
        0b00001100,
        0b00001100,
        0b00000000
    },

    // 5
    {
        0b01111110,
        0b01100000,
        0b01111100,
        0b00000110,
        0b00000110,
        0b01100110,
        0b00111100,
        0b00000000
    },

    // 6
    {
        0b00111100,
        0b01100110,
        0b01100000,
        0b01111100,
        0b01100110,
        0b01100110,
        0b00111100,
        0b00000000
    },

    // 7
    {
        0b01111110,
        0b00000110,
        0b00001100,
        0b00011000,
        0b00110000,
        0b00110000,
        0b00110000,
        0b00000000
    },

    // 8
    {
        0b00111100,
        0b01100110,
        0b01100110,
        0b00111100,
        0b01100110,
        0b01100110,
        0b00111100,
        0b00000000
    },

    // 9
    {
        0b00111100,
        0b01100110,
        0b01100110,
        0b00111110,
        0b00000110,
        0b01100110,
        0b00111100,
        0b00000000
    },

    // 2 - Exclamation mark
    { 
        0b00111100,
        0b00111100, 
        0b00111100, 
        0b00111100,
        0b00111100, 
        0b00111100, 
        0b00000000, 
        0b00011000 
        },

    // 11 - Walking person
    {
        0b00011000,
        0b00111100,
        0b00011000,
        0b01111110,
        0b00011000,
        0b00100100,
        0b01000010,
        0b00000000
    }
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
//For initializing the proper clock that will be used to determine every second that has passed.
void timer1_init(void){
    // ACLK, up mode, interrupt enabled
    TA1CTL = TASSEL_1 | MC_1 | TACLR;
    TA1CCR0 = 32768 - 1; //Amount of time that would be a second using the ASMCLK
    TA1CCTL0 = CCIE;  // enable interrupt
}
//Used for determining what state we are in and updates timing counters
void update_state_machine(void){
    int i;

    for(i = 0; i < NUM_DEVICES; i++){

        switch(state_walking[i]){

        case STATE_WALK:
            if(walk_display_time[i] > 0){
                walk_display_time[i]--;
            }

            if(walk_display_time[i] == 0){
                state_walking[i] = STATE_COUNTDOWN;
                walk_counter[i] = 9;
            }
            break;

        case STATE_COUNTDOWN:
            if(walk_counter[i] > 0){
                walk_counter[i]--;
            }
            else{
                state_walking[i] = STATE_HAND;
            }
            break;

        default:
            break;
        }
    }
}
//Used to display the actual state that we are in. Sending image 10 is sending the hand/stop. Sending image 11 is the walking person. Sending image walkcounter displays the different numbers that are being counted down from
void display_state(void){
    uint8_t display_all[NUM_DEVICES];
    int i;

    for(i = 0; i < NUM_DEVICES; i++){

        switch(state_walking[i]){

        case STATE_HAND:
            display_all[i] = 10;
            break;

        case STATE_WALK:
            display_all[i] = 11;
            break;

        case STATE_COUNTDOWN:
            display_all[i] = walk_counter[i];
            break;
        }
    }

    sendimageSPI_multi(display_all);
}
//Used to activate the walking state.
void start_walk(uint8_t device){
    if(state_walking[device] == STATE_HAND){
        state_walking[device] = STATE_WALK;
        walk_display_time[device] = 5;
    }
}

int main(void){
    WDTCTL = WDTPW | WDTHOLD; //Used password and halted watchdog timer
    volatile uint32_t smclk = CS_getSMCLK();
    // Disable the GPIO power-on default high-impedance mode
    PMM_unlockLPM5();
    pinInit();
    ledmatrixInit();
    timer1_init();

    __enable_interrupt();
    //Want to start with not being able to walk first
    int i;

    for(i = 0; i < NUM_DEVICES; i++){
        state_walking[i] = STATE_HAND;
    }
    //Then will intialize walk.
    start_walk(0);
    start_walk(1);
    start_walk(2);
    while(1){       
        //Checking every second that has elapsed 
        if(tick_1s){
                tick_1s = 0;
        
                update_state_machine();
                display_state();
            }

    }

}


#pragma vector = TIMER1_A0_VECTOR
__interrupt void TIMER_A0_ISR(void){
    tick_1s = 1;//1 second has passed
}
