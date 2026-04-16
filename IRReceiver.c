#include <driverlib.h>
#include <msp430.h>
#include <msp430fr6989.h>
#include <stdint.h>

#define CaptureBufferSize 68
#define totalTimings 67
#define UpperLeader 9500
#define LowerLeader 8500
#define NUM_CHANNELS 4

/*
Pin Mapping:
P1.5 -> TA0.CCI0A -> ir[0]
P1.6 -> TA0.CCI1A -> ir[1]
P1.7 -> TA0.CCI2A -> ir[2]
P1.3 -> TA1.CCI1A -> ir[3]
*/

typedef struct {
    volatile uint16_t buffer[CaptureBufferSize];
    volatile uint16_t lastCapture;
    volatile uint8_t frameComplete;
    volatile uint8_t captureIndex;
    volatile uint8_t leaderDetected;
} IR_Channel;

IR_Channel ir[NUM_CHANNELS];
volatile uint32_t result[NUM_CHANNELS];

uint32_t decodeNEC(volatile uint16_t *buffer)
{
    uint32_t value = 0;
    int index = 2; // skip leader
    for(int bit = 0; bit < 32; bit++)
    {
        uint16_t space = buffer[index + 1];

        if(space > 1500)
        {
            value |= (1UL << bit);
        }

        index += 2;
    }

    return value;
}

void handleCapture(IR_Channel *ch, uint16_t currentcapture)
{
    uint16_t duration = currentcapture - ch->lastCapture;
    ch->lastCapture = currentcapture;

    // Leader detection
    if(duration > LowerLeader && duration < UpperLeader){
        ch->leaderDetected = 1;
        ch->captureIndex = 0;
    }

    if(ch->leaderDetected){
        if(ch->captureIndex < CaptureBufferSize){
            ch->buffer[ch->captureIndex++] = duration;
        }

        if(ch->captureIndex == totalTimings){
            ch->frameComplete = 1;
            ch->captureIndex = 0;
            ch->leaderDetected = 0;
        }
    }
}

void initTimerA0Capture(void)
{
    Timer_A_initCaptureModeParam capture = {0};

    capture.captureMode = TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE;
    capture.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    capture.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    capture.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    capture.captureOutputMode = TIMER_A_OUTPUTMODE_OUTBITVALUE;

    // CCR0
    capture.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0;
    Timer_A_initCaptureMode(TIMER_A0_BASE, &capture);

    // CCR1
    capture.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    Timer_A_initCaptureMode(TIMER_A0_BASE, &capture);

    // CCR2
    capture.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;
    Timer_A_initCaptureMode(TIMER_A0_BASE, &capture);
}

void initTimerA1Capture(void)
{
    Timer_A_initCaptureModeParam capture = {0};

    capture.captureMode = TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE;
    capture.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    capture.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    capture.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    capture.captureOutputMode = TIMER_A_OUTPUTMODE_OUTBITVALUE;

    //TA1.1 = CCR1 = 4th IR receiver
    capture.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    Timer_A_initCaptureMode(TIMER_A1_BASE, &capture);
}

void initTimerAContinuousMode(void)
{
    Timer_A_initContinuousModeParam continuousmode = {0};

    continuousmode.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    continuousmode.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    continuousmode.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    continuousmode.timerClear = TIMER_A_DO_CLEAR;
    continuousmode.startTimer = true;

    Timer_A_initContinuousMode(TIMER_A0_BASE, &continuousmode);
    Timer_A_initContinuousMode(TIMER_A1_BASE, &continuousmode);
}

void initPins(void)
{
    // P1.5 -> CCR0
    P1DIR &= ~BIT5;
    P1SEL0 |= BIT5;
    P1SEL1 |= BIT5;

    // P1.6 -> CCR1
    P1DIR &= ~BIT6;
    P1SEL0 |= BIT6;
    P1SEL1 |= BIT6;

    // P1.7 -> CCR2
    P1DIR &= ~BIT7;
    P1SEL0 |= BIT7;
    P1SEL1 |= BIT7;

    //P3.3 -> CCR
    P3DIR &= ~BIT3;
    P3SEL0 &= ~BIT3;
    P3SEL1 |= BIT3;
}

int main(void)
{
    WDT_A_hold(__MSP430_BASEADDRESS_WDT_A__);
    PMM_unlockLPM5();

    // Initialize struct values
    for(int i = 0; i < NUM_CHANNELS; i++){
        ir[i].lastCapture = 0;
        ir[i].frameComplete = 0;
        ir[i].captureIndex = 0;
        ir[i].leaderDetected = 0;
        result[i] = 0;
    }

    initPins();
    initTimerA0Capture();
    initTimerA1Capture();

    // Enable CCR interrupts
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE,
                                          TIMER_A_CAPTURECOMPARE_REGISTER_0);
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE,
                                          TIMER_A_CAPTURECOMPARE_REGISTER_1);
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE,
                                          TIMER_A_CAPTURECOMPARE_REGISTER_2);

    Timer_A_enableCaptureCompareInterrupt(TIMER_A1_BASE,
        TIMER_A_CAPTURECOMPARE_REGISTER_1);

    initTimerAContinuousMode();

    __enable_interrupt();

    while(1)
    {
        for(unsigned int i = 0; i < NUM_CHANNELS; i++){
            if(ir[i].frameComplete){
                result[i] = decodeNEC(ir[i].buffer);

                ir[i].frameComplete = 0;
                ir[i].leaderDetected = 0;
            }
        }
    }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_CCR0_ISR(void)
{
    uint16_t currentcapture =
        Timer_A_getCaptureCompareCount(TIMER_A0_BASE,
                                       TIMER_A_CAPTURECOMPARE_REGISTER_0);

    handleCapture(&ir[0], currentcapture);
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    switch(__even_in_range(TA0IV,4)){

        case 2: // CCR1
        {
            uint16_t currentcapture =
                Timer_A_getCaptureCompareCount(TIMER_A0_BASE,
                                               TIMER_A_CAPTURECOMPARE_REGISTER_1);

            handleCapture(&ir[1], currentcapture);
            break;
        }

        case 4: // CCR2
        {
            uint16_t currentcapture =
                Timer_A_getCaptureCompareCount(TIMER_A0_BASE,
                                               TIMER_A_CAPTURECOMPARE_REGISTER_2);

            handleCapture(&ir[2], currentcapture);
            break;
        }
    }
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)
{
    switch(__even_in_range(TA1IV,2)){

        case 2:
        {
            uint16_t current =
                Timer_A_getCaptureCompareCount(TIMER_A1_BASE,
                    TIMER_A_CAPTURECOMPARE_REGISTER_1);

            handleCapture(&ir[3], current);
            break;
        }
    }
}

/*
Commands on the controller and their corresponding hex values; Should be used to key into emergency state
0:0xE916FF00
1:0xF30CFF00
2:0xE718FF00
3:0xA15EFF00
4:0xF708FF00
5:0xE31CFF00
6:0xA55AFF00
7:0xBD42FF00
8:0xAD52FF00
9:0xB54AFF00
Power Button:
Vol+:0xB946FF00
Vol-:0xEA15FF00
Up Arrow:0xF609FF00
Down Arrow:0xF807FF00
Back Button:0xBB44FF00
Pause Button:0xBF40FF00
Fast Foward:0xBC43FF00
Func/Stop:0xB847FF00
EQ:0xE619FF00
ST/REPT:0xF20DFF00
*/