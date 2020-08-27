/*
 * ManchesterCode.c
 *
 *  Created on: 2020-8-17 *      Author: Cliff
 */
#include "ManchesterCode.h"
#include <ioavr.h>

volatile uint8_t  receiving = 0, databyte = 0;
volatile uint32_t samples = 0; //   unsigned long long volatile
volatile uint8_t  samplesReady = 0;
volatile int8_t   sampleCount = DATA_SAMPLE - 1;

uint8_t DiffManchester_SendByte( uint8_t byte, uint8_t lastEnd)
{
    int8_t bit = 7;

    for (; bit >= 0; --bit)
    {
      if ( IS_DataSET(byte,bit) )
        {
            if( 1 == lastEnd )
            {
              //no jump, 10
              lastEnd = 0;

              delay_us(HALF_TIME);
              SET_IO(TX_PORT, TX_PIN);

              delay_us(HALF_TIME);
              CLEAR_IO(TX_PORT, TX_PIN);
             }
            else
            {
              //no jump, 01
              lastEnd = 1;

              delay_us(HALF_TIME);
              CLEAR_IO(TX_PORT, TX_PIN);

              delay_us(HALF_TIME);
              SET_IO(TX_PORT, TX_PIN);
            }
        }
        else
        {
	   if( 1 == lastEnd )
		{
		  //jump, 01
		  lastEnd = 1;

		  delay_us(HALF_TIME);
		  CLEAR_IO(TX_PORT, TX_PIN);

		  delay_us(HALF_TIME);
		  SET_IO(TX_PORT, TX_PIN);
		 }
		else
		{
		  //no jump, 10
		  lastEnd = 0;
		  delay_us(HALF_TIME);
		  SET_IO(TX_PORT, TX_PIN);

		  delay_us(HALF_TIME);
		  CLEAR_IO(TX_PORT, TX_PIN);
		}
	  }
     }

   return lastEnd;
}

void  DiffManchester_SendData   (const uint8_t* data, uint8_t bytes)
{
	uint8_t i;
	uint8_t lastEnd = 1;

	/* signal for starting */
	delay_us( HALF_TIME );
	CLEAR_IO(TX_PORT,TX_PIN);

	delay_us( HALF_TIME );
	SET_IO  (TX_PORT,TX_PIN);

	/* Send data byte for byte */
	for (i = 0; i < bytes; ++i)
	{ lastEnd = DiffManchester_SendByte(data[i], lastEnd); }

	/* No more bits are sent but the last bit needs to finish delaying, otherwise if the
	 * last bit is a one it won't be recognized because I'm turning the TX off after, which
	 * would turn it into a 0 (Have to turn it off, else the LED will stay on). */

	delay_us( HALF_TIME );

	/* Clear pin after */
	CLEAR_IO(TX_PORT,TX_PIN);
}

uint32_t getSample(void)
{
    /* Reset flag */
    samplesReady = 0;

    return samples;
}

uint8_t interpretSample( void )
{
    int8_t i = 7;
    uint8_t bit;

    samplesReady = 0; 
    uint32_t samps = samples;
    for (; i >= 0; --i)
    {
        /* Grab the current bit */
        bit = (samps >> (i*4)) & 0x0F;

        if (     (bit == 0x03) || (bit == 0x0c)) CLEAR_DataBIT(databyte,i);
        else if ((bit == 0x0F) || (bit == 0x00)) SET_DataBIT(databyte,i);
        else return 0;
    }
    return 1;
}

uint8_t DiffManchester_GetData( uint8_t data[CODE_BYTELENGTH], uint8_t dataLens )
{
  uint8_t  error=1;
  if( 1 == samplesReady ) 
  {
	error=0;
	for( uint8_t i=0; i<dataLens; i++ )
	{
	  while( 0 == samplesReady );/*need to wait in here*/
	  {
		if( interpretSample(  ))
		{
		  data[i] = databyte;
		}
		else
		{
		  error=1; //break;                  
		}
	   }
	 }
  }

  /*clear the buffer and ready for next run*/
  delay_ms(1);
  receiving = 0;
  sampleCount = DATA_SAMPLE - 1;
  getSample();

  return error;
}

void DiffManchester_ReadBit( void )
{
  if( 1 == receiving )
  {
    //SET_IO(TX_PORT,TX_PIN);
    if (IS_IOSET(RX_PORT,RX_PIN))
    {
      SET_DataBIT(samples,sampleCount);  
    }
    else
    {
      CLEAR_DataBIT(samples,sampleCount);
    }
    //CLEAR_IO(TX_PORT,TX_PIN);
    /*If the bit is finished, set the samplesReady flag */
    if (! sampleCount--)
    {
      samplesReady = 1;
      sampleCount = DATA_SAMPLE - 1;
    }
  }
}

void DiffManchester_EnableRead( uint8_t True )
{
   receiving = True;
}

void DiffManchester_WaitForRead( void )
{
   while( 0 == samplesReady );
}
