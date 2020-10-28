#include "ManchesterInit.h"
#include "ManchesterCode.h"
//#include <ioavr.h>
//CPU frequency = 8MHz, CLKio=1MHz

int main( void )
{
  initRX ( );
  initTX ( );
  
  //startRX( );
  //SET_BIT(TX_PORT,TX_PIN);
  //delay_ms(100);
  //CLEAR_BIT(TX_PORT,TX_PIN);
  /*
  SET_BIT(TX_PORT,TX_PIN); 
  uint8_t lastEnd = 1;
  delay_ms(5000);
  CLEAR_BIT(TX_PORT,TX_PIN);
  */
  
  //stopRX( );
  while( 1 )
  {     

	uint8_t datasend[25] = {0XAA, 0xEE, 0xCC, 0xCC, 0xDD, 0xDD, 0xEE, 0xEE, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xAA, 0xBB,
			    0xCC, 0xDD, 0xEE, 0xFF, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xAA};
    uint8_t data[25];
    volatile uint8_t error=1;
    data[0] = 0xBB;
    //DiffManchester_WaitForRead(  );
    
    //error = DiffManchester_GetData( data, 1 );

	
    //stopRX( );
    //delay_ms(150);
    
  SET_BIT(TX_PORT,TX_PIN);
  //delay_us(1);
  //CLEAR_BIT(TX_PORT,TX_PIN);
	
    if(error ==0 )
    { 
	  //DiffManchester_SendData( data, 1); 
    }
    else
    {
      //DiffManchester_SendData ( datasend, 1 );   
    }
    //startRX();
  }
  
  return 0;
}
