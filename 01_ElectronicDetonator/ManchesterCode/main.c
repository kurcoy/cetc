/* 
 * File:   main.c
 * Author: Cliff
 *
 * Created on December 14, 2020, 1:43 PM
 */

/*
 * PIC12F1571 
 *
 * Pins:
 *                     +----+
 *                Vdd -|1  8|- Vss
 *           PWM1 RA5 -|2  7|- RA0/ICSPDAT
 *           PWM2 RA4 -|3  6|- RA1/ICSPLCK    X9 DETECT
 *           RA3/MCLR -|4  5|- RA2 PWM3       X8 ACK
 *                     +----+
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include"Manchester/ManchesterCode.h" 
#include"Manchester/ManchesterInit.h"

//#define _XTAL_FREQ 08000000   // Oscillator frequency 8MHz.

#pragma config FOSC = INTOSC  // INTOSC oscillator: I/O function on CLKIN pin.
#pragma config WDTE = OFF     // Watchdog Timer disable.
#pragma config PWRTE = OFF    // Power-up Timer enbable. 
#pragma config MCLRE = OFF     // MCLR/VPP pin function is MCLR. ON 
#pragma config CP = OFF       // Program memory code protection disabled.
#pragma config BOREN = OFF     // Brown-out Reset enabled.
#pragma config CLKOUTEN = OFF // CLKOUT function is disabled; I/O or oscillator function on the CLKOUT pin.
#pragma config WRT = OFF      // Flash Memory Write protection off.
#pragma config STVREN = OFF    // Stack Overflow or Underflow will cause a Reset.
#pragma config BORV = LO      // Brown-out Reset Voltage (Vbor), low trip point selected.
#pragma config LVP = OFF      // High-voltage on MCLR/VPP must be used for programming.

int main() 
{
  initRX ( );
  initTX ( );
  
  //SET_IO(TX_PORT,TX_PIN);    
  //delay_ms(10);
  //CLEAR_IO(TX_PORT,TX_PIN);

  uint8_t data[25];
  uint8_t error=1;
   
  startRX( );
  //uint8_t datasend[25] = {0XAB, 0xBE, 0xCC, 0x9C, 0x8D, 0x7D, 0x6E, 0x5E, 0x4A, 0x3B, 0x2C, 0x1D, 0xEE, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xAA}; 
  uint8_t datasend[1] = {0XAB}; 
  while( 1 )
  {  
    //CLEAR_IO(TX_PORT,TX_PIN);
    //delay_ms(500); 
    //SET_IO(TX_PORT,TX_PIN);   

    DiffManchester_WaitForRead(  );
    error = DiffManchester_GetData( data, 25);

    stopRX( );
    
    delay_ms(500);     
    if(error ==0 )
    { 
      DiffManchester_SendData( data, 25); 
    }
    else
    {
        DiffManchester_SendData (datasend,1);   
    }

    startRX(); 
  }

  return (EXIT_SUCCESS);
}