//==============================================================================
//
// Title:		PipeProcess.h
// Purpose:		A short description of the interface.
//
// Created on:	2020/4/14 at 9:04:59 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __PipeProcess_H__
#define __PipeProcess_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include "SystemService.h"  

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

RETVAL PROCESSPIPE_Install  ( int _SysModuleID,  int PanelID, void *funcData );       
			
RETID  PROCESSPIPE_Create   ( char *PipeName );
RETVAL PROCESSPIPE_ReadPipe ( int PipeID, char* Buffer, int BufferSize, int TimeOut );
RETVAL PROCESSPIPE_WritePipe( int PipeID, char* Buffer, int BufferSize, int TimeOut ); //default timeout =500ms
RETVAL PROCESSPIPE_WatiForCompletation      ( int PipeID, FLAG *IsDone, int TimeOut );     


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __PipeProcess_H__ */
