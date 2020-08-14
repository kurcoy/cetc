//==============================================================================
//
// Title:		WorkingCodeSys.h
// Purpose:		A short description of the interface.
//
// Created on:	2020/5/6 at 15:58:27 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

#ifndef __WorkingCodeSys_H__
#define __WorkingCodeSys_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"
#include "SystemService.h" 
#include "crypto.h" 
#include "urlRequest.h"
#include "cJSON.h"
//==============================================================================
// Constants

//==============================================================================
// Types
typedef enum
{
	WCT_WCON_DOWNLOAD, 
	WCT_WCOFF_DOWNLOAD, 
	WCT_RESULT_UPLOAD,
	WCT_LOCATION_UPLOAD,
}
WC_TYPE;

typedef struct
{
	char sqrq[50]; 
	char yxq[50];  
	char gzm[50]; 
	char fbh[50]; 
	char gzmcw[50]; 
	char cw[50]; 
	char uid[50]; 
}
INFO_GZM;

typedef struct
{
	char jd[50];
	char wd[50];
	char bj[50]; 
	char sj[50]; 
}
INFO_JBQY;

typedef struct
{
	char mc[50];  
	char jd[50];
	char wd[50];
	char bj[50]; 
	char qssj[50]; 	
	char jzsj[50]
}
INFO_ZBQY;

typedef struct
{
	char sbbh[50]; 
	char jd[50];  
	char wd[50]; 
	char bpsj[50]; 
	char bprysfz[50]; 
	char uid[50]; 
	char xmbh[50];  
	char htid[50]; 
	char dwdm[50]; 
}
INFO_RESULT;
//==============================================================================
// External variables

//==============================================================================
// Global functions
RETVAL WORKINGCODESYS_Install   ( int _SysModuleID, int PanelID, void *funcData ); 

/* EncryptedStr, need to free by caller */	 
/* DecryptedStr, need to free by caller */	 
RETVAL WORKINGCODESYS_Encryption( char *SourceStr, char *DESKey, char **EncryptedStr );
RETVAL WORKINGCODESYS_Decryption( char *SourceStr, int StrSize,  char *DESKey, char **DecryptedStr );

/* Response, need to free by caller */	  
RETVAL WORKINGCODESYS_HTTPPost  ( char *URL, char *Postdata, char** Response, int *ResLength );   

//RETVAL WORKINGCODESYS_DownloadInfoParse (  char *DownloadInfo, int *Status );

/* ParseInfoArray, need to free by caller */
RETVAL WORKINGCODESYS_DetonatorInfoParse 		  ( char *SourceInfo, INFO_GZM  **ParseInfoArray, int *NumOfInfo, int *Status ); 
RETVAL WORKINGCODESYS_InhibitionAreaInfoParse ( char *SourceInfo, INFO_JBQY **ParseInfoArray, int *NumOfInfo, int *Status );       
RETVAL WORKINGCODESYS_PermitionAreaInfoParse  ( char *SourceInfo, INFO_ZBQY **ParseInfoArray, int *NumOfInfo, int *Status );       
RETVAL WORKINGCODESYS_InstruInfoParse 			  ( char *SourceInfo, char     ***ParseInfoArray, int *NumOfInfo, int *Status ); 


#ifdef __cplusplus
    }
#endif

#endif  /* ndef __WorkingCodeSys_H__ */
