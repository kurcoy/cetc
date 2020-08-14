//==============================================================================
//
// Title:		ComPort.h
// Purpose:		A short description of the interface.
//
// Created on:	2020/6/9 at 9:45:32 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __ComPort_H__
#define __ComPort_H__

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

RETVAL COMPORT_Install           ( int _SysModuleID, int PanelID, void *funcData ); 

RETID  COMPORT_Init              ( char *DevName, int ComID, int Baudrate, int ReadBufferSize, int WriteBufferSize );     

RETVAL COMPORT_Open              ( int DevID  );   
RETVAL COMPORT_Close             ( int DevID  );   

RETVAL COMPORT_GetReadBuffer     ( int DevID, char *ReadBuffer ); 
RETVAL COMPORT_WriteBuffer       ( int DevID, char *WriteBuffer, int WriteSize );  


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __ComPort_H__ */
