//==============================================================================
//
// Title:		CoreService.h
// Purpose:		A short description of the interface.
//
// Created on:	2020/4/16 at 10:32:09 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __CoreService_H__
#define __CoreService_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

//==============================================================================
// Types

typedef enum         //return value 
{                   
  RET_ERROR = -1,    // <0 error / error number
  RET_UNDEF =  0,    // =0 undefined / not set yet / not fully done
  RET_NOERR =  1,    // >0 ok
} 
RETVAL;

typedef enum         
{                    
  FLAG_TRUE  =  1,   
  FLAG_UNDEF =  0,  
  FLAG_FALSE = -1                            
} 
FLAG;

typedef enum
{
	DATAT_REPORT,
	DATAT_ERRORLOG,  
}DATATYPE;

typedef enum
{
	MSG_STATUS,
	MSG_WARNING, 
	MSG_ERROR, 
}
MSGTYPE;

typedef int RETIDX;
typedef int RETID;   // return an Identifier  ( 1, 2, 3, ... -> invalid if <1 )  

typedef RETVAL (systemFunc) ( int SysModuleID,  int PanelID, void *funcData );  
typedef int (threadFunc)( void *funcData ); 
typedef RETIDX (errorListFunc)( MSGTYPE Type, int numError,  char* addInfoText );  

//==============================================================================
// External variables

//==============================================================================
// Global functions

RETID  SYSTEM_AddSystemModule        ( int parentSysModuleID, char *TabName, char *ModuleName ); 
RETVAL SYSTEM_SetSysModuleInstFunc   ( int SysModuleID, systemFunc *instFunc,   void *funcData );
RETVAL SYSTEM_SetSysModuleErrorLog   ( int SysModuleID, int ErrorCtrlID, int ErrorOffsetID );

RETVAL SYSTEM_SetSysModuleUninstFunc ( int SysModuleID, systemFunc *uninstFunc, void *funcData );
RETVAL SYSTEM_SetSysModuleErrListFunc( int SysModuleID, errorListFunc *ErrorListFunc );  

RETVAL SYSTEM_SetSystemMainPanel     ( const char *UirFileName, int panelID );
RETID  SYSTEM_LoadSystemMainPanel		 ( void ); 
  
RETVAL SYSTEM_InstallModules         ( void ); 
RETVAL SYSTEM_UnInstallModules       ( void ); 

RETVAL SYSTEM_CreateThreadIdentifier ( int SysModuleID, int *ThreadID );  
RETVAL SYSTEM_CreateThread 					 ( int ThreadID, threadFunc *ThreadFunction, void *funcData );       
RETVAL SYSTEM_TerminateThread 			 ( int ThreadID );  

RETVAL SYSTEM_DumpDatatoLocalFile    ( DATATYPE LogDataType, char **FileHeader, char **DataArray, int NumOfData );   
RETVAL SYSTEM_GenerateMessage        ( MSGTYPE Type, int SysModuleID, int errNumber, char *fullMessage );    

RETVAL SYSTEM_SleepTime 			       ( int MilliSeconds );  
RETVAL SYSTEM_GetPath 			   			 ( char *SystemPath );   

RETVAL SYSTEM_CheckIPNetwork				 ( char *IpAddress, int *Available ); 



//int RecurseFileFolder( char *FilePath, char***FileNames );
//int readfrom local file
//int EnumComPort( int tabPanelHandle ) ;


//display panel, default uir file located in system folder 

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __CoreService_H__ */
