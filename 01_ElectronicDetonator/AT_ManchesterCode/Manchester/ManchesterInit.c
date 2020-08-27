
//  Created by Peter Goldsborough on 02/06/14.
//  Copyright (c) 2014 Peter Goldsborough. All rights reserved.
//

#include "ManchesterInit.h"
#include "ManchesterCode.h"
#include <ioavr.h>

uint16_t usTimer1Count=0;
uint16_t usTimer0Count=0;
volatile uint8_t  usTimerEnable=0;
/***********************************RX*************************************/
/***********************************RX*************************************/
void initRX(void)
{
    /* close interrupte and set CPU frequency=8MHz  */  
    asm("cli");  
    CLKPR =  0x80;
    CLKPR =  0x00;
    
    /* Port B Data Direction Register: Set the RX pin as input */
    CLEAR_BIT(DDRB,DDB1);
    CLEAR_BIT(DDRB,DDB4);
    /* Port B Data Direction Register: Set LED as output */
    //SET_BIT(DDRB,DDB4);
    
    /* General Interrupt Mask Register: Enable pin change interrupt */
    SET_BIT(GIMSK,PCIE);
    
    /* Pin Change Mask Register: Watch pin change on pin 3 */
    SET_BIT(PCMSK,PCINT1);
    

    /* TCNT1 Timer/Counter Register: Enable the overflow interrupt for Timer1 */
    /* Enable TCNT1 with clk/8 pre-scaling */
    TCNT1 = 0;
    SET_BIT(TIMSK,OCIE1A);
    OCR1A =125;
    
    asm("sei"); 
    
}

void startRX(void)
{
    /* Enable global interrupts */
    //OCR1C = 125;
    SET_BIT(TIMSK,OCIE1A);
    SET_BIT  (TCCR1,CS12);
    //SET_BIT  (TCCR1,CS10);
    usTimerEnable=0;
}

void stopRX(void)
{
    /* Stop the wait for the preamble if it's still running,
     else stop waiting for samples in main */
    DiffManchester_EnableRead ( 0 );     
    /* Disable sampling Timer/Counter */

    CLEAR_BIT(TCCR1,CS12);
    usTimerEnable=0;

}

uint32_t samples1, sampleCount1, samplesReady1;
#pragma vector= TIM1_COMPA_vect //TIM1_COMPA_vect
__interrupt void timer1isr(void)
{
  TCNT1 = 10; 
  DiffManchester_ReadBit( );	
  //change the value at the end of interrupt
}

#pragma vector=PCINT0_vect 
__interrupt void pcint0isr(void)
//ISR(PCINT0_vect)
{
  TCNT1 = 70; 
  DiffManchester_EnableRead( 1 );
}

/***********************************TX*************************************/
/***********************************TX*************************************/
void initTX(void)
{
    /* Set TX_PIN as output */
    SET_BIT(DDRB,DDB0);
}
