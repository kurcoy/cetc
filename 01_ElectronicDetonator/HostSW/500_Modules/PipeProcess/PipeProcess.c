//==============================================================================
//
// Title:		PipeProcess.c
// Purpose:		A short description of the implementation.
//
// Created on:	2020/4/14 at 9:04:59 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include "PipeProcess.h"
#include <windows.h>
#include <ansi_c.h>
#include <stdio.h>

//==============================================================================
// Constants

//==============================================================================
// Types
typedef struct
{
	int  OwnID; 
	HANDLE Handle;
	char Name[50]; 
	int  OpenFlag;
	int  WriteFlag; 
	int  ReadFlag;
	
	int  BytesWriten; 
	int  BytesRead; 
	char WriteBuffer[500];  //buffersize pending
	char ReadBuffer [500];	//buffersize pending  
	
	int  ExecStep; 
	int  ThreadID;
	int  ThreadRunning;
	
}PIPE_INFO,*pPIPE_INFO;

typedef enum
{
	PIPE_OPEN=0,
	PIPE_READ=1, 
	PIPE_WRITE=2, 
	PIPE_IDLE=3,
	PIPE_ERROR,
}
PIPE_STASTUS;

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables
static int SysModuleID; 
static PIPE_INFO *PipeList; 
static int NumofPipes; 
//==============================================================================
// Global functions
static int PipeExecution ( void *funcData );

/************************************************************************************************/
/************************************************************************************************/ 
typedef enum
{
  PIPEERR_INST_MODULE=1,
	PIPEERR_PARAM,
	PIPEERR_INVALID,   
	PIPEERR_CREATE,
	PIPEERR_OPEN,
	PIPEERR_READ,
	PIPEERR_WRITE,
	PIPEERR_TMEOUT,
	PIPEERR_THREAD,
	
	
}
PIPEERR_NRS;             

