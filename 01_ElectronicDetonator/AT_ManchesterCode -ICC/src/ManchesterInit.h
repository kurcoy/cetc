
//  Created by Peter Goldsborough on 02/06/14.
//  Copyright (c) 2014 Peter Goldsborough. All rights reserved.
//
#ifndef __ManchesterINIT__
#define __ManchesterINIT__
//#include <stdint.h>
//#include <intrinsics.h>

#include <iot45v.h>
#include <macros.h>

/******************************Global definition******************************/
//#define ENABLE_BIT_DEFINITIONS

#define IS_SET(byte,bit)     (((byte) & (1UL << (bit))) >> (bit))
#define SET_BIT(byte, bit)   ((byte)  |= (1UL  << (bit)))
#define CLEAR_BIT(byte,bit)  ((byte)  &= ~(1UL  << (bit)))

/******************************func definition******************************/
void initRX(void);
void startRX(void);
void stopRX(void);
void initTX(void);

void delay_us( unsigned int x );
void delay_ms( unsigned int x);
#endif
