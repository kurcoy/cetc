//==============================================================================
//
// Title:		UserOperation.h
// Purpose:		A short description of the interface.
//
// Created on:	2020/6/28 at 9:43:54 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __UserOperation_H__
#define __UserOperation_H__

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
 typedef enum
{
  UP_DL_REQUEST=1,
	UP_DL_NOTFOUND,
	UP_DL_NOTTASK,
	UP_DL_BLACKLIST,
	UP_DL_BEYOND,
	UP_DL_WITHIN,
	UP_DL_NOTEXIST,
	UP_DL_NOTASK,
	UP_DL_PROJECT,
	UP_DL_NOTSETAREA,
	UP_DL_OFFLINE,
	UP_DL_NOTSET,
	UP_DL_NETWORK=99,   
	
	UP_INST_MODULE=100,		 
	UP_ENCRYT,
	UP_DECRYT,
	UP_NETWORK,
	UP_DATABASE,
	UP_INFOPARSE,
	UP_UIDNUM, 
	UP_UPLOADED,
	UP_DEV_INVALID,  
	
	UP_JBDIS,
	UP_ZBDIS,
	UP_JBSJ,
	UP_ZBSJ,
	
	UP_DET_ERR=200,
	UP_DET_BLACKLIST=201,
	UP_DET_USE,
	UP_DET_NOTEXIST,

	
}
USEROP_ERS;
//==============================================================================
// External variables

//==============================================================================
// Global functions

RETVAL USEROPERATION_Install      ( int _SysModuleID,  int PanelID, void *funcData );
RETVAL USEROPERATION_GetErrorText ( int numError, char **addInfoText );   
#ifdef __cplusplus
    }
#endif

#endif  /* ndef __UserOperation_H__ */
