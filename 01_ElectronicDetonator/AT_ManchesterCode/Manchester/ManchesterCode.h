
//  Created by Peter Goldsborough on 02/06/14.
//  Copyright (c) 2014 Peter Goldsborough. All rights reserved.
//
#ifndef __ManchesterCode__
#define __ManchesterCode__
#include <stdint.h>
#include <intrinsics.h>

/******************************Global definition******************************/
#define ENABLE_BIT_DEFINITIONS

#define CPU_F ((double)8000000)
#define delay_us(x) __delay_cycles((unsigned long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((unsigned long)(CPU_F*(double)x/1000.0))

/* 2500 bits per second*/
#define BAUD_RATE 2000

/* The time for one bit in µs/cycles */
#define BIT_TIME  500

#define HALF_TIME 250

/* The time between samples in µs/cycles */
#define SAMPLE_TIME 125

/* The number of sample per data unit */
#define DATA_SAMPLE 32

#define SET_BIT(byte, bit) ((byte) |= (1UL << (bit)))

#define CLEAR_BIT(byte,bit) ((byte) &= ~(1UL << (bit)))

#define IS_SET(byte,bit) (((byte) & (1UL << (bit))) >> (bit))

#define RX_PORT PINB
#define RX_PIN  PB1

#define TX_PORT PORTB
#define TX_PIN  PB0

#define HIGH 0x03
#define LOW  0x0C

#define CONNECTION_TIME 77

/******************************func definition******************************/
void initRX(void);
void startRX(void);
void stopRX(void);

void initTX(void);
void Manchester_sendByte(const uint8_t byte);
void Manchester_sendData(const uint8_t* data, uint16_t bytes);

uint8_t  DiffManchester_sendByte( uint8_t byte, uint8_t lastEnd);
void     DiffManchester_sendData(const uint8_t* data, uint16_t bytes);

uint8_t  DiffManchester_GetData( uint8_t **data, uint8_t dataLens );

#endif
