#include "ManchesterInit.h"
#include "ManchesterCode.h"
#include <ioavr.h>
//CPU frequency = 8MHz, CLKio=1MHz

int main( void )
{
  initRX ( );
  initTX ( );
  
  SET_BIT(TX_PORT,TX_PIN);
  delay_ms(1000);
  CLEAR_BIT(TX_PORT,TX_PIN);

  startRX( );

  uint8_t data[25];
  uint8_t error=1;
  uint8_t lens = 25;  
  while( 1 )
  {     
    uint8_t datasend[25] = {0XAA, 0xEE, 0xCC, 0xCC, 0xDD, 0xDD, 0xEE, 0xEE, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xAA, 0xBB,
			    0xCC, 0xDD, 0xEE, 0xFF, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xAA};
    
    DiffManchester_WaitForRead(  );
    error = DiffManchester_GetData( data, lens );
    error = 0;
    
    stopRX( );
    delay_ms(500);
    uint8_t data1[1]=0xAA;
    //DiffManchester_SendByte(data1, 0);
  
    if(error ==0 )
    { 
      DiffManchester_SendData( data, lens); 
    }
    else
    {
      datasend[24] = 0XBB;
      DiffManchester_SendData ( datasend, lens );   
    }
 
    startRX();
  }
  
  return 0;
}
