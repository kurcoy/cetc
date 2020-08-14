//==============================================================================
//
// Title:		ComPort.c
// Purpose:		A short description of the implementation.
//
// Created on:	2020/6/9 at 9:45:32 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files
#include "ComPort.h"   
#include <rs232.h>
#include <ansi_c.h>


//==============================================================================
// Constants

//==============================================================================
// Types
typedef struct
{
  int    DevID; 
  char   Name[50];
  int    ComID;
  int    BaudRate;
	int    TimeOut;
  
  int    ThreadID;
  int    ThreadIsRunning;
  FLAG   Update;
  FLAG   IsOpen;
	
  int    ReadBufferSize;
  int    WriteBufferSize;
  unsigned char *QueryCommand;
  unsigned char *ReadBuffer;
  
	int    Type;						//reserved    
  int    NumOfReadValue;  //reserved 
  double *OffsetValue;		//reserved 
  double *ReadValue;			//reserved 
  
}
COMPORTDATA;
//==============================================================================
// Static global variables
static int SysModuleID;
static COMPORTDATA *ComPortList;
static int NumOfComPort;
//==============================================================================
// Static functions
static int SensorReadFunction             ( void *funcData );    
RETVAL COMPORT_ReadBuffer 							  ( int  DevID     );      
//==============================================================================
// Global variables

//==============================================================================
// Global functions
typedef enum
{
  ERR_INST_MODULE,
  ERR_NAMEPARA,
	ERR_PARAM,
	ERR_PORTNUM,
	ERR_INVALIDNAME,
	ERR_INVALIDDEV,
	ERR_INVALIDBAUD,
	ERR_CREATETHREAD,
	ERR_NOUPDATE,
	ERR_SENSORSIZE,
}
ERROR_NRS;

