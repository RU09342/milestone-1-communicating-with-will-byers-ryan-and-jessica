//Milestone_1- Stranger Things
//Jessica Wozniak, Ryan Hare, and Timothy Gordon
#include <msp430.h> 

#define RED BIT4 //Pin 1.4 is the TB0CCR1 output pin.
#define BLUE BIT5 //Pin 1.5 is the TB0CCR2 output pin.
#define GREEN BIT4 //Pin 3.4 is the TB0CCR3 output pin.

int size;
int duty[3];
unsigned int count = 0;

void LEDInit(void);

void TimerBInit(void);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop Watchdog
    PM5CTL0 &= ~LOCKLPM5;                   //disable HIGHZ

    LEDInit();
    TimerBInit();

    // Startup clock system with max DCO setting ~8MHz
    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;           // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers
    CSCTL0_H = 0;                           // Lock CS registers

    // Configure USCI_A0 for UART mode
    UCA0CTLW0 = UCSWRST;                    // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK;             // CLK = SMCLK
    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA0BRW = 52;                           // 8000000/16/9600
    UCA0MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA0CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
    UCA0IE |= UCRXIE;                       // Enable USCI_A0 RX interrupt

    __enable_interrupt(); //Enable interrupts.

    __bis_SR_register(LPM3_bits | GIE);     // Enter LPM3, interrupts enabled
    __no_operation();                       // For debugger
}

#pragma vector=EUSCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:
            while(!(UCA0IFG&UCTXIFG));

            switch (count)
            {
            case 0:
                {
                    size = UCA0RXBUF;
                }
            case 1:
                {
                    duty[0] = UCA0RXBUF;
                }
            case 2:
                {
                    duty[1] = UCA0RXBUF;
                }
            case 3:
                {
                    duty[2] = UCA0RXBUF;
                    UCA0TXBUF = (size - 0x03);
                    __no_operation();
                }
            }
        case USCI_UART_UCTXIFG: break;
        case USCI_UART_UCSTTIFG: break;
        case USCI_UART_UCTXCPTIFG: break;

        default: UCA0TXBUF = UCA0RXBUF;
                break;

    }
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

    //Set initial values for CCR1, CCR2, CCR3
    TB0CCR1 = 200; //Red
    TB0CCR2 = 200; //Blue
    TB0CCR3 = 200; //Green

    TB0CCR0 = 255; //Set CCR0 for a ~1kHz clock.

    TB0CTL = TBSSEL_2 + MC_1; //Enable Timer B0 with SMCLK and up mode.
}
