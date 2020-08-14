
//  Created by Peter Goldsborough on 02/06/14.
//  Copyright (c) 2014 Peter Goldsborough. All rights reserved.
//

#include "ManchesterCode.h"
#include <ioavr.h>

volatile uint32_t samples = 0;
volatile uint8_t  samplesReady = 0;
volatile int8_t   sampleCount = DATA_SAMPLE - 1;

uint8_t receiving = 0, databyte = 0, connectionCount = 0;

uint16_t computeChecksum(const uint8_t* data, uint16_t bytes);
uint32_t getSample(void);
uint8_t  interpretSample (const uint32_t samps);

uint16_t computeChecksum(const uint8_t* data, uint16_t bytes)
{
    /* * * * * * * * * * * * * * * * * * * * * * * * * *
     *                                                 *
     *  Apparently 0xFF (255) is preferred over 0      *
     *  because it provides better shielding against   *
     *  overflow and prevents the checksum from ever   *
     *  becoming zero, therefore providing a checksum  *
     *  computed flag when the returned value is non-  *
     *  zero. Taken from here: http://goo.gl/Cjt4AO    *
     *                                                 *
     * * * * * * * * * * * * * * * * * * * * * * * * * */
    
    uint16_t sum1 = 0xFF;
    uint16_t sum2 = 0xFF;
    
    uint16_t wordLen;
    
    while (bytes)
    {
        /* maximum word size before overflow would occur */
        wordLen = (bytes > 20) ? 20 : bytes;
        
        bytes -= wordLen;
        
        do
        {
            sum1 += *(data++);
            sum2 += sum1;
        }
        
        while (--wordLen);
        
        /* * * * * * * * * * * * * * * * * * * * * *
         * Do first modular reduction: mask off
         * lower byte and add right shifted upper
         * byte to it
         * * * * * * * * * * * * * * * * * * * * * */
        
        sum1 = (sum1 & 0xFF) + (sum1 >> 8);
        sum2 = (sum2 & 0xFF) + (sum2 >> 8);
    }
    
    /* Safety reduction */
    sum1 = (sum1 & 0xFF) + (sum1 >> 8);
    sum2 = (sum2 & 0xFF) + (sum2 >> 8);
    
    /* Return both sums in a 16 bit value */
    return sum2 << 8 | sum1;
}

uint32_t getSample(void)
{
    /* Reset flag */
    samplesReady = 0;
    
    return samples;
}
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
    receiving = 1;
    
    /* Going to record individual bits now */
    sampleCount = 3;
    
    /* Reset this. Otherwise all RX attempts will fail after the first one. Not sure why. */
    samples = 0;
    
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
    sampleCount = DATA_SAMPLE - 1;
    
}

void stopRX(void)
{
    /* Stop the wait for the preamble if it's still running,
     else stop waiting for samples in main */
    receiving = 0;
    
    /* Disable sampling Timer/Counter */
    CLEAR_BIT(TIMSK,OCIE0A);

}


void senddata(uint8_t byte)
{
    int8_t bit = 7;
    
    for (; bit >= 0; --bit)
    {
        if ( IS_SET(byte,bit) )
        {
            while(TCNT1 < HALF_TIME);
            SET_BIT(TX_PORT,TX_PIN);
            
            TCNT1 = 1;
        }
        
        else
        { 
            while(TCNT1 < HALF_TIME);
            CLEAR_BIT(TX_PORT,TX_PIN);
            
            TCNT1 = 1;
        }
        
    }
    
}
uint8_t DiffManchester_GetData( uint8_t **data, uint8_t dataLens )
{  
  if( samplesReady )
  {
    uint32_t fisrtSamps = getSample(); 
    uint8_t  bit = (fisrtSamps >> (30)) & 0x0F;
    if( interpretSample( fisrtSamps ) ) *data[0]  = (~bit) || databyte << (1);
    else return 0 ;
  }
  
  for( int i=1; i<dataLens; i++ )
  {
    if (receiving && samplesReady )
    {
      if( interpretSample(getSample()))
      {
        *data[i] = databyte;  
      }
      else
      {
        return 0;
      }
    }
  }
  
  for(  uint8_t i=0; i <dataLens; i++ )
    senddata(*data[i]);
  
  return 1;
}

uint8_t interpretSample(const uint32_t samps)
{
    int8_t i = 7;
    
    uint8_t bit;
    
    for (; i >= 0; --i)
    {
        /* Grab the current bit */
        bit = (samps >> (i*4)) & 0x0F;
        
        //if ((bit == HIGH)) SET_BIT(databyte,i);
        //else if (bit == LOW) CLEAR_BIT(databyte,i);       
        if ((bit == 0x03) || ( (bit == 0x0c) ))   CLEAR_BIT(databyte,i);
        else if ((bit == 0x00) || (bit == 0x0F) ) SET_BIT  (databyte,i);
        else return 0;
    }
    
    return 1;
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
    TCNT0 = 62;//52
}

