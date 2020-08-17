
//  Created by Peter Goldsborough on 02/06/14.
//  Copyright (c) 2014 Peter Goldsborough. All rights reserved.
//

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
  
    TCNT0 = TCNT1= 0;
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
 
}

void startRX(void)
{
    uint8_t preambleBit, lows = 0, highs = 0;
    
    //SET_BIT(PORTB,LED);
    
    /* Enable global interrupts */
    asm("sei");
            
    /* Timer/Counter Register: Reset Timer/Counter0 Register*/
    //TCNT0 = TCNT1 = 0;

    /* Enable while waiting for the preamble. Is turned false when the connection
     times out. */
    //receiving = 1;
    
    /* Going to record individual bits now */
    //sampleCount = 3;
    
    /* Reset this. Otherwise all RX attempts will fail after the first one. Not sure why. */
    //samples = 0;
    
    /* receiving becomes false if the connection times out, so this will not go on forever */
    /*
    while (receiving)
    {
        if (samplesReady)
        {
        
            preambleBit = getSample();
            
            sampleCount = 3;
                
            if( preambleBit == LOW )
            {
                if (!highs) ++lows;
                
                else lows = highs = 0;
            }
            
            else if (preambleBit == HIGH)
            {
                if (lows >= 6)
                {
                    if (++highs >= 2) break;
                }
                
                else lows = highs = 0;
            }
            
            else
            {
                lows = highs = 0;
                sampleCount = 2;
            }
        }
    }
    */
    //sampleCount = DATA_SAMPLE - 1;
    
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
  /*  
  if (++connectionCount >= CONNECTION_TIME)
    {
        connectionCount = 0;
        
        stopRX();
    }
  */
}

#pragma vector=PCINT0_vect 
__interrupt void pcint0isr(void)
//ISR(PCINT0_vect)
{
    /* Set the timer to what it should be at the pin change. 52 instead
     of 50 because of ISR-call overhead. */
    DiffManchester_EnableRead( 1 );
    TCNT0 = 62;//52
}

#pragma vector=TIM0_COMPA_vect
__interrupt void timer0isr(void)
//ISR(TIMER0_COMPA_vect)
{
  DiffManchester_ReadBit(  );

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
