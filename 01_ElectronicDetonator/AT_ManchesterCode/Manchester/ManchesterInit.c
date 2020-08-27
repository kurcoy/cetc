/*
 * ManchesterInit.c
 *
 *  Created on: 2020-8-17 *      Author: Cliff
 */

#include "ManchesterInit.h"
#include "ManchesterCode.h"
#include <ioavr.h>

/***********************************RX*************************************/
/***********************************RX*************************************/
void initRX(void)
{
    /* close interrupte and set CPU frequency=8MHz  */  
    asm("cli");  
    CLKPR =  0x80;
    CLKPR =  0x00;
    
    /* Port B Data Direction Register: Set the RX and TX pin */
    CLEAR_BIT(DDRB,DDB1);
    SET_BIT(DDRB,DDB0);
    
    /* General Interrupt Mask Register: Enable pin change interrupt */
    SET_BIT(GIMSK,PCIE);
    
    /* Pin Change Mask Register: Watch pin change on pin 3 */
    SET_BIT(PCMSK,PCINT1);
    

    /* TCNT1 Timer/Counter Register: Enable the compare match interrupt for Timer1 */
    /* Enable TCNT1 with clk/8 pre-scaling */
    TCNT1 = 0;
    SET_BIT(TIMSK,OCIE1A);
    OCR1A =125;
    
    asm("sei"); 
    
}

void startRX(void)
{
  /* Enable sampling Timer/Counter and interrupts */
  SET_BIT(TIMSK,OCIE1A);
  SET_BIT(TCCR1,CS12);
}

void stopRX(void)
{
  /* Disable sampling Timer/Counter */  
  DiffManchester_EnableRead ( 0 );     
  CLEAR_BIT(TCCR1,CS12);
}

uint32_t samples1, sampleCount1, samplesReady1;
#pragma vector= TIM1_COMPA_vect 
__interrupt void timer1isr(void)
{
  TCNT1 = 10; 
  DiffManchester_ReadBit( );	
}

#pragma vector=PCINT0_vect 
__interrupt void pcint0isr(void)
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
