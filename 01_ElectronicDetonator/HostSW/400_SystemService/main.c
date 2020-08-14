#include <utility.h>
#include <userint.h>
//==============================================================================
//
// Title:		main.c
// Purpose:		host software for electronic detonator module
//
// Created on:	2020/3/20 at 16:16:45 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files


#include "SystemService.h"
#include "ApplicationSetup.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables
char ProjectDirectory[MAX_PATHNAME_LEN];  
//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions
//////////////////////////////////////////////////////////////////////////////////////////////////////  
//////////////////////////////////////////////////////////////////////////////////////////////////////  
static int InitializeApplication ( void )
{
	ApplicationSetup      ( ); 
	SYSTEM_InstallModules ( );      	
	//SYSTEM_CreateThreadIdentifier ( -1, &uirThreadSystemID );
	//SYSTEM_CreateThread( ThreadOne.SystemID, CORE_StateMachine, &ThreadOne ); 
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////// 
int main (int argc, char *argv[])
{
	//check database 
	//check execution contorl software 
	//load userpanel
	//mySQLtest ( );   

	GetProjectDir (ProjectDirectory); 
	
	/* initialize the LabWindows/CVI Run-Time Engine and load the user interface panel into memory */
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;
	
	////////////////////////////////////////////////////////////  
	//if ( (logPanelHandle = LoadPanel ( 0, "UserPanel\\UserPanel.uir", USER_PANEL)) <= 0 )
	//	return -1; 
	
	InitializeApplication (  );      
		
	int PanelHandle = SYSTEM_LoadSystemMainPanel (  );
	if( PanelHandle <= 0 )
		return -1;
	////////////////////////////////////////////////////////////    

	////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////
	
	/* display the panel and run the UI */
	DisplayPanel ( PanelHandle );
	RunUserInterface ( );

	SYSTEM_UnInstallModules ( ); 
	 
	/* free resources and return */
	//DiscardPanel (logPanelHandle);
	CloseCVIRTE ();
	return 0;
}


