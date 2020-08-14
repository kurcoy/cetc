//==============================================================================
//
// Title:		urlCode.h
// Purpose:		A short description of the interface.
//
// Created on:	2020/5/11 at 15:37:57 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __urlCode_H__
#define __urlCode_H__

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

//==============================================================================
// External variables

//==============================================================================
// Global functions
			
/* urlIn:      inout parameter   */
/* urlEncoded: ouput parameter, need to free by caller */   
/* lens:       ouput parameter, the length of urlEncoded string */   

int urlencode( unsigned char *urlIn, unsigned char **urlEncoded, int *lens);  
 
#ifdef __cplusplus
    }
#endif

#endif  /* ndef __urlCode_H__ */
