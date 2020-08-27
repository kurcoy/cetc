#include "ManchesterInit.h"
#include "ManchesterCode.h"
#include <ioavr.h>
//CPU frequency = 8MHz, CLKio=1MHz

void main( void )
{
  initRX ( );
  initTX ( );
  
  SET_BIT(TX_PORT,TX_PIN);
  delay_ms(1000);
  CLEAR_BIT(TX_PORT,TX_PIN);

  startRX( );

  uint8_t data[25];
  uint8_t error=1;
  uint8_t i = 0; 
  while( 1 )
  {     
    uint8_t datasend[25] = {0XAA, 0xEE, 0xCC, 0xCC, 0xDD, 0xDD, 0xEE, 0xEE, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xAA, 0xBB,
			    0xCC, 0xDD, 0xEE, 0xFF, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xAA};
    
    DiffManchester_WaitForRead(  );
    error = DiffManchester_GetData( data, CODE_BYTELENGTH );
    error = 0;
    
    stopRX( );
    delay_ms(500);
  
    if(error ==0 )
    { 
      data[24] = i++;
      DiffManchester_SendData( data, CODE_BYTELENGTH); 
    }
    else
    {
      DiffManchester_SendData ( datasend, CODE_BYTELENGTH );   
    }
 
    startRX();
  }
}
