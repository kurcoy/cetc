//==============================================================================
//
// Title:		urlRequest.h
// Purpose:		A short description of the interface.
//
// Created on:	2020/5/11 at 17:42:27 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __urlRequest_H__
#define __urlRequest_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include "curl.h"

//==============================================================================
// Constants

//==============================================================================
// Types
typedef struct
{
	char *str;
	int  size;	
}
URL_RESP_STR;
//==============================================================================
// External variables

//==============================================================================
// Global functions

//char* Params:   post param will be url decoded   
//char* Response: will be calloced this function and need to free by caller			
int urlRequest_Post( char *url, char *Params, char** Response, int *ResLength );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __urlRequest_H__ */
