#include <driverlib.h>
#include <msp430.h>
#include <msp430fr6989.h>
#include <stdio.h>

void setupSPI(void){
    UCA0CTLW0 |= UCSWRST;//Software reset enable (needed to be able to safely configure SPI)
    UCA0CTLW0 &= ~UCCKPH // Essentially is changing data by moving it on first edge(in our case rising edge) and on the next edge(falling edge) it is capturing new data
    UCA0CTLW0 &= ~UCCKPL // Choosing to havethe inactive state to be low as this is the state in which data is moved into the peripheral while when its high data is latched
    UCA0CTLW0 |= UCMSB | UCMST | UCMODE0 | UCSYNC; //Selecting MSB first; Selecting Master mode;Selecting 3pin SPI; Synchronous mode enabled;
    UCA0CTLW0 &= ~UCSSEL0;// Choosing SMCLK in master mode
    UCA0CTLW0 |= UCSSEL1; 
    UCA0CTLW0 &= ~UC7BIT;//Setting Character length to 8 bit data
    UCA0CTLW0 &= ~UCSWRST;//Needed for SPI to function correctly
}

//P4.2 -> SPI MOSI
P4DIR &= ~BIT2;
P4SEL0 |= BIT2;  
P4SEL1 &= ~BIT2; 

//P1.5 -> UCA0CLK 
P1DIR &= ~BIT5;
P1SEL0 &= ~BIT5;
P1SEL1 |= BIT5;


