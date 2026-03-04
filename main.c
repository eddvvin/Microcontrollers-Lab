#include <driverlib.h>
#include <msp430.h>
#include <msp430fr6989.h>
#include <stdio.h>
#define CaptureBufferSize 68
#define totalTimings 67
#define UpperLeader 9500
#define LowerLeader 8500

/*
Pin 1.5 -> TA0.0
Pin 1.6 -> TA0.1
Pin 1.7 -> TA0.2
*/

volatile uint16_t CaptureBufferA[CaptureBufferSize];
volatile uint16_t lastCaptureA = 0;
volatile uint8_t framecompleteA = 0;
volatile uint8_t captureindexA = 0;
volatile uint8_t leaderDetectedA = 0; //0  Waiting ; 1 Detected

volatile uint16_t CaptureBufferB[CaptureBufferSize];
volatile uint16_t lastCaptureB = 0;
volatile uint8_t framecompleteB = 0;
volatile uint8_t captureindexB = 0;
volatile uint8_t leaderDetectedB = 0; //0  Waiting ; 1 Detected

volatile uint16_t CaptureBufferC[CaptureBufferSize];
volatile uint16_t lastCaptureC = 0;
volatile uint8_t framecompleteC = 0;
volatile uint8_t captureindexC = 0;
volatile uint8_t leaderDetectedC = 0; //0  Waiting ; 1 Detected

int bit;
volatile uint32_t result =0;

volatile uint32_t decodeNEC(volatile uint16_t *buffer)
{
    uint32_t value = 0;
    int index = 2;   // skip leader mark and leader space

    for(bit = 0; bit < 32; bit++)
    {
        uint16_t space = buffer[index + 1];  // skip mark, read space

        if(space > 1500)   // threshold between 562us and 1690us
        {
            value |= (1UL << bit);   // LSB first
        }

        index += 2;  // move to next mark/space pair
    }

    return value;
}

//Initializing capture mode CCR0
void initTimerACapturemodeCCR0(void){
    Timer_A_initCaptureModeParam capture = {0};
    capture.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0;
    capture.captureMode = TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE;
    capture.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    capture.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    capture.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    capture.captureOutputMode = TIMER_A_OUTPUTMODE_OUTBITVALUE;
    Timer_A_initCaptureMode(TIMER_A0_BASE, &capture);
}

//Initializing capture mode CCR1
void initTimerACapturemodeCCR1(void){
    Timer_A_initCaptureModeParam captureCCR1 = {0};
    captureCCR1.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    captureCCR1.captureMode = TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE;
    captureCCR1.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    captureCCR1.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    captureCCR1.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    captureCCR1.captureOutputMode = TIMER_A_OUTPUTMODE_OUTBITVALUE;
    Timer_A_initCaptureMode(TIMER_A0_BASE, &captureCCR1);
}

//Initializing capture mode CCR2
void initTimerACapturemodeCCR2(void){
    Timer_A_initCaptureModeParam captureCCR2 = {0};
    captureCCR2.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;
    captureCCR2.captureMode = TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE;
    captureCCR2.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    captureCCR2.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    captureCCR2.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    captureCCR2.captureOutputMode = TIMER_A_OUTPUTMODE_OUTBITVALUE;
    Timer_A_initCaptureMode(TIMER_A0_BASE, &captureCCR2);
}

//Enabling continuous mode
void initTimerAContinuousMode(void){
    Timer_A_initContinuousModeParam continuousmode = {0};
    continuousmode.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    continuousmode.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    continuousmode.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    continuousmode.timerClear = TIMER_A_DO_CLEAR;
    continuousmode.startTimer = true;
    Timer_A_initContinuousMode(TIMER_A0_BASE, &continuousmode);
}

void P1_5_1_7init(void){
    //P1.5 -> TA0.CCI0A
    P1DIR  &= ~BIT5;
    P1SEL0 |= BIT5;
    P1SEL1 |= BIT5;

    //P1.6 -> TA0.CCI1A
    P1DIR &= ~BIT6;
    P1SEL0 |= BIT6;
    P2SEL0 |= BIT6;
    
    //P1.7 -> TA0.CCI2A
    P1DIR &= ~BIT7;
    P1SEL0 |= BIT7;
    P2SEL0 |= BIT7;
}

