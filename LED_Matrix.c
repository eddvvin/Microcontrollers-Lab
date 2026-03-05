#include <driverlib.h>
#include <msp430.h>
#include <msp430fr6989.h>
#include <stdio.h>
#define NUM_of_digit 3

volatile int display_update_flag = 0;
volatile uint8_t spiBUSY = 0;
volatile uint8_t *TXbuff;
volatile uint8_t TXlength;
volatile uint8_t bufferSPI;

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
        0b00110000,
        0b01100000,
        0b01111110,
        0b00000000
    }
};

void setupSPI(void){
    UCA0CTLW0 |= UCSWRST;//Software reset enable (needed to be able to safely configure SPI)
    UCA0CTLW0 &= ~UCCKPH; // Essentially is changing data by moving it on first edge(in our case rising edge) and on the next edge(falling edge) it is capturing new data
    UCA0CTLW0 &= ~UCCKPL; // Choosing to havethe inactive state to be low as this is the state in which data is moved into the peripheral while when its high data is latched
    UCA0CTLW0 |= UCMSB | UCMST | UCMODE0 | UCSYNC; //Selecting MSB first; Selecting Master mode;Selecting 3pin SPI; Synchronous mode enabled;
    UCA0CTLW0 &= ~UCSSEL0;// Choosing SMCLK in master mode
    UCA0CTLW0 |= UCSSEL1; 
    UCA0CTLW0 &= ~UC7BIT;//Setting Character length to 8 bit data
    UCA0IE |= UCTXIE; //Enable eUSCIO TX interrupt
    UCA0CTLW0 &= ~UCSWRST;//Needed for SPI to function correctly
}

void CS_LOW(void){
    P9OUT &= ~BIT6;
}

void CS_HIGH(void){
    P9OUT |= BIT6;
}

//Need to add a function that essentially will transfer the desired address and data so that it can program the LED matrix
void transmitdataSPI(uint8_t rowaddress, uint8_t data){
    uint8_t bufferSPI[2]; //Needed so that we can send out 1 byte out at a time since that is the max we can send
    bufferSPI[0] = rowaddress; //Row address
    bufferSPI[1] = data; //data being sent

    TXbuff = bufferSPI; //setting ptr = to array meaning we are pointing to the first index
    TXlength = 2; //buffer length

    CS_LOW();
    UCA0TXBUF = *TXbuff++; //derefrencing pointer to get value and then incrementing
    TXlength --; //decrementing length so we can know when we are done sending byte
    spiBUSY = 1; //mark as busy
}

// void ledmatrixInit(){

// }

void sendimageSPI(uint8_t digit){ 
    uint8_t row;
    for(row = 0; row < 8; row++){
        transmitdataSPI(row, images[digit][row]);
    }
}

void pinInit(void){
//P4.2 -> SPI MOSI
P4DIR &= ~BIT2;
P4SEL0 |= BIT2;  
P4SEL1 &= ~BIT2; 

//P1.5 -> UCA0CLK 
P1DIR &= ~BIT5;
P1SEL0 &= ~BIT5;
P1SEL1 |= BIT5;

P9DIR |= BIT6;
P9SEL0 &= ~BIT6;
P9SEL1 &= ~BIT6;
}

int main(void){
    uint32_t smclk = CS_getSMCLK();
    printf("SMCLK = %lu Hz\n", smclk);
    WDTCTL = WDTPW | WDTHOLD; //Used password and halted watchdog timer
    // Disable the GPIO power-on default high-impedance mode
    PMM_unlockLPM5();
    pinInit();
    setupSPI();
    __enable_interrupt();
    while(1)
        if(display_update_flag){
            display_update_flag = 0;
            sendimageSPI(0);
        }
}

#pragma vector = USCI_A0_VECTOR // Will activate once buffer is clear
__interrupt void USCI_A0_ISR(void){
    switch(UCA0IV){
        case 2:
            break; //Am not using but for reading buffer
        case 4:
            UCA0TXBUF = *TXbuff++; //Putting next byte into buffer
            TXlength--;
            if(TXlength == 0){
                spiBUSY = 0;
                CS_HIGH(); // Driving high to latch data
            }
        default:
            break;
    }
}
