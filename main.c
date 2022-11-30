#include <msp430.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


/**
 * main.c
 */

// make the following connections

#define     LED1                  BIT0
#define     LED2                  BIT6
#define     LED3                  BIT7
#define     LED4                  BIT3
#define     LED5                  BIT4
#define     LED6                  BIT5

#define     VRX                   BIT1

#define     BEEP                  BIT2

void main(void)
{
    int i;
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    ADC10CTL0 = ADC10SHT_2 + ADC10ON;         // ADC10ON
    ADC10CTL1 = INCH_1;                       // A1 as input
    ADC10AE0 |= 0x02;                         // PA.1 ADC option select

    // set up piezoelectric beeper and LED ports to output
    P1DIR |= BEEP + LED1 + LED2 + LED3 + LED4 + LED5 + LED6;
    P1OUT &= ~(BEEP + LED1 + LED2 + LED3 + LED4 + LED5 + LED6);
    P1SEL |= BEEP;


//  // pull up input ports
    P1REN |= VRX;

    srand(time(0));

    // note: changed beep from P1.0 to P1.7 cause 1.0 had too much feedback

    CCR0 = 1000;                 // PWM Period
    CCTL1 = OUTMOD_7;          // CCR1 reset/set
    CCR1 = 250;                // CCR1 PWM duty cycle
    TACTL = TASSEL_2 + MC_1;       // SMCLK, up mode

    for (;;) {
        int num = (rand() % 6);

        P1OUT = lightUp(num);

        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
        while (ADC10CTL1 &ADC10BUSY);         // ADC10BUSY?
        ADC10CTL1 = INCH_1;                       // input A7

        if (853 < ADC10MEM && ADC10MEM <= 1023)
            P1OUT |= LED1;
        else if (682 < ADC10MEM && ADC10MEM <= 853)
            P1OUT |= LED2;
        else if (512 < ADC10MEM && ADC10MEM <= 682)
            P1OUT |= LED3;
        else if (341 < ADC10MEM && ADC10MEM <= 512)
            P1OUT |= LED4;
        else if (171 < ADC10MEM && ADC10MEM <= 341)
            P1OUT |= LED5;
        else if (0 < ADC10MEM && ADC10MEM <= 341)
            P1OUT |= LED6;
        else
            P1OUT = 0;

        if (isMatched(num, P1OUT)) {
            _delay_cycles(1000000);
            continue;
        }

        CCR0 = playTone(P1OUT); // play tone based on which light is on
    }

    //_BIS_SR(LPM0_bits); // Enter Low Power Mode 0

}

// plays tone based on which light is on
int playTone(int lights) {
    if (lights & LED1)
        return 2000;
    else if (lights & LED2)
        return 4000;
    else if (lights & LED3)
        return 6000;
    else if (lights & LED4)
        return 8000;
    else if (lights & LED5)
        return 9000;
    else if (lights & LED6)
        return 10000;
    else
        return 2000; // just in case
}

// returns which LED to light up
int lightUp(int i) {
    if (i == 0)
        return LED1;
    else if (i == 1)
        return LED2;
    else if (i == 2)
        return LED3;
    else if (i == 3)
        return LED4;
    else if (i == 4)
        return LED5;
    else if (i == 5)
        return LED6;
    else
        return 0;
}

// return 1 if numbers match
int isMatched(int out, int in) {
    if (out == in)
        return 1;
    else
        return 0;
}
