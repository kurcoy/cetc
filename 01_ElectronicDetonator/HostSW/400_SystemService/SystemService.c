//==============================================================================
//
// Title:		CoreService.c
// Purpose:		A short description of the implementation.
//
// Created on:	2020/4/16 at 10:32:09 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <windows.h>   
#include <cvintwrk.h>
#include <utility.h>
#include "toolbox.h"
#include <userint.h>
#include <ansi_c.h>
#include "SystemService.h"

//==============================================================================
// Constants

//==============================================================================
// Types
typedef struct
{
	int  OwnID; 
	int  parentSysModuleID;
	int  PanelID;
	int  ErrorCtrlID;  
	int  ErrorOffsetID;
	char ModuleName[50];
	char TabName[50];
	
	RETVAL  (*InstFunc)   (int SysModuleID, int PanelID,   void *instData);
  RETVAL  (*UninstFunc) (int SysModuleID, int PanelID,   void *uninstData);
  RETIDX  (*ErrListFunc)(MSGTYPE type,    int numError,  char *AddInfo);
  
  void *InstData;
  void *UninstData;
	
}SYSMODULE,*pSYSMODULE;
//============================================================================== 
typedef struct 
{
  int            ID;
  int            SysModuleID;
  int            WndThreadId;
  int            Status;
  HANDLE         hThread;
  
  threadFunc    *ThreadFunction;
  void          *funcData;
}
THREADDATA, *pTHREADDATA;
//============================================================================== 
typedef enum
{ 
  THREAD_FAILED     = -1,
  THREAD_TERMINATED = -2,
  THREAD_REGISTERED =  0,
  THREAD_FINISHED   =  1,
  THREAD_STARTED    =  2,
  THREAD_RESTART    =  3,
  THREAD_RUNNING    =  4,
  THREAD_STOP       =  5
}
THREAD_STATUS;


//============================================================================== 
extern char ProjectDirectory[MAX_PATHNAME_LEN];
char MainPanelName[200];
int  MainPanelID; 
//==============================================================================
// Static global variables

static pSYSMODULE    *SysModuleList;
static pTHREADDATA   *ThreadDataList; 
static int          SysModuleListLen;
static int 					CurrentFreeMuduleID;

static int          ThreadDataListLen;
static int 					CurrentFreeThreadID;
static int          SysModuleID;
//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions
typedef enum
{
  SYSERR_PARAM=1,
	SYSERR_FILEOPEN,
	SYSERR_THREAD_CREATED,
}
ERRORS;

