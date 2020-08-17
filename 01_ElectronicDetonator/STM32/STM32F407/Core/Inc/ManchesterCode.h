/*
 * ManchesterCode.h
 *
 *  Created on: 2020-8-14
 *      Author: Cliff
 */
#include "stm32f4xx_hal.h"

#ifndef INC_MANCHESTERCODE_H_
#define INC_MANCHESTERCODE_H_

/* 2500 bits per second*/
#define BAUD_RATE 2000
/* The time for one bit in 碌s/cycles */
#define BIT_TIME  500
#define HALF_TIME 250
/* The time between samples in 碌s/cycles */
#define SAMPLE_TIME 125
/* The number of sample per data unit */
#define DATA_SAMPLE 32

#define IS_IOSET(GPIO,GPIO_PIN)  HAL_GPIO_ReadPin(GPIO,GPIO_PIN)
#define SET_IO(GPIO,GPIO_PIN)    HAL_GPIO_WritePin(GPIO,GPIO_PIN,GPIO_PIN_SET)
#define CLEAR_IO(GPIO,GPIO_PIN)  HAL_GPIO_WritePin(GPIO,GPIO_PIN,GPIO_PIN_RESET)

#define IS_DataSET(byte,bit)         (((byte) & (1UL << (bit))) >> (bit))
#define SET_DataBIT(byte, bit)       ((byte)  |= (1 << (bit)))
#define CLEAR_DataBIT(byte,bit)      ((byte)  &= ~(1 << (bit)))

#define RX_PORT GPIOA
#define RX_PIN  GPIO_PIN_5

#define TX_PORT GPIOE
#define TX_PIN  GPIO_PIN_0

#define HIGH 0x03
#define LOW  0x0C

uint8_t DiffManchester_SendByte   ( uint8_t byte, uint8_t lastEnd);
void    DiffManchester_SendData   (const uint8_t* data, uint16_t bytes);

void    DiffManchester_EnableRead ( uint8_t True );
void    DiffManchester_WaitForRead( void );
void    DiffManchester_ReadBit    ( void );
uint8_t DiffManchester_GetData    ( uint8_t **data, uint8_t dataLens );

extern  void delay_us(uint16_t us);

#endif /* INC_MANCHESTERCODE_H_ */
