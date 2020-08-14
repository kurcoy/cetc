//==============================================================================
//
// Title:		ApplicationSetup.c
// Purpose:		A short description of the implementation.
//
// Created on:	2020/4/16 at 11:06:57 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include "ApplicationSetup.h"

#include "PipeProcess.h"
#include "UserPanel.h"  
#include "WorkingCodeSys.h"
#include "mysqlite.h" 
#include "comport.h"     
#include "UserOperation.h"
//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

RETVAL ApplicationSetup  ( void )
{
	static char *SQLiteDataBase[] = { "BlastingInfo", "" }; 
	static char *ComPortDevice [] = { "BlastingDev", "" };  
	//////////////////////////add modules into applicaiton//////////////////////////////   
	int newSysModuleID;
	
	newSysModuleID =  
    SYSTEM_AddSystemModule     (0, "", "UserOperation" );              
    SYSTEM_SetSysModuleInstFunc( newSysModuleID, USEROPERATION_Install,  0 );
		SYSTEM_SetSysModuleErrorLog( newSysModuleID, 1, 1000 							     );  
	
	newSysModuleID =  
    SYSTEM_AddSystemModule     (0, "", "ProcessPipe" );              
    SYSTEM_SetSysModuleInstFunc( newSysModuleID, PROCESSPIPE_Install,    0 );
		SYSTEM_SetSysModuleErrorLog( newSysModuleID, 1, 1000 							     );  
		
	newSysModuleID =  
    SYSTEM_AddSystemModule     (0, "", "WorkingCodeSys" );              
    SYSTEM_SetSysModuleInstFunc( newSysModuleID, WORKINGCODESYS_Install, 0 );
		SYSTEM_SetSysModuleErrorLog( newSysModuleID, 1, 1000 							     );      

	newSysModuleID =  
    SYSTEM_AddSystemModule     (0, "", "MYSQLITE" );              
    SYSTEM_SetSysModuleInstFunc( newSysModuleID, SQLITE_Install, SQLiteDataBase );
		SYSTEM_SetSysModuleErrorLog( newSysModuleID, 1, 1000 							          );

	newSysModuleID =  
    SYSTEM_AddSystemModule     (0, "", "COMPORT" );              
    SYSTEM_SetSysModuleInstFunc( newSysModuleID, COMPORT_Install,ComPortDevice  );
		SYSTEM_SetSysModuleErrorLog( newSysModuleID, 1, 1000 							          );		

 //////////////////////////add main panel for start up////////////////////////////// 
	char *UirFileName = "100_UserFunction\\UserPanel.uir"; 
	SYSTEM_SetSystemMainPanel   ( UirFileName, USER_PANEL );  
	
	//////////////////////////xxxxxxxxxxxxxxxxxxxxxxxxxxx//////////////////////////////
	
		
	return RET_NOERR;
}