int main(void) {
    // Stop watchdog timer
    WDT_A_hold(__MSP430_BASEADDRESS_WDT_A__);
    
    // Disable the GPIO power-on default high-impedance mode
    PMM_unlockLPM5();

    initTimerACapturemodeCCR0();

    initTimerACapturemodeCCR1();

    initTimerACapturemodeCCR2();
        
    //Enabling Timer_A Capture Compare Interrupt CCR0
    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
    
    initTimerAContinuousMode();

    P1_5_1_7init();

    __enable_interrupt();
    while(1){
        if(framecompleteA){
        result = decodeNEC(CaptureBufferA);
        // Capturebuffer
        framecompleteA = 0;
        leaderDetectedA = 0;
        }
        if(framecompleteB){
        result = decodeNEC(CaptureBufferB);
        // Capturebuffer
        framecompleteB = 0;
        leaderDetectedB = 0;
        }
        if(framecompleteC){
        result = decodeNEC(CaptureBufferC);
        // Capturebuffer
        framecompleteC = 0;
        leaderDetectedC = 0;
        }
    }
}

#pragma vector = TIMER0_A0_VECTOR //Definition of the interrupt vector
__interrupt void TIMER0_A0_CCR0_ISR(void){
    uint16_t currentcapture = Timer_A_getCaptureCompareCount(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
    uint16_t durationA = currentcapture - lastCaptureA;
    lastCaptureA = currentcapture;

    //leader detection
    if(durationA > LowerLeader && durationA < UpperLeader){
        leaderDetectedA = 1;
        captureindexA = 0;
    }

    if(leaderDetectedA){
        if(captureindexA < CaptureBufferSize){
            CaptureBufferA[captureindexA++] = durationA;
        }
        if(captureindexA == totalTimings){
            framecompleteA = 1;
            captureindexA = 0;
            leaderDetectedA = 0;
        }
    }
}

#pragma vector = TIMER0_A1_VECTOR //NEED TO DO MORE REASEARCH BUT SEEMS TO HANDLE BOTH CCR1 and CCR2 
__interrupt void TIMER0_A1_ISR(void){
//Pg112 MSP430 optimizing C/C++ Compiler for more information on _even_in_range 

    switch(__even_in_range(TA0IV,2)){
        case 2:
        {
            uint16_t currentcapture = Timer_A_getCaptureCompareCount(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1);
            uint16_t durationB = currentcapture - lastCaptureB;
            lastCaptureB = currentcapture;

               //leader detection
            if(durationB > LowerLeader && durationB < UpperLeader){
                leaderDetectedB = 1;
                captureindexB = 0;
            }

            if(leaderDetectedB){
                if(captureindexB < CaptureBufferSize){
                    CaptureBufferB[captureindexB++] = durationB;
                }
                if(captureindexB == totalTimings){
                    framecompleteB = 1;
                    captureindexB = 0;
                    leaderDetectedB = 0;
                    }
                }
        }
        case 4:
        {
            uint16_t currentcapture = Timer_A_getCaptureCompareCount(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_2);
            uint16_t durationC = currentcapture - lastCaptureC;
            lastCaptureC = currentcapture;

               //leader detection
            if(durationC > LowerLeader && durationC < UpperLeader){
                leaderDetectedC = 1;
                captureindexC = 0;
            }

            if(leaderDetectedC){
                if(captureindexC < CaptureBufferSize){
                    CaptureBufferC[captureindexC++] = durationC;
                }
                if(captureindexC == totalTimings){
                    framecompleteC = 1;
                    captureindexC = 0;
                    leaderDetectedC = 0;
                    }
                }
        }
    }
}

/*
Notes:
-Look more into unsigned subtraction to account for wrapping that may occur when timer is in continuous mode
-TimerA_CCR0 Has the highest priority so this is something that we should take into account as for what light it will correspond to
-Interrupt vector register TAxIV is used to determine which flag requested an interrupt
-Will need to transition to structure logic rather than just renaming with a,b,c etc.
*/