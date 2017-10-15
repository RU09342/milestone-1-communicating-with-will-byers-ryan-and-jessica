#include <msp430.h> 

#define RED BIT4 //Pin 1.4 is the TB0CCR1 output pin.
#define BLUE BIT5 //Pin 1.5 is the TB0CCR2 output pin.
#define GREEN BIT4 //Pin 3.4 is the TB0CCR3 output pin.

void LEDInit(void);

void TimerBInit(void);

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; //Disable high impedance modes.

    LEDInit();
    TimerBInit();

    __enable_interrupt(); //Enable interrupts.
    __bis_SR_register(LPM0 + GIE);  //Enter low power mode with interrupts.

}

void LEDInit(void) {
    //For pin 1.4, 1.5, and 3.4, P1DIR = 1, P1SEL0 = 1, P1SEL1 = 0.
    P1DIR |= RED;
    P1SEL1 &= ~RED;
    P1SEL0 |= RED;

    P1DIR |= BLUE;
    P1SEL1 &= ~BLUE;
    P1SEL0 |= BLUE;

    P3DIR |= GREEN;
    P3SEL1 &= ~GREEN;
    P3SEL0 |= GREEN;
}

void TimerBInit(void) {
    TB0CCTL1 = OUTMOD_3; //Set OUTMOD_3 (set/reset) for CCR1, CCR2, CCR3
    TB0CCTL2 = OUTMOD_3;
    TB0CCTL3 = OUTMOD_3;

    TB0CCTL0 = CCIE;

    TB0CCR1 = 512; //Set initial values for CCR1, CCR2, CCR3
    TB0CCR2 = 512;
    TB0CCR3 = 512;

    TB0CCR0 = 1024; //Set CCR0 for a ~1kHz clock.

    TB0CTL = TBSSEL_2 + MC_1; //Enable Timer B0 with SMCLK and up mode.
}