static RETIDX ErrorListing ( MSGTYPE Type, int numError, char *addInfoText )
{
  char *MessageText = "End of List";
  
  switch( numError )
  {
    case PIPEERR_INST_MODULE:         MessageText = "Installation failed";                break;
    case PIPEERR_PARAM:               MessageText = "Invalid parameter";                  break;
		case PIPEERR_INVALID:						  MessageText = "Invalid Pipe ID";                    break; 
    case PIPEERR_CREATE:              MessageText = "Create NamedPipe failed";            break; 
		case PIPEERR_OPEN:                MessageText = "Connect NamedPipe invalid";          break;
    case PIPEERR_READ:                MessageText = "Read From ProcessPipe failed";       break;  
    case PIPEERR_WRITE:               MessageText = "Write To ProcessPipe failed";        break; 
		case PIPEERR_TMEOUT:              MessageText = "Wait for ProcessPipe timeout";       break;
		case PIPEERR_THREAD:              MessageText = "ProcessPipe thread invalid";         break; 
		
    default:                          numError    = -1;                                   break;
  }
  SYSTEM_GenerateMessage ( Type, SysModuleID, numError, MessageText );

  return numError;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

static RETVAL UnInstall ( int SysModuleID,  int PanelID,   void *funcData )
{
	for( int i=0; i<NumofPipes; i++ )
	{
		PIPE_INFO *PipeData=&PipeList[NumofPipes-1];
		PipeData->ThreadRunning = 0; 
		SYSTEM_SleepTime( 100 ); 
		SYSTEM_TerminateThread ( PipeData->ThreadID );
    PipeData->BytesWriten = 0; PipeData->BytesRead = 0;	
	}
	
	if( PipeList ) { free(PipeList); PipeList = 0; }
	
	NumofPipes=0;  
	return RET_NOERR;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL PROCESSPIPE_Install  ( int _SysModuleID,  int PanelID, void *funcData )
{
	SysModuleID = _SysModuleID; 
	
	SYSTEM_SetSysModuleUninstFunc    ( SysModuleID, UnInstall, 0 );
  SYSTEM_SetSysModuleErrListFunc   ( SysModuleID, ErrorListing );
	
	return RET_NOERR;  
	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETID PROCESSPIPE_Create ( char *PipeName )
{
	HANDLE Handle;
	int error;
	DWORD bufferSize = 4096;
	if( (error=PIPEERR_PARAM) && PipeName != NULL )
	if( error=PIPEERR_CREATE )	
	if((Handle = CreateNamedPipe( PipeName, PIPE_ACCESS_DUPLEX,PIPE_TYPE_MESSAGE|PIPE_READMODE_BYTE,1,0,0,1000,NULL)) != INVALID_HANDLE_VALUE)
	//if( ConnectNamedPipe(Handle,NULL) != 0) //put into another thread
	{
		PIPE_INFO *PipeData;  
		
		if( PipeList = realloc( PipeList, (++NumofPipes) * sizeof(PIPE_INFO) ))
    if( memset( (PipeData=&PipeList[NumofPipes-1]), 0, sizeof(PIPE_INFO) )) 
		{
			PipeData->Handle = Handle; 
			PipeData->OwnID = NumofPipes;
			strcpy(PipeData->Name, PipeName );
			
		  if( RET_NOERR == SYSTEM_CreateThreadIdentifier ( SysModuleID, &PipeData->ThreadID ))  
      if( RET_NOERR == SYSTEM_CreateThread         	 ( PipeData->ThreadID, PipeExecution, &PipeData->OwnID ))  
			if( RET_NOERR == SYSTEM_SleepTime							 ( 100 ))
			if( PipeData->ThreadRunning > 0 )
			{
				return PipeData->OwnID;
			}
		}
	}
	//printf("CreateNamedPipe failed with error %x \n",GetLastError());
	if(Handle) CloseHandle(Handle);
 
	ErrorListing ( MSG_ERROR, error, "" );  
	return -1;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL PROCESSPIPE_WatiForCompletation( int PipeID, FLAG *IsDone, int TimeOut )
{
	int error; 
	if( (error=PIPEERR_INVALID) && PipeID>0 )  
	{ 
		PIPE_INFO *PipeData; 
		PipeData=&PipeList[PipeID-1];

		*IsDone = FLAG_FALSE;   
		int i=TimeOut/10;
		while( (i-- > 0) && (error=PIPEERR_TMEOUT) )
		{
			if( RET_NOERR == SYSTEM_SleepTime(10) )
			if( (error=PIPEERR_THREAD) && PipeData->ThreadRunning ) 
			if( (error=PIPEERR_OPEN) && PipeData->OpenFlag )
			if( !(error=0) && PIPE_IDLE == PipeData->ExecStep )
			if( 0 == PipeData->WriteFlag && 0 == PipeData->ReadFlag )	
			{
				*IsDone = FLAG_TRUE; 
				return RET_NOERR; 
			}
			if(error) break;
		}
		if( PIPEERR_TMEOUT == error || 0 == error ) return RET_NOERR;  
	}
	
	ErrorListing ( MSG_ERROR, error, " IsAvailiable" ); 
	return RET_ERROR; 		
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL PROCESSPIPE_ReadPipe( int PipeID, char* Buffer, int BufferSize, int TimeOut )
{
	int error;
	int BytesRead; 
	
	if( (error=PIPEERR_INVALID) && PipeID>0 )
	if( (error=PIPEERR_PARAM)	&& Buffer != NULL && BufferSize>0 )
	{
		error=-1;
		
		PIPE_INFO *PipeData; 
		PipeData=&PipeList[PipeID-1];

		FLAG IsDone;

		if( PipeData->BytesRead > 0 )	error = 0;

		if(error)
		if( RET_NOERR == PROCESSPIPE_WatiForCompletation( PipeID, &IsDone, TimeOut ))
    if( (error=PIPEERR_TMEOUT) && FLAG_TRUE == IsDone )
		{
			error=0; IsDone = FLAG_FALSE; PipeData->ReadFlag = 1; PipeData->BytesRead = BufferSize; 		
		}
		
		if(0 == error)
		if( RET_NOERR == SYSTEM_SleepTime ( 50 ))	
		if( RET_NOERR == PROCESSPIPE_WatiForCompletation( PipeID, &IsDone, TimeOut )) 	
		if( (error=PIPEERR_TMEOUT) && FLAG_TRUE == IsDone ) 
		{
			strncpy( Buffer, PipeData->ReadBuffer, BufferSize );
			Buffer[BufferSize] = '\0';
			PipeData->BytesRead = 0;  
			memset( PipeData->ReadBuffer, NULL, 500 );
			error = 0;
		}
	
		if( 0 == error )  				  return RET_NOERR; 
		if( PIPEERR_TMEOUT==error ) return RET_UNDEF; 
	}
	
	ErrorListing ( MSG_ERROR, error, " ReadPipe" ); 
	return RET_ERROR; 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL PROCESSPIPE_WritePipe( int PipeID, char* Buffer, int BufferSize, int TimeOut )
{
	int error;
	int BytesWrite; 
	
	if( (error=PIPEERR_INVALID) && PipeID>0 )
	if( (error=PIPEERR_PARAM)	&& Buffer != NULL && BufferSize>0 )	
	{
		error=-1;
		PIPE_INFO *PipeData; 
		PipeData=&PipeList[PipeID-1];

		FLAG IsDone;
		if( RET_NOERR == PROCESSPIPE_WatiForCompletation( PipeID, &IsDone, TimeOut ))
    if( FLAG_TRUE == IsDone && (error=PIPEERR_TMEOUT) )	
		{
			//PipeData->ReadFlag = 1; //before wirte need to read, to avoid thread blocking   //mutex   
			PipeData->BytesWriten = BufferSize;
			strncpy( PipeData->WriteBuffer,Buffer,BufferSize );
			PipeData->WriteBuffer[BufferSize] = '\0';
			PipeData->WriteFlag = 1;
			
			return RET_NOERR;
		}
		
		if( PIPEERR_TMEOUT==error ) return RET_UNDEF;   
	}

	ErrorListing ( MSG_ERROR, error, " WritePipe" ); 
	return RET_ERROR; 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
static int PipeExecution ( void *funcData )
{
	int error;
	int *PipeID = funcData;
	PIPE_INFO *PipeData; 
	
	if( (error=PIPEERR_INVALID) && 0<*PipeID && (PipeData=&PipeList[*PipeID-1]) )
	{
		PipeData->ThreadRunning = 1;  
		
		while( PipeData->ThreadRunning && PipeData->Handle && (RET_NOERR == SYSTEM_SleepTime(50)) )
		{
			switch( PipeData->ExecStep )
			{
				case PIPE_IDLE: 
				{
					if( !PipeData->OpenFlag      )  PipeData->ExecStep = PIPE_OPEN;	
					else if( PipeData->ReadFlag  )	PipeData->ExecStep = PIPE_READ;  
					else if( PipeData->WriteFlag )	PipeData->ExecStep = PIPE_WRITE;	  
	        
				}break;
				
				case PIPE_OPEN: 
				{
					error=PIPEERR_OPEN;
					if( ConnectNamedPipe(PipeData->Handle,NULL) != 0)
					{
						error=0; PipeData->OpenFlag = 1;	
					}
					PipeData->ExecStep = PIPE_IDLE; 
				}break; 
			
				case PIPE_READ: 
				{
					error=PIPEERR_READ;
					int BytesRead=0;
					
					//if( PipeData->BytesRead >= 0 ) PipeData->BytesRead = 50; //mutex
					
					if( ReadFile( PipeData->Handle,PipeData->ReadBuffer,PipeData->BytesRead,&BytesRead,NULL) > 0 ) 
					{
						error=0;  PipeData->ReadFlag = 0; 
						//PipeData->BytesRead = BytesRead; //mutex   		
					}
					PipeData->ExecStep = PIPE_IDLE; 
				}break; 
	
				case PIPE_WRITE: 
				{
					error=PIPEERR_WRITE; 
					int BytesWrite=0; 
					if( WriteFile( PipeData->Handle,PipeData->WriteBuffer,PipeData->BytesWriten,&BytesWrite,NULL) > 0 ) 
					{
						error=0; PipeData->WriteFlag = 0;
						PipeData->BytesWriten = 0;
						memset( PipeData->WriteBuffer, NULL, 500 ); 
					}	
					PipeData->ExecStep = PIPE_IDLE; 
				}break;
				
				default: //initial status
					PipeData->ExecStep = PIPE_IDLE; break;
			}
			
			if(error) break; 
		}
	}

	PipeData->ThreadRunning = 0;
	
	if(!error) 
	{
		return RET_NOERR;
	}
	else
	{
		int nError = GetLastError();
		char info[100];
		strcpy( info, " PipeExecThread ");

		switch( GetLastError() )
		{
			case 0:  		break; 
			case 2:     strcat(info, "_ERROR_FILE_NOT_FOUND ");  break; 
			case 5:  		strcat(info, "_ERROR_ACCESS_DENIED ");   break; 	
			case 6:  		strcat(info, "_ERROR_INVALID_HANDLE ");  break;   
			case 232:		strcat(info, "_ERROR_PIPE_CLOSED "   );  break;
			
			default:  	strcat(info, "_ERROR_UNKNOWN ");	       break;  
		}
		
		ErrorListing ( MSG_ERROR, error, info ); 
		return RET_ERROR; 
	}
	
}


