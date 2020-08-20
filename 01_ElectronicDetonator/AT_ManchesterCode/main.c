#include "ManchesterInit.h"
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
    delay_ms(500);

    uint8_t data[25] = {0XAA, 0x37, 0x33, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x01,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEE, 0xEE};

    
    //DiffManchester_WaitForRead(  );
    if( 0 == DiffManchester_GetData( data, 1 ))
    {
      //data[dataLens-1] = i++;
      DiffManchester_SendData( data, 1); 
      //DiffManchester_SendByte   ( 0x00, 0 );       
    }
    else
    {
      DiffManchester_SendByte   ( 0xEE, 0 );   
    }
     
    //DiffManchester_SendData( data, 25); 
  
  }
  
  return 0;
}