static RETIDX ErrorListing ( MSGTYPE Type, int numError, char *addInfoText )                                           
{                                                                                                                          
  char *MessageText = "End of List";                                                                                       
                                                                                                                         
  switch( numError )                                                                                                       
  {                                                                                                                        
    case ERR_INST_MODULE:           MessageText = "Installation failed";                                  break;
    case ERR_NAMEPARA:              MessageText = "Invalid name parameter";                               break;
		case ERR_PARAM:                 MessageText = "Invalid input parameter";                              break;                                   
		case ERR_PORTNUM:								MessageText = "Invalid com port number";                              break;
		case ERR_INVALIDNAME:        	  MessageText = "Invalid Dev Name";                  			 						  break; 
		case ERR_INVALIDDEV:            MessageText = "Invalid Dev ID";                                       break;
		case ERR_INVALIDBAUD:        	  MessageText = "Invalid Baudrate ";                 									  break;
    case ERR_CREATETHREAD:          MessageText = "Create thread failed";               									break;
    case ERR_NOUPDATE:              MessageText = "ReadBuffer not update";              									break;
    case ERR_SENSORSIZE:            MessageText = "Read Value Number Input Invalid";    									break;
		
    default:                        numError    = -1;                                                     break;                                      
  }                                                                                                                        
	char info[200]; 
	strcpy( info, addInfoText ); 
	strcat( info, "_" );      
	strcat( info, MessageText);
	
  SYSTEM_GenerateMessage ( Type, SysModuleID, numError, info );                                                                                                             
  return numError;                                                                                                         
}
//*********************************************************************************************************************
static RETVAL UnInstall ( int SysModuleID,  int PanelID,   void *funcData )
{
  //int n=100;
  for( int i=0; i<NumOfComPort; i++ )
  {
    int n=100;
    COMPORTDATA *COMSD = &ComPortList[i];
    COMSD->ThreadIsRunning = 0; 
    while( -1 != COMSD->ThreadIsRunning && n-- ) SYSTEM_SleepTime( 5 ); 
    if( COMSD->ReadBuffer  ) free(COMSD->ReadBuffer);  COMSD->ReadBuffer  = 0;
    if( COMSD->ReadValue )   free(COMSD->ReadValue);   COMSD->ReadValue   = 0;   
    if( COMSD->OffsetValue ) free(COMSD->OffsetValue); COMSD->OffsetValue = 0;   
  }
  
  if( ComPortList ) { free( ComPortList ); ComPortList = 0; }
  
  NumOfComPort = 0; 
  
  return RET_ERROR;
}
/**************************************************************************************************************/
RETVAL COMPORT_Install           ( int _SysModuleID, int PanelID, void *funcData )
{
  char **DeviceNameList = funcData; 
  SysModuleID = _SysModuleID;
 
  SYSTEM_SetSysModuleUninstFunc  ( SysModuleID, UnInstall, 0 );                                          
  SYSTEM_SetSysModuleErrListFunc ( SysModuleID, ErrorListing );     
  
  if( DeviceNameList!=0 && DeviceNameList[0]!=0 )
  {
    while( *DeviceNameList[NumOfComPort] )   
    {                                          
      NumOfComPort++;
    }                                      
    if ( ComPortList = calloc ( NumOfComPort, sizeof( COMPORTDATA ) ))
    for( int i=0; i<NumOfComPort; i++ )
    {
      COMPORTDATA *COMSD = &ComPortList[i];
      COMSD->DevID = i+1;
      COMSD->ComID = -1;
      strcpy ( COMSD->Name, DeviceNameList[i] );
    }
    return RET_NOERR;
  }
  
  ErrorListing ( MSG_ERROR, ERR_INST_MODULE, " " );  
  return RET_ERROR;
}
//*****************************************************************************************************************************************
static RETVAL GetComPortDataByName ( char *DevName, COMPORTDATA **ComPort )
{
  int error = ERR_INVALIDNAME;
  if( NULL != DevName )
  for( int i=0; i<NumOfComPort; i++ )
  {
    COMPORTDATA *COMSD = &ComPortList[i];
    if( 0 == strcmp( COMSD->Name, DevName ))
    {
      *ComPort = COMSD;
      return RET_NOERR;
    }
  }
	
  ErrorListing ( MSG_ERROR, error, DevName );
  return RET_ERROR;
}
//*****************************************************************************************************************************************
static RETVAL GetSensorDataByDevID ( int DevID, COMPORTDATA **ComPort )
{
  int error = ERR_INVALIDDEV;
  if( DevID > 0 && DevID <= NumOfComPort ) 
  {
    *ComPort = &ComPortList[DevID-1];    
    return RET_NOERR;
    
  }
  
  ErrorListing ( MSG_ERROR, error, "" );
  return RET_ERROR;
}
/**************************************************************************************************************/ 
RETVAL  COMPORT_Open  ( int DevID  )   
{
  int error=-1;
  
  COMPORTDATA *COMSD;
  if((error=ERR_INVALIDNAME) && RET_NOERR == GetSensorDataByDevID ( DevID, &COMSD ))
  if(!(error=0) && COMSD->ThreadIsRunning > 0)
	if( (error=ERR_PORTNUM) && 0 == OpenComConfig ( COMSD->ComID, "" ,  COMSD->BaudRate, 0, 8, 1, COMSD->ReadBufferSize, COMSD->WriteBufferSize ))	 //COMSD->Name
	if( 0 == SetComTime 	 (  COMSD->ComID, 1 ))
	if( COMSD->QueryCommand  = calloc   ( COMSD->WriteBufferSize+1, sizeof(char)))  
	if( COMSD->ReadBuffer    = calloc   ( COMSD->ReadBufferSize+1,  sizeof(char))) 
	if( RET_NOERR == COMPORT_ReadBuffer ( COMSD->DevID ))   	
	{

			return RET_NOERR;

	}
	
  ErrorListing ( MSG_ERROR, error, " Open" );
  return RET_ERROR;
}
/**************************************************************************************************************/ 
RETID  COMPORT_Init             ( char *DevName, int ComID, int Baudrate, int ReadBufferSize, int WriteBufferSize ) 
{
  int error;
  COMPORTDATA *COMSD;
  if((error=ERR_INVALIDNAME) && RET_NOERR == GetComPortDataByName ( DevName, &COMSD ))
  if((error=ERR_INVALIDBAUD) && Baudrate > 0 )
	if((error=ERR_PARAM) && ReadBufferSize >=0 && WriteBufferSize >= 0 )     	
  {
    error=0;
		//if( FLAG_TRUE == COMSD->IsOpen && ComID == COMSD->ComID ) return 
		
    COMSD->ComID           = ComID; 
    COMSD->ThreadIsRunning = 1;
    COMSD->Update          = FLAG_TRUE;

    COMSD->ReadBufferSize  = ReadBufferSize; 
    COMSD->WriteBufferSize = WriteBufferSize;
    COMSD->NumOfReadValue  = 2;
		COMSD->BaudRate        = Baudrate;    
		
		COMSD->TimeOut         = 500;//s
		
		//sprintf( COMSD->Name, "COM%d", ComID );
		
		//if( (error=ERR_PORTNUM) && 0 == OpenComConfig ( ComID, portName, Baudrate, 0, 8, 1, COMSD->ReadBufferSize, COMSD->WriteBufferSize ))
		//if( 0 == SetComTime 	 ( ComID, 1 ))
    
		//if( COMSD->QueryCommand  = calloc   ( COMSD->WriteBufferSize+1, sizeof(char)))  
		//if( COMSD->ReadBuffer    = calloc   ( COMSD->ReadBufferSize+1,  sizeof(char))) 
		//if( RET_NOERR == COMPORT_ReadBuffer ( COMSD->DevID ))   
			 
    //if( COMSD->ReadValue     = calloc             ( COMSD->NumOfReadValue,  sizeof(double)))
    //if( COMSD->OffsetValue   = calloc             ( COMSD->NumOfReadValue,  sizeof(double)))
    
   
    //if( (error=ERR_CREATETHREAD) && RET_NOERR == SYSTEM_CreateThreadIdentifier  ( SysModuleID, &COMSD->ThreadID ))
    //if( RET_NOERR == SYSTEM_CreateThread            ( COMSD->ThreadID, SensorReadFunction, &COMSD->DevID )) 
    {
      error=0;
    	COMSD->IsOpen = FLAG_TRUE;
      return COMSD->DevID;
    }
  
  }

  ErrorListing ( MSG_ERROR, error, "Init" );
  return -1;
}
//*****************************************************************************************************************************************  
RETVAL COMPORT_Close ( int DevID  ) 
{
  int error=-1;
  
  COMPORTDATA *COMSD;
  if((error=ERR_INVALIDNAME) && RET_NOERR == GetSensorDataByDevID ( DevID, &COMSD ))
  if(!(error=0) && COMSD->ThreadIsRunning > 0)
	{
		error=-1;
		if( 0 == CloseCom ( COMSD->ComID )) 
		{
			//COMSD->ThreadIsRunning = 0;
			return RET_NOERR;
		}
	}
  ErrorListing ( MSG_ERROR, error, " Close" );
  return RET_ERROR;
}	
//*****************************************************************************************************************************************
static int SensorReadFunction             ( void *funcData )
{
  int error=-1;
  int *DevID = funcData;
  
  COMPORTDATA *COMSD;                                                          
  if((error=ERR_INVALIDNAME) && RET_NOERR == GetSensorDataByDevID ( *DevID, &COMSD ))
  while(COMSD->ThreadIsRunning > 0)
  {
    if( RET_NOERR == COMPORT_ReadBuffer   ( *DevID )) 
    if( RET_NOERR == SYSTEM_SleepTime     ( 25      ))  
    {
      continue;
    }
    break;
  }
  
  COMSD->ThreadIsRunning = -1;
  return 1;
}
//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************
static int writeComPort( int DevID )
{
	int error=-1;
  
  COMPORTDATA *COMSD;
  if((error=ERR_INVALIDNAME) && RET_NOERR == GetSensorDataByDevID ( DevID, &COMSD ))
  if(!(error=0) && COMSD->ThreadIsRunning > 0)
	{
		error=0;  
		if( COMSD->QueryCommand )
		{
			error=-1;
			//if( 0 <= ComWrt   ( COMSD->ComID, COMSD->QueryCommand, COMSD->WriteBufferSize )) error=0; 
		int a = ComWrt   ( COMSD->ComID, COMSD->QueryCommand, COMSD->WriteBufferSize ); error=0; 
		}
	}
	return error;
}
//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************
RETVAL COMPORT_ReadBuffer ( int DevID  ) 
{
  int error=-1;
  
  COMPORTDATA *COMSD;
  if((error=ERR_INVALIDNAME) && RET_NOERR == GetSensorDataByDevID ( DevID, &COMSD ))
  if(!(error=0) && COMSD->ThreadIsRunning > 0)
  if( (error=-1)             && NULL      != memset( COMSD->ReadBuffer, NULL,  (COMSD->ReadBufferSize)*sizeof(char)))
      
  if( 0 == FlushInQ ( COMSD->ComID))
	if( 0 == writeComPort ( DevID ))
	//if( 0 <= ComWrt   ( COMSD->ComID, COMSD->QueryCommand, COMSD->WriteBufferSize ))
  if( RET_NOERR ==  SYSTEM_SleepTime       ( 200 )) 
	{}
	int a=GetInQLen (  COMSD->ComID);
	int b=GetOutQLen ( COMSD->ComID);
	
	if( 0 <= ComRd    ( COMSD->ComID, COMSD->ReadBuffer, COMSD->ReadBufferSize ))   
  {
    error=0;
    COMSD->ReadBuffer[COMSD->ReadBufferSize]= '\0'; // add the ending character into the string   
    COMSD->Update = FLAG_TRUE; 
  }
      
  if(!error) return RET_NOERR;

  ErrorListing ( MSG_ERROR, error, " ReadFunction" );
  return RET_ERROR;
}
/**************************************************************************************************************/
RETVAL COMPORT_Read ( int DevID, double *ReadValue, int NumOfReadValue )
{
  int error=-1;
  COMPORTDATA *COMSD;
  if( RET_NOERR == GetSensorDataByDevID   ( DevID, &COMSD ))
  if( (error=ERR_SENSORSIZE) && NumOfReadValue == COMSD->NumOfReadValue ) 
  if( (error=ERR_NOUPDATE)   && COMSD->ThreadIsRunning > 0 )
  {
      for( int i=0; i < NumOfReadValue; i++ )
      {
        if( &ReadValue[i] ) ReadValue[i] = (COMSD->ReadValue)[i] - (COMSD->OffsetValue)[i]; 
      }
      
      COMSD->Update = FLAG_FALSE;
      
      return RET_NOERR;
  }
  
  ErrorListing ( MSG_ERROR, error, "COMPORT_Read" );
  return RET_ERROR; 
}
/**************************************************************************************************************/    
/**************************************************************************************************************/
RETVAL COMPORT_GetReadBuffer ( int DevID, char *ReadBuffer )
{
  int error=-1;
  COMPORTDATA *COMSD;
	if( (error=ERR_NAMEPARA) && NULL != ReadBuffer )
  if( RET_NOERR == GetSensorDataByDevID   ( DevID, &COMSD ))
  if( (error=ERR_NOUPDATE)   && COMSD->ThreadIsRunning > 0 )
  {
    memcpy( ReadBuffer , COMSD->ReadBuffer, COMSD->ReadBufferSize );
    COMSD->Update = FLAG_FALSE;
    
    return RET_NOERR;
  }

  ErrorListing ( MSG_ERROR, error, "COMPORT_Read" );
  return RET_ERROR; 
}
/**************************************************************************************************************/    
/**************************************************************************************************************/
RETVAL COMPORT_WriteBuffer       ( int DevID, char *WriteBuffer, int WriteSize )
{
	int error=-1;
  COMPORTDATA *COMSD;
	if( (error=ERR_NAMEPARA) && NULL != WriteBuffer )
  if( RET_NOERR == GetSensorDataByDevID   ( DevID, &COMSD ))
  if( (error=ERR_NOUPDATE)   && COMSD->ThreadIsRunning > 0 )
	{
		memcpy( COMSD->QueryCommand, WriteBuffer, WriteSize ); 
		COMSD->WriteBufferSize = WriteSize;	
	  if( RET_NOERR == COMPORT_ReadBuffer ( DevID  ))
		{
			return RET_NOERR;	
		}
	}
	
	ErrorListing ( MSG_ERROR, error, "WriteBuffer" );
  return RET_ERROR; 
}
