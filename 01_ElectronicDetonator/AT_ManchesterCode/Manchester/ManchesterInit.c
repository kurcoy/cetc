
//  Created by Peter Goldsborough on 02/06/14.
//  Copyright (c) 2014 Peter Goldsborough. All rights reserved.
//

#include "ManchesterInit.h"
#include "ManchesterCode.h"
#include <ioavr.h>

uint16_t connectionCount=0;

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
    
    /* Enable Timer1 with clk/4096 prescaling. //0x0E, ck/8192
     Timer1 is used for timing out the connection pipeline, overflow interrupt( Timer/Counter Register TCNT0 )
     Timer0 is used for sampling, output compare interrupt( Output Compare Register A OCR0A)*/
    //TCCR1 |= 0x0D;
    
    /* TCNT1 Timer/Counter Register: Enable the overflow interrupt for Timer1 */
    //SET_BIT(TIMSK,TOIE1);
  
    TCNT0 = 0;
    /*Enable Timer0 CTC mode*/
    CLEAR_BIT(TCCR0A,WGM00);
    SET_BIT  (TCCR0A,WGM01);
    CLEAR_BIT(TCCR0B,WGM02);
    
    /* We are not using the Clear Timer on Compare Match feature. The reason
     is that it's not actually the ISR call that needs to be called regularly,
     but the sampling, therefore we clear the timer after the sample and don't
     let the timer be cleared automatically. */
    
    /* Output Compare Register A: Sample time */
    OCR0A = 255;
 
    /* Enable the output compare interrupt for Timer0 */
    SET_BIT  (TIMSK,OCIE0A);
    //SET_BIT  (TIMSK, TOIE1);
    /* Turn on Timer0 with clk/8 prescaling, gives 1µs per cycle @8Mhz.
     Timer0 is used for sampling */
 
    SET_BIT  (TCCR0B,CS01); 
    
    TCNT1 = 0;
    /* Enable TCNT1 with clk/8 pre-scaling */
    CLEAR_BIT(TCCR1,CS10);
    CLEAR_BIT(TCCR1,CS11);
    SET_BIT  (TCCR1,CS12);
    CLEAR_BIT(TCCR1,CS13);
    SET_BIT(TIMSK,TOIE1);
}

void startRX(void)
{
    uint8_t preambleBit, lows = 0, highs = 0;
    
    //SET_BIT(PORTB,LED);
    
    /* Enable global interrupts */
    asm("sei");         
}

void stopRX(void)
{
    /* Stop the wait for the preamble if it's still running,
     else stop waiting for samples in main */
    DiffManchester_EnableRead ( 0 );     
    /* Disable sampling Timer/Counter */
    CLEAR_BIT(TIMSK,OCIE0A);

}


//ISR(TIMER1_OVF_vect)//TIM1_OVF_vect 
#pragma vector= TIM1_OVF_vect
__interrupt void timer1isr(void)
{
  DiffManchester_ReadBit(  );
  
  if (++connectionCount >= 8000)
  {
    //DiffManchester_SendByte   ( 0x00, 0 );  
    connectionCount = 0;
    //SET_BIT(TX_PORT,TX_PIN);
  }
  TCNT1 = 130;
}

#pragma vector=PCINT0_vect 
__interrupt void pcint0isr(void)
//ISR(PCINT0_vect)
{
  DiffManchester_EnableRead( 1 );
  TCNT1 = 65+130;//52
}

#pragma vector=TIM0_COMPA_vect
__interrupt void timer0isr(void)
//ISR(TIMER0_COMPA_vect)
{
  

}

/***********************************TX*************************************/
/***********************************TX*************************************/
void initTX(void)
{
    /* Set TX_PIN as output */
    SET_BIT(DDRB,DDB0);
	
    /* Enable TCNT1 with clk/8 pre-scaling */
    CLEAR_BIT(TCCR1,CS10);
    CLEAR_BIT(TCCR1,CS11);
    SET_BIT  (TCCR1,CS12);
    CLEAR_BIT(TCCR1,CS13);
}
