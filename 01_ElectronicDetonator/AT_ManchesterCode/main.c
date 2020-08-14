#include "ManchesterCode.h"
#include <ioavr.h>
//CPU frequency = 8MHz, CLKio=1MHz

int main( void )
{
  initRX ( );
  initTX ( );
  
  //SET_BIT(TX_PORT,TX_PIN);
  //delay_ms(1000);
  CLEAR_BIT(TX_PORT,TX_PIN);
  //startRX( );
  //SET_BIT(TX_PORT,TX_PIN); 
  
  uint8_t lastEnd = 1;
  delay_ms(5000);
  
  while( 1 )
  {
    //SET_BIT(TX_PORT,TX_PIN);
    delay_ms(500);
    //CLEAR_BIT(TX_PORT,TX_PIN);
    const uint8_t databyte = 0xE9;
    //Manchester_sendByte(data);
    uint8_t data[25] = {0XFF, 0x37, 0x33, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x01,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEE, 0xAA};
    //{0xE9, 0x16, 0x3c};
    //uint8_t *P = &data;
    //if( DiffManchester_GetData( &P, 1 ) ) ; 
    //DiffManchester_sendByte( databyte, 1);
    /*
    lastEnd = 1;
    for( int i=0; i<2; i++ )
    {
      lastEnd = DiffManchester_sendByte( data[i], lastEnd); 
    }
    */
    DiffManchester_sendData( data, 25); 
    //Manchester_sendByte(databyte);
    
  
  }
  
  return 0;
}
