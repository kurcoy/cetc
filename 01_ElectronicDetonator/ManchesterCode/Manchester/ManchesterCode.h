/*
 * ManchesterCode.h
 *
 *  Created on: 2020-8-17 *      Author: Cliff
 */
#ifndef INC_MANCHESTERCODE_H_
#define INC_MANCHESTERCODE_H_

#include "ManchesterInit.h"

#define CODE_BYTELENGTH 25

#define ENABLE_BIT_DEFINITIONS

/* 2500 bits per second*/
#define BAUD_RATE 2000
/* The time for one bit in 碌s/cycles */
//#define BIT_TIME  500
//#define SAMPLE_TIME 125
#define HALF_TIME 250
/* The time between samples in 碌s/cycles */

/* The number of sample per data unit */
#define DATA_SAMPLE 32

#define IS_IOSET(GPIO,GPIO_PIN)  (((GPIO) & (1UL << (GPIO_PIN))) >> (GPIO_PIN))
#define SET_IO(GPIO,GPIO_PIN)    ((GPIO)  |= (1UL << (GPIO_PIN)))
#define CLEAR_IO(GPIO,GPIO_PIN)  ((GPIO)  &= ~(1UL << (GPIO_PIN)))

#define IS_DataSET(byte,bit)     (((byte) & (1UL << (bit))) >> (bit))
#define SET_DataBIT(byte, bit)   ((byte)  |= (1UL << (bit)))
#define CLEAR_DataBIT(byte,bit)  ((byte)  &= ~(1UL << (bit)))

uint8_t DiffManchester_SendByte   ( uint8_t byte, uint8_t lastEnd);
void    DiffManchester_SendData   (const uint8_t* data, uint8_t bytes);

void    DiffManchester_EnableRead ( uint8_t True );
void    DiffManchester_WaitForRead( void );
void    DiffManchester_ReadBit    ( void );
uint8_t DiffManchester_GetData    ( uint8_t data[CODE_BYTELENGTH], uint8_t dataLens );



#endif /* INC_MANCHESTERCODE_H_ */
