/*
 * ManchesterInit.c
 *
 *  Created on: 2020-8-17 *      Author: Cliff
 */

#ifndef __ManchesterINIT__
#define __ManchesterINIT__
#include <stdint.h>
#include <xc.h>
 
/******************************Global definition******************************/
//#define ENABLE_BIT_DEFINITIONS

#define RX_PORT PORTA
#define RX_PIN  _PORTA_RA1_POSITION

#define TX_PORT PORTA
#define TX_PIN  _PORTA_RA2_POSITION

#define _XTAL_FREQ (16000000)   // Oscillator frequency 16MHz.
#define delay_us(x) __delay_us(x)
#define delay_ms(x) __delay_ms(x)
void delay_us_bytimer(uint16_t us);
/******************************function definition******************************/
void initRX(void);
void startRX(void);
void stopRX(void);
void initTX(void);
void getCalData( uint8_t data[2] );

#endif
