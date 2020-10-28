
//  Created by Peter Goldsborough on 02/06/14.
//  Copyright (c) 2014 Peter Goldsborough. All rights reserved.
//

#include "ManchesterInit.h"
#include "ManchesterCode.h"
//#include <ioavr.h>


volatile uint16_t usTimer0Count=0;
volatile uint8_t  counter=250;
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
    /*
	TCNT1 = 0;
    SET_BIT(TIMSK,OCIE1A);
    OCR1A =125;
    */
	TCNT0 = 0;
    CLEAR_BIT(TCCR0A,WGM00);
    SET_BIT  (TCCR0A,WGM01);
    CLEAR_BIT(TCCR0B,WGM02);
    OCR0A = 250;
    /* Enable the output compare interrupt for Timer0 */
    SET_BIT(TIMSK,OCIE0A);
    SET_BIT(TCCR0B,CS01);
	
    asm("sei"); 
    
}

void startRX(void)
{
    /* Enable global interrupts */
    SET_BIT(TIMSK,OCIE1A);
    SET_BIT(TCCR1,CS12);
}

void stopRX(void)
{
    /* Stop the wait for the preamble if it's still running,
     else stop waiting for samples in main */
    DiffManchester_EnableRead ( 0 );     
    /* Disable sampling Timer/Counter */

    CLEAR_BIT(TCCR1,CS12);
}
#pragma interrupt_handler timer0isr:11
void timer0isr()
{
    TCNT0 = 0;
	usTimer0Count++;
}

//#pragma vector= TIM1_COMPA_vect //TIM1_COMPA_vect
//__interrupt void timer1isr(void)
#pragma interrupt_handler timer1isr:4
void timer1isr()
{
    TCNT1 = 0;
	DiffManchester_ReadBit( );	
    //TCNT1 = 20; //50
}

//#pragma vector=PCINT0_vect 
//__interrupt void pcint0isr(void)
#pragma interrupt_handler pcint0isr:3
void pcint0isr()
//ISR(PCINT0_vect)
{
  TCNT1 = 65;//80
  DiffManchester_EnableRead( 1 );
  //TCNT1 = 80;//80
}

/***********************************TX*************************************/
/***********************************TX*************************************/
void initTX(void)
{
    /* Set TX_PIN as output */
    SET_BIT(DDRB,DDB0);
}
/**************************************************************************/

void delay_us(unsigned int us)
{
  TCNT0=0;
  while(TCNT0 < us);
  //TCNT0 = 0; 
  
  //uint8_t value = (TCNT0 + us)%counter; 
  //while( TCNT0 < value);
}

/**************************************************************************/
void delay_1ms(void)
{
 unsigned int i;
 for(i=1;i<(unsigned int)(CPU_F*143-2);i++);
}

void delay_ms(unsigned int x)
{
/*
 unsigned int i;
 for (i=0; i<x;i++)
 	 delay_1ms( );
  */ 
  usTimer0Count = 0;
  while( usTimer0Count < 4*x); 
 
}