#pragma vector=TIM0_COMPA_vect
__interrupt void timer0isr(void)
//ISR(TIMER0_COMPA_vect)
{
  /* If the current sample reads high, set the current bit in the samples */
  if (IS_SET(RX_PORT,RX_PIN))
  {
      //TCNT0 = 1; //Writing to the TCNT0 Register blocks (removes) the Compare Match on the following timer clock.
      SET_BIT(samples,sampleCount);
  }
  else
  {
      //TCNT0 = 1;//Writing to the TCNT0 Register blocks (removes) the Compare Match on the following timer clock.
      CLEAR_BIT(samples,sampleCount);
  }

  //If the bit is finished, set the samplesReady flag /
  if (! sampleCount--)
  {
          samplesReady = 1;
          sampleCount = DATA_SAMPLE - 1;
  }
 /*
  TCNT1 = 1;
  SET_BIT(TX_PORT,TX_PIN);
  
  while(TCNT1 < 150);
  CLEAR_BIT(TX_PORT,TX_PIN);
  //delay_us(50);
*/
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
void Manchester_sendData(const uint8_t* data, uint16_t bytes)
{
    uint8_t i;
    uint16_t chksm;
    
    /* Compute the checksum for the given data */
    chksm = computeChecksum(data,bytes);
    
    /* Send lots of 10s to let the receiver adjust to the signal */
    for(i = 0; i < 75; ++i)
    { Manchester_sendByte(0); }
    
    /* Send last 10s and the two 01 start pulses */
    Manchester_sendByte(3);
    
    /* Send data byte for byte */
    for (i = 0; i < bytes; ++i)
    { Manchester_sendByte(data[i]); }
    
    /* Append checksum, upper and lower byte */
    Manchester_sendByte((chksm >> 8));
    Manchester_sendByte((chksm & 0xFF)); // you can leave the & 0xFF away here if you want
    
    /* No more bits are sent but the last bit needs to finish delaying, otherwise if the
     * last bit is a one it won't be recognized because I'm turning the TX off after, which
     * would turn it into a 0 (Have to turn it off, else the LED will stay on). */
    
    while(TCNT0 < HALF_TIME);
    
    /* Clear pin after */
    CLEAR_BIT(TX_PORT,TX_PIN);
}

void Manchester_sendByte(const uint8_t byte)
{
    int8_t bit = 7;
    
    for (; bit >= 0; --bit)
    {
        if ( IS_SET(byte,bit) )
        {
            /* * * * * * * * * * * * * * * * * * *
             * Set bit low, wait for mid-period,
             * then set high again and wait for
             * end of period. This indicates a
             * high/set bit according to Manchester
             * Encoding.
             * * * * * * * * * * * * * * * * * * */
            
            while(TCNT0 < HALF_TIME);
            CLEAR_BIT(TX_PORT,TX_PIN);
            
            TCNT0 = 0;
			
            while(TCNT0 < HALF_TIME);
            SET_BIT(TX_PORT,TX_PIN);
            
            TCNT0 = 1;
        }
        
        else
        {
            /* * * * * * * * * * * * * * * * * * *
             * Set bit high, wait for mid-period,
             * then set low again and wait for
             * end of period. This indicates a
             * low/cleared bit according to
             * Manchester Encoding.
             * * * * * * * * * * * * * * * * * * */
            
	    while(TCNT0 < HALF_TIME);
            SET_BIT(TX_PORT,TX_PIN);
            
            TCNT0 = 0;
			
            while(TCNT0 < HALF_TIME);
            CLEAR_BIT(TX_PORT,TX_PIN);
            
            TCNT0 = 1;
        }
        
    }
    
}

void DiffManchester_sendData(const uint8_t* data, uint16_t bytes)
{
    uint8_t i;
    uint8_t lastEnd = 1;
  
    /* signal for starting */
    while(TCNT0 < HALF_TIME);
    CLEAR_BIT(TX_PORT,TX_PIN);
    
    TCNT0 = 0;
                
    while(TCNT0 < HALF_TIME);
    SET_BIT(TX_PORT,TX_PIN);
    
    TCNT0 = 1;  
    
    /* Send data byte for byte */
    for (i = 0; i < bytes; ++i)
    { lastEnd = DiffManchester_sendByte(data[i], lastEnd); }
    
    /* No more bits are sent but the last bit needs to finish delaying, otherwise if the
     * last bit is a one it won't be recognized because I'm turning the TX off after, which
     * would turn it into a 0 (Have to turn it off, else the LED will stay on). */
    
    while(TCNT0 < HALF_TIME);
    
    /* Clear pin after */
    CLEAR_BIT(TX_PORT,TX_PIN);
}

uint8_t DiffManchester_sendByte( uint8_t byte, uint8_t lastEnd)
{
    int8_t bit = 7;

    for (; bit >= 0; --bit)
    {
      if ( IS_SET(byte,bit) )
        {
            if( 1 == lastEnd )
            {
              //no jump, 10
              lastEnd = 0;
              
              while(TCNT0 < HALF_TIME);
              SET_BIT(TX_PORT,TX_PIN);
              
              TCNT0 = 0;
                          
              while(TCNT0 < HALF_TIME);
              CLEAR_BIT(TX_PORT,TX_PIN);
              
              TCNT0 = 1;
             }
            else
            {
              //no jump, 01
              lastEnd = 1;
              
              while(TCNT0 < HALF_TIME);
              CLEAR_BIT(TX_PORT,TX_PIN);
              
              TCNT0 = 0;
                          
              while(TCNT0 < HALF_TIME);
              SET_BIT(TX_PORT,TX_PIN);
              
              TCNT0 = 1;  
            }  
        } 
        else
        {           
	   if( 1 == lastEnd )
            {
              //jump, 01
              lastEnd = 1;
              
              while(TCNT0 < HALF_TIME);
              CLEAR_BIT(TX_PORT,TX_PIN);
              
              TCNT0 = 0;
                          
              while(TCNT0 < HALF_TIME);
              SET_BIT(TX_PORT,TX_PIN);
              
              TCNT0 = 1; 
             }
            else
            {
              //no jump, 10
              while(TCNT0 < HALF_TIME);
              SET_BIT(TX_PORT,TX_PIN);
              
              TCNT0 = 0;
                          
              while(TCNT0 < HALF_TIME);
              CLEAR_BIT(TX_PORT,TX_PIN);
              
              TCNT0 = 1; 
            }  
          }
     }
   return lastEnd; 
}
