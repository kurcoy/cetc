/*
 * ManchesterInit.c
 *
 *  Created on: 2020-8-17 *      Author: Cliff
 */

#include "ManchesterInit.h"
#include "ManchesterCode.h"


/***********************************RX*************************************/
/***********************************RX*************************************/
void initRX(void)
{
  OSCCON = 0x7A;       // 8 Mhz oscillator.0b01110010 0x72 F2 32MHz
  ANSELA = 0;          // Analog off.
  LATA = 0;            // Zero all port bits.
  
  TRISAbits.TRISA1 = 1;// set RA1 as input
  
  INTCONbits.GIE = 0;
  
  INLVLAbits.INLVLA1 = 0;
  OPTION_REGbits.nWPUEN = 0; //Weak pull-ups are enabled by individual WPUx latch values
  WPUA = 0x02;
  //SLRCONAbits.SLRA1 = 0; 
  
  //INTCONbits.IOCIE = 1; // enable Interrupt-On-Change interrupt
  IOCAP=0x02; // enabled on RA1 pin for a positive going edge;
  IOCAN=0x02; // enabled on RA1 pin for a negative going edge;
  
  //PIE1bits.TMR2IE = 0;
  T2CONbits.TMR2ON = 1;
  T2CONbits.T2CKPS = 0x01;//ps 1:16
  PR2 = 0x7D;        
  //OPTION_REGbits.TMR0CS = 0; //enable timer0 timer mode 
  //OPTION_REGbits.PSA = 0;
  //OPTION_REGbits.PS = 0x00; //pre-scale 1:2 + FOSC/4 = every 1us
  
  T1CONbits.TMR1CS = 0; //Fosc=16MHz, Ftimer1=4Mhz
  
  
  INTCONbits.PEIE = 1;
  INTCONbits.GIE = 1;   // enable all interrupts       
  

}

void startRX(void)
{
  /* Enable sampling Timer/Counter and interrupts */
    TMR2 = 0;
	TMR2IF=0; 
    IOCAF1= 0;
    //INTCONbits.GIE = 1;  // enable all interrupts
    INTCONbits.IOCIE = 1;   
    PIE1bits.TMR2IE = 1;    
}

void stopRX(void)
{
  /* Disable sampling Timer/Counter */  
  DiffManchester_EnableRead ( 0 );     
  //INTCONbits.TMR0IE = 0;// disable timer0 interrupts 
  //INTCONbits.GIE = 0;  // disable all interrupts
  PIE1bits.TMR2IE = 0;
  INTCONbits.IOCIE = 0;  
}
void initTX(void)
{
  /* Set TX_PIN as output */
  TRISAbits.TRISA2 = 0;// set RA2 as output
}

void initCal(void)
{
  //Set timer1 as calibration base 
  TMR1H = 0; 
  TMR1L = 0;
  T1CONbits.TMR1ON = 1; //timer1 enable
  T1CONbits.TMR1CS = 0; //Fosc=16MHz, Ftimer1=4Mhz
  //T1CONbits.T1CKPS    //PS
  
  //INTCONbits.INTE = 1;  //
  //OPTION_REGbits.INTEDG = 1; //rising edge
  
  /*
  PIE1bits.TMR1GIE = 0;   
  PIE1bits.TMR1IE = 0;   
  T1GCONbits.TMR1GE = 1; //gate enable

  T1GCONbits.T1GPOL = 1; //gate polarity, rising edge
  T1GCONbits.T1GSS = 0;  //gate source
   * 
  T1GCONbits.T1GSPM = 1; //single pulse mode  
  T1GCONbits.T1GGO_nDONE = 1;
  */
//T1GCONbits.T1GVAL gate status
//TMR1GIF ???
//TMR1H:TMR1L
  
}

void StopCal(void)
{
  T1CONbits.TMR1ON = 0; //timer1 enable
  //T1GCONbits.T1GSPM = 0;
  //T1GCONbits.T1GGO_nDONE = 0;
  //
} 

uint16_t cal = 0;
void TimerCal(void)
{
    if( cal == 0 )
    {
        initCal( ); 
        cal = 1;      
        //SET_IO  (TX_PORT,TX_PIN);
    }
    else
    {
        StopCal();
        //INTCONbits.INTE = 0;
        cal = (TMR1H<<8) | (TMR1L);
        uint8_t data[3];
        data[0] = TMR1H;
        data[1] = TMR1L;
        data[2] = cal/10;
        DiffManchester_SendData( data, 3); 
        
        stopRX();

        IOCAF1= 0; 
    }    
}

void getCalData( uint8_t data[CODE_BYTELENGTH] )
{
    data[0] = TMR1L; 
    data[1] = TMR1H; 
    //INTCONbits.INTE = 1;
}
void delay_us_bytimer(uint16_t us)
{
    uint16_t differ=0xffff-us*4-5;// 4*90%
    T1CONbits.TMR1ON = 1; //timer1 enableTMR1H = 0; 	
    TMR1H = differ >>8; 
    TMR1L = differ && 0x00FF; 

	while(differ < 0xffff-5)
	{
		differ = (TMR1H<<8) | (TMR1L);
	}
	T1CONbits.TMR1ON = 0; //timer1 disable
}
/***********************************TX*************************************/
/***********************************TX*************************************/
uint8_t count = 0;
void __interrupt() isr(void)
{
    if(PIE1bits.TMR2IE && TMR2IF) //TMR0IF
    {  
                 
       //SET_IO(TX_PORT,TX_PIN); 
       //CLEAR_IO(TX_PORT,TX_PIN);
        TMR2IF=0; 
        TMR2 =10; // any timer 0 interrupts?
        DiffManchester_ReadBit( );
        if (! count--) PIE1bits.TMR2IE = 0;

        
    }
 
    if (INTCONbits.IOCIE && IOCAF1 ) //IOCIF
    {            
       //TimerCal();
       //PIE1bits.TMR2IE = 0;  
       TMR2IF=0; 
       IOCAF1= 0; // any INTERRUPT-ON-CHANGE on RA1
       DiffManchester_EnableRead( 1 ); 
       if( IS_IOSET(RX_PORT,RX_PIN) ) TMR2 =45; 
       else                           TMR2 =100; 
       //TMR2 =80; //170
       count = 3;
       PIE1bits.TMR2IE = 1;
    }

    //if(TMR1GIE && TMR1GIF)
    //if( INTCONbits.INTE && INTF )
}