static RETIDX SYSTEM_ErrorListing ( MSGTYPE Type, int numError, char *addInfoText )
{
  char *MessageText = "Unknown Error";
  
  switch(numError)
  {
    case SYSERR_PARAM:                   MessageText = "Invalid Parameter";                 break;
    case SYSERR_FILEOPEN:								 MessageText = "The written file already opened";  break; 
		
		case SYSERR_THREAD_CREATED:          MessageText = "Create Thread Failed";              break;
		
		
    default:                              numError   = -1;                                  break;
  }
	char info[200]; 
	strcpy( info, addInfoText ); 
	strcat( info, "_" ); 
	strcat( info, MessageText);
	
  SYSTEM_GenerateMessage ( Type, SysModuleID, numError, info );
	return numError;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
static RETVAL UnInstall ( int SysModuleID,  int PanelID,   void *funcData )     
{
	//call Uninstall once
	//free thread data
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_Install ( int _SysModuleID, int PanelID, void *funcData )  
{
	CurrentFreeMuduleID = 0;
	memset( MainPanelName, NULL, 200);   	
	SysModuleID = SYSTEM_AddSystemModule( 0, NULL, "System" );
	
	return RET_NOERR; 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETID  SYSTEM_AddSystemModule        ( int parentSysModuleID, char *TabName, char *ModuleName ) 
{
	pSYSMODULE newSysModule;
  int i=0, error=SYSERR_PARAM;
	
  if( !ModuleName || *ModuleName==0 )
  for(i=1; i<=CurrentFreeMuduleID; i++)
  {
    if( SysModuleList && SysModuleList[i-1])
    if(strcmp(SysModuleList[i-1]->TabName, TabName) == 0) 
    return SysModuleList[i-1]->OwnID;
  }
  
  newSysModule = calloc (1, sizeof(SYSMODULE));
    
  if(CurrentFreeMuduleID == SysModuleListLen)
  {
    if(!SysModuleList) SysModuleList = calloc  (SysModuleListLen += 10, sizeof(pSYSMODULE));
    else               SysModuleList = realloc (SysModuleList, (SysModuleListLen += 10)*sizeof(pSYSMODULE));
  }
  
  SysModuleList[CurrentFreeMuduleID] = newSysModule;
  newSysModule->OwnID             = ++CurrentFreeMuduleID;
  newSysModule->parentSysModuleID    = parentSysModuleID;
  
  if(TabName )             strncpy(newSysModule->TabName,    TabName,    50);
  if(ModuleName)           strncpy(newSysModule->ModuleName, ModuleName, 50);
    
  return newSysModule->OwnID;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_SetSysModuleInstFunc   ( int SysModuleID, systemFunc *instFunc,   void *funcData )
{
	int error=SYSERR_PARAM;
  if( SysModuleID>0 && SysModuleID<=CurrentFreeMuduleID )
  if( SysModuleList[SysModuleID-1])
  {
    SysModuleList[SysModuleID-1]->InstFunc = instFunc;
    SysModuleList[SysModuleID-1]->InstData = funcData;
    return RET_NOERR;
  }

	SYSTEM_ErrorListing ( MSG_ERROR, error, " SetSysInstFunc");
  return RET_ERROR;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_SetSysModuleUninstFunc ( int SysModuleID, systemFunc *uninstFunc, void *funcData )
{
	int error=SYSERR_PARAM;

	if( SysModuleID>0 && SysModuleID<=CurrentFreeMuduleID)
	if( SysModuleList[SysModuleID-1])
	{
	  SysModuleList[SysModuleID-1]->UninstFunc = uninstFunc;
	  SysModuleList[SysModuleID-1]->UninstData = funcData;
	  return RET_NOERR;
	}

	SYSTEM_ErrorListing ( MSG_ERROR, error, " SetSysUninstFunc");
  return RET_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_SetSysModuleErrListFunc ( int SysModuleID,  errorListFunc *ErrorListFunc )
{
  int error = SYSERR_PARAM;
  
  if( SysModuleID>0 && SysModuleID<=CurrentFreeMuduleID)
  if( SysModuleList[SysModuleID-1])
  {
    SysModuleList[SysModuleID-1]->ErrListFunc = ErrorListFunc;
    return RET_NOERR;
  }
  
  SYSTEM_ErrorListing ( MSG_ERROR, error, " SetSysErrListFunc");
  return RET_ERROR;
}
///////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////// 
RETVAL SYSTEM_SetSysModuleErrorLog ( int SysModuleID, int ErrorCtrlID, int ErrorOffsetID )
{
	int error;
  
	if( (error=SYSERR_PARAM) && ErrorCtrlID > 0 && ErrorOffsetID > 0 )
	if( SysModuleID>0 && SysModuleID<=CurrentFreeMuduleID )
  if( SysModuleList[SysModuleID-1])	
	{
	  SysModuleList[SysModuleID-1]->ErrorOffsetID = ErrorOffsetID;
		SysModuleList[SysModuleID-1]->ErrorCtrlID   = ErrorCtrlID;
		return RET_NOERR;
	}
	
	SYSTEM_ErrorListing ( MSG_ERROR, SYSERR_PARAM, "SetSystemErrorLog" ); 
	return RET_ERROR;	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//Errorlog_dump data into local file


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_SetSystemMainPanel         ( const char *UirFileName, int panelID )
{
	if( UirFileName != NULL && panelID > 0)
	{
		memset( MainPanelName, NULL, 200);
		strcpy( MainPanelName, UirFileName); 
		MainPanelID = panelID;
	  return RET_NOERR;
	}
	
	SYSTEM_ErrorListing ( MSG_ERROR, SYSERR_PARAM, " SetSysMainPanel" ); 
	return RET_ERROR;	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETID SYSTEM_LoadSystemMainPanel		 ( void )
{
	int panelHandle; 
	
	if( *MainPanelName && MainPanelID > 0)  //already set MainPanel uir name 
	if( (panelHandle = LoadPanel ( 0, MainPanelName, MainPanelID)) > 0 ) 
	{
	  return panelHandle;
	}
	return -1;	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_InstallModules         ( void ) 
{
	pSYSMODULE SysModule;
	
	for( int i=1; i<=CurrentFreeMuduleID; i++ )
	{
		SysModule = SysModuleList[i-1]; 
		if( SysModule )
		{
		  int parentPanel;
		  SysModule->PanelID = 0;
			/*
		  if( SysModule->TabName && *SysModule->TabName )
		  {
		    if(SysModule->parentSysModuleID) parentPanel = SYSTEM_GetParentPanelID ( SysModule->parentSysModuleID );
		    else                             parentPanel = mainPanel;
    
		    SysModule->PanelID = TABMAN_NewTabPage ( parentPanel, -1, SysModule->TabName );
		  }
		  else
		  {
		    if(SysModule->parentSysModuleID) 
		      SysModule->PanelID = SYSTEM_GetParentPanelID ( SysModule->parentSysModuleID );
		  }
			*/
		  if( SysModule->InstFunc ) 
		    SysModule->InstFunc ( SysModule->OwnID, SysModule->PanelID, SysModule->InstData );
		}
	}
		return RET_NOERR;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_UnInstallModules       ( void )
{
	pSYSMODULE SysModule;
	
	for( int i=1; i<=CurrentFreeMuduleID; i++ )
	{
		SysModule = SysModuleList[i-1]; 
		if( SysModule )
		{
		  int parentPanel;
		  SysModule->PanelID = 0;
			/*
		  if( SysModule->TabName && *SysModule->TabName )
		  {
		    if(SysModule->parentSysModuleID) parentPanel = SYSTEM_GetParentPanelID ( SysModule->parentSysModuleID );
		    else                             parentPanel = mainPanel;
    
		    SysModule->PanelID = TABMAN_NewTabPage ( parentPanel, -1, SysModule->TabName );
		  }
		  else
		  {
		    if(SysModule->parentSysModuleID) 
		      SysModule->PanelID = SYSTEM_GetParentPanelID ( SysModule->parentSysModuleID );
		  }
			*/
		  if( SysModule->UninstFunc ) 
		    SysModule->UninstFunc ( SysModule->OwnID, SysModule->PanelID, SysModule->InstData );
		}
	}
	
	if(SysModuleList) { free(SysModuleList); SysModuleList=0; }
	return RET_NOERR;		
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_CreateThreadIdentifier ( int SysModuleID, int *ThreadID )
{
	int error; 
	if( (error=SYSERR_PARAM) && SysModuleID>0 && SysModuleID<=CurrentFreeMuduleID )
	{
	  if(CurrentFreeThreadID == ThreadDataListLen)
	  {
	    if(!ThreadDataList) ThreadDataList = calloc  (ThreadDataListLen += 10, sizeof(THREADDATA));
	    else                ThreadDataList = realloc (ThreadDataList, (ThreadDataListLen += 10)*sizeof(THREADDATA));
	  }
  
	  pTHREADDATA newThreadData  = &ThreadDataList[CurrentFreeThreadID];
	  newThreadData->ID	         = ++CurrentFreeThreadID;
		newThreadData->SysModuleID = SysModuleID; 
		newThreadData->hThread     = 0; 
		newThreadData->Status      =  THREAD_REGISTERED;
		*ThreadID = newThreadData->ID;
		return RET_NOERR;
	}
	SYSTEM_ErrorListing( MSG_ERROR, error, " CreateThreadId" );  
	return RET_ERROR;  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
static DWORD WINAPI ThreadPreFunction ( void *funcData )
{
  pTHREADDATA currTData = funcData;
  
  currTData->Status      = THREAD_RUNNING;
  currTData->WndThreadId = GetCurrentThreadId();

  do 
  {
    if(currTData->ThreadFunction) currTData->ThreadFunction ( currTData->funcData );
    currTData->Status  = THREAD_FINISHED;
    
    while ( currTData->Status != THREAD_STOP && currTData->Status != THREAD_RESTART ) Sleep( 10 );
  }
  while ( currTData->Status != THREAD_STOP );
  
  currTData->hThread = 0; return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_CreateThread 					 ( int ThreadID, threadFunc *ThreadFunction, void *funcData )
{
	int error; 
	if( (error=SYSERR_PARAM) && ThreadID>0 &&ThreadID<=CurrentFreeThreadID )
	if( NULL != ThreadFunction )
	{
		pTHREADDATA currTData = &ThreadDataList[ThreadID-1];
		if( currTData->hThread != 0 )
		{
			currTData->Status = THREAD_RESTART;
			return RET_NOERR; 
		}
		else
		{
			currTData->ThreadFunction = ThreadFunction;
	    currTData->funcData       = funcData;
	    currTData->hThread = CreateThread( NULL, 0, ThreadPreFunction, currTData, 0, 0);
		
			if(currTData->hThread)
      {
        currTData->Status  = THREAD_STARTED;
        Sleep(1);
        return RET_NOERR;
      }
      else
      {
        currTData->Status  = THREAD_FAILED;
        error       = SYSERR_THREAD_CREATED;
      }	
		}
	}
	SYSTEM_ErrorListing( MSG_ERROR, error, " CreateThread" );  
	return RET_ERROR;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_TerminateThread 			 ( int ThreadID )
{
	int error; 
	if( (error=SYSERR_PARAM) && ThreadID>0 &&ThreadID<=CurrentFreeThreadID )
	{
		error=0;
		if( &ThreadDataList[ThreadID-1]->hThread )
		{
	    pTHREADDATA currTData = &ThreadDataList[ThreadID-1]; 
			TerminateThread( currTData->hThread, 0);
	    currTData->hThread = 0;
	    currTData->Status  = THREAD_TERMINATED;
		}
		return RET_NOERR;			
	}
	SYSTEM_ErrorListing( MSG_ERROR, error, " TerminateThread " );  
	return RET_ERROR;	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_FormatErrorOutput ( int errNumber, char *errString )
{
  int a = errNumber/1000000;
  int b = abs(errNumber-a*1000000);
  int c = abs(b-((b/1000)*1000));
  
  if(a<0) sprintf( errString, "%04d %03d %03d", a, b/1000, c );
  else    sprintf( errString, "%03d %03d %03d", a, b/1000, c );
  
  return RET_NOERR;;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_GenerateMessage        ( MSGTYPE Type, int SysModuleID, int errNumber, char *fullMessage )
{
	if( SysModuleID>=0 )  
  if( SysModuleID<=CurrentFreeMuduleID && SysModuleList ) 
  {
    char *Module = 0;
		int offsetErrID, ErrorCtrlID; 
			
		
		if( SysModuleID>0 )
		{
			Module = SysModuleList[SysModuleID-1]->ModuleName;   
			offsetErrID = SysModuleList[SysModuleID-1]->ErrorOffsetID;
			ErrorCtrlID = SysModuleList[SysModuleID-1]->ErrorCtrlID; //check contrl id valid=?
			errNumber += 1000000000; 
			errNumber += offsetErrID;
		}
		else
		{
			Module = "System"; 
			ErrorCtrlID = SysModuleList[SysModuleID]->ErrorCtrlID; //check contrl id valid=?
			errNumber += 1000000000; 
		}

		char errString[20];
    SYSTEM_FormatErrorOutput ( errNumber, errString );
		
		//SetCtrlAttribute (MainPanelID, ErrorCtrlID, ATTR_CTRL_VAL, errString );        
		//save data to local
		char *Header[] = { "ErrorCode", "Module", "ErrorInfo", "" }; 
		char *DataArray[3] = { errString, Module, fullMessage};
		SYSTEM_DumpDatatoLocalFile( DATAT_ERRORLOG, Header, DataArray, 3 ); 
		
		switch( Type )	//to display in different format
		{
			case MSG_STATUS: 
			{
			//VAL_BLACK	
			}
			break; 
		
			case MSG_WARNING: 
			{
				//VAL_YELLOW 
			}
			break;
		
			case MSG_ERROR: 
			{
				//VAL_RED
			}
			break; 
		}
	
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_DumpDatatoLocalFile( DATATYPE LogDataType, char **FileHeader, char **DataArray, int NumOfData )
{
  char   sysPath[300]; 
	char   FilePath[300]; 
  FILE   *ResultFile;
  int    error=SYSERR_PARAM;
  char   time[20];
  int    month, day, year;
  //char   *buffer;
  
	if( FileHeader != NULL && DataArray != NULL && NumOfData>0         )
  if( (error=-1)&& RET_NOERR == SYSTEM_GetPath       ( sysPath ))
  if(         0 == GetSystemDate        ( &month, &day, &year       ))
  //if(      NULL != ( buffer = calloc    ( NumOfData, sizeof(double))))
  {  
    sprintf ( time, "%04u-%02u-%02u", year, month, day     );
		strcpy  ( FilePath, sysPath ); 
		strcat  ( FilePath, "\\900_ResultFile" );
		
		if( 1 > FileExists ( FilePath, 0 ))   MakeDir( FilePath ); 
		
		switch( LogDataType )
		{
			case DATAT_ERRORLOG: 
			{
				strcat  ( FilePath, "\\ErrorReport" );	
			}
			break;
			
			default:
			{
				strcat  ( FilePath, "\\ResultReport" ); break;
			}
		}

    if( 1 > FileExists ( FilePath, 0 ))   MakeDir( FilePath );
    
    strcat  ( FilePath, "\\"   );
    strcat  ( FilePath, time   );
    strcat  ( FilePath, ".csv" );
  	error=SYSERR_FILEOPEN;   
    //////////////////////////////////////////////////////////////////////////////////////////////
    //new report and build data item title/ file header 
    //////////////////////////////////////////////////////////////////////////////////////////////
    if( 1 > FileExists   ( FilePath, 0   )) 
    {
      int i=0;
      ResultFile = fopen ( FilePath, "w+" );          
      
      if( NULL != ResultFile )
      {
        fprintf ( ResultFile, "%s,", "TimeStamp" );
        while( *FileHeader[i] )
        {
          fprintf( ResultFile, "%s,", FileHeader[i] );
          i++; 
        }
      }
      fprintf( ResultFile, "%s", "\n"     ); 
      fclose ( ResultFile                 );
			error=0; 
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////
    //collect and dump data into file 
    //////////////////////////////////////////////////////////////////////////////////////////////
    if( 0 < FileExists   ( FilePath, 0  ))
    {
      int isWriteable;
			GetFileWritability( FilePath, &isWriteable );
			if(isWriteable)
			{
				ResultFile = fopen ( FilePath, "a" );
	      if( NULL != ResultFile )
	      {
	        fprintf( ResultFile, "%s,",     TimeStr() );
	        for( int i=0; i < NumOfData; i++ )
	        {
	          fprintf( ResultFile, "%s,", DataArray[i] );
	        }
	        fprintf( ResultFile, "%s", "\n"            );
	        fclose ( ResultFile                        );
	        error=0;
	      }
			}
    }
    //if(buffer) free(buffer); buffer=0;
    if(!error) return RET_NOERR;    
  }
	
	if(error==SYSERR_FILEOPEN) MessagePopup("Error", "The file already opened" ); 
	
	SYSTEM_ErrorListing( MSG_ERROR, error, " LogData" ); 
  return RET_ERROR; 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_SleepTime 			       ( int MilliSeconds )
{
	int error=SYSERR_PARAM;
	if( MilliSeconds>0 )
	{
		Sleep(MilliSeconds); 
		return RET_NOERR;
	}
	SYSTEM_ErrorListing( MSG_ERROR, error, " SleepTime" );  
	return RET_ERROR;	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_GetPath 			   			 ( char *SystemPath )
{
	int error=SYSERR_PARAM; 
	if( SystemPath )
	{
		strcpy( SystemPath, ProjectDirectory );
		return RET_NOERR;
	}
	SYSTEM_ErrorListing( MSG_ERROR, error, " GetPath" );  
	return RET_ERROR;	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
RETVAL SYSTEM_CheckIPNetwork( char *IpAddress, int *Available )
{
	int error=SYSERR_PARAM;
	int InetError;
	
	if( NULL != IpAddress && NULL != Available )
	{
		*Available = 1; 
		InetError = InetPing( IpAddress, Available, 1000 );
		
		if( 0 == InetError )
		{
			return RET_NOERR;   		
		} 
		
	}
	
	char *info = InetGetErrorMessage( InetError ); 
	strcat(info, "_Network" );
	SYSTEM_ErrorListing( MSG_ERROR, error, info );  
	return RET_ERROR;	
}
