//==============================================================================
//
// Title:		WorkingCodeSys.c
// Purpose:		A short description of the implementation.
//
// Created on:	2020/5/6 at 15:58:27 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include "WorkingCodeSys.h"
#include "toolbox.h"
#include <utility.h>
#include <ansi_c.h>

//==============================================================================
// Constants
#define LIB_OK  1
//==============================================================================
// Types

//==============================================================================
// Static global variables
static int SysModuleID;
//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions
typedef enum
{
  DESERR_INST_MODULE=-100,
	DESERR_PARAM,
	DESERR_BASE64EN,
	DESERR_BASE64DE,
	DESERR_3DESEN,
	DESERR_3DESDE,
	DESERR_HTTPPOST,
	DESERR_POSTRESP,  
	DESERR_CJSON,
	
}
DESERR_NRS;             

static RETIDX ErrorListing ( MSGTYPE Type, int numError, char *addInfoText )
{
  char *MessageText = "Unknown Error";
  
  switch( numError )
  {
    case DESERR_INST_MODULE:         MessageText = "Installation failed";                break;
    case DESERR_PARAM:               MessageText = "Invalid parameter";                  break;
		case DESERR_BASE64EN:            MessageText = "Base64 encryption failed";           break;  
		case DESERR_BASE64DE:            MessageText = "Base64 decryption failed";           break;  
		case DESERR_3DESEN:              MessageText = "3DES encryption failed";             break;   
		case DESERR_3DESDE:              MessageText = "3DES decryption failed";             break;   
		case DESERR_HTTPPOST:            MessageText = "Http post process failed";           break;
		case DESERR_POSTRESP:						 MessageText = "Http post response failed";          break;
		case DESERR_CJSON:               MessageText = "cJSON parse info failed";            break;  
		
    default:                          numError    = -1;                                  break;
  }
	char info[200]; 
	strcpy( info, addInfoText );
	strcat( info, "_" );     
	strcat( info, MessageText);
	
  SYSTEM_GenerateMessage ( Type, SysModuleID, numError, info );

  return numError;
}
/*****************************************************************************************************/
/*****************************************************************************************************/
static RETVAL UnInstall ( int SysModuleID,  int PanelID,   void *funcData )
{
	return RET_NOERR;
}
/*****************************************************************************************************/
/*****************************************************************************************************/
RETVAL WORKINGCODESYS_Install  ( int _SysModuleID,  int PanelID, void *funcData )
{
	SysModuleID = _SysModuleID; 
	
	SYSTEM_SetSysModuleUninstFunc    ( SysModuleID, UnInstall, 0 );
  SYSTEM_SetSysModuleErrListFunc   ( SysModuleID, ErrorListing );
	
	return RET_NOERR;  
}
/*****************************************************************************************************/
/*****************************************************************************************************/    
RETVAL WORKINGCODESYS_Encryption( char *SourceStr, char *DESKey, char **EncryptedStr )
{
	int error = DESERR_PARAM; 
	if( NULL != SourceStr && NULL != DESKey && NULL != EncryptedStr )
	{
		int datalen = strlen((char*) SourceStr );
		error = DESERR_BASE64EN; 
		unsigned char *base64;
		
		if( 0 < des_encode( DES3_ECB, (unsigned char*)DESKey, 24, NULL, (unsigned char*)SourceStr, &base64, &datalen)) 
		if( 0 < datalen && (error=DESERR_3DESEN)) 	
		if( 0 < base64_en( base64, (unsigned char**)EncryptedStr, &datalen))
		if( 0 < datalen ) 
	 	{
			error=0;
		}
		
		if(base64) free(base64);
		if(!error) return RET_NOERR;      
	}
	
	ErrorListing ( MSG_ERROR, error, " Encryption" );  
	return RET_ERROR;
}
/*****************************************************************************************************/
/*****************************************************************************************************/ 
RETVAL WORKINGCODESYS_HTTPPost  ( char *URL, char *Postdata, char** Response, int *ResLength )
{
	int  error = DESERR_PARAM;
	char sysPath[300]; 
  char date[20], time[20];
  int  month, day, year, hours, minutes, seconds;
	
	if( NULL != URL && NULL !=Postdata && NULL != Response  && NULL != ResLength )  
	if( (error=-1) && RET_NOERR == SYSTEM_GetPath ( sysPath ))  
	if( 0 == GetSystemDate( &month, &day, &year ))
	if( 0 == GetSystemTime( &hours, &minutes, &seconds ))
	{
		error = DESERR_HTTPPOST;
		
		char result[10];    
		char FileLogPath[500];
		char fileName[500];
		FILE *fptr; 

		/* file log path in system */
		strcpy  ( FileLogPath, sysPath ); 
		strcat  ( FileLogPath, "\\900_ResultFile\\" );
		if( 1 > FileExists ( FileLogPath, 0 ))  MakeDir( FileLogPath ); 
		
		strcat  ( FileLogPath, "\\DownloadResult\\" ); 
		if( 1 > FileExists ( FileLogPath, 0 ))  MakeDir( FileLogPath ); 
		
		sprintf ( date, "%04u-%02u-%02u", year, month, day ); 
		strcat  ( FileLogPath, date );
		strcat  ( FileLogPath, "\\" );
		
		if( 1 > FileExists ( FileLogPath, 0 ))  MakeDir( FileLogPath ); 
		
		/* post response context */ 
		//char param[4096];   
		//strcpy( param, "param=");
		//strcat( param, Postdata);
		
		int res = urlRequest_Post  ( URL, Postdata, Response, ResLength );
		
		strcpy( fileName, FileLogPath);
		strcpy( result,  (res>0)? "Success_" : "Failure_"); 
		strcat( fileName, result     );
		sprintf ( time, "%02u%02u%02u", hours, minutes, seconds );    
		strcat  ( fileName, time );  
		strcat( fileName,  ".txt"    ); 
		
		fptr = fopen ( fileName, "w+" );
		if( fptr )
		{
			fprintf( fptr, "%s", *Response );
      fclose ( fptr );
			if( res>0 )error=0;
		}

		if( (0 == error) && (*ResLength>0) ) return RET_NOERR; 
	}
	
	ErrorListing ( MSG_ERROR, error, " HTTPPost" );  
	return RET_ERROR;
}
/*****************************************************************************************************/
/*****************************************************************************************************/ 
RETVAL WORKINGCODESYS_Decryption( char *SourceStr, int StrSize, char *DESKey, char **DecryptedStr )
{
	int error = DESERR_PARAM; 
	if( NULL != SourceStr && NULL != DESKey && NULL != DecryptedStr )
	{
		error = DESERR_BASE64DE;
		unsigned char *base64data; 
		int j=0; 
		
		unsigned char *param = ( unsigned char*) calloc( StrSize, sizeof(char));

		for( int i=0; i<StrSize; i++ )
		{
			if( 0x0A != SourceStr[i] )
			{
				param[j++] = SourceStr[i];
			}
		}
		
		//int datalen = strlen((unsigned char*) SourceStr );
		int datalen = j;
	  if( 0 < base64_de ( (unsigned char*)param, &base64data, &datalen))
		//if( 0 < datalen && (error=DESERR_3DESDE))
		if( 0 < des_decode( DES3_ECB, (unsigned char*)DESKey, 24, NULL, base64data, (unsigned char**)DecryptedStr, &datalen))
		if( 0 < datalen ) 
		{
			error=0;
		}
		
		if( param ) { free(param); param=NULL; }
		
		if(base64data) free(base64data);
		if(!error) return RET_NOERR;   
	}
	
	ErrorListing ( MSG_ERROR, error, " Decryption" );  
	return RET_ERROR;
}

/*****************************************************************************************************/
/*****************************************************************************************************/
RETVAL WORKINGCODESYS_DetonatorInfoParse ( char *SourceInfo, INFO_GZM **ParseInfoArray, int *NumOfInfo, int *Status )
{
	int  error=DESERR_PARAM;  
	INFO_GZM *wcContent;  
	*NumOfInfo=0;
	int  NumOfItem=6;
	char *Header[] = { "sqrq", "uid", "cwxx", "yxq", "gzm", "gzmcwxx", "fbh","" };  
	char sqrq[50] = "yyyy-mm-dd hh:mm:ss";
	char info[100];
	
	if( NULL != SourceInfo &&  NULL != Status && NULL != ParseInfoArray ) 
	{
		error=DESERR_CJSON;
		cJSON *root, *tmp_obj, *wc_obj, *itm_obj, *lg_obj; 
		
		root	= cJSON_Parse( SourceInfo );
		if( NULL != root )
		{
			if( (error=DESERR_POSTRESP) )
			if( (tmp_obj = cJSON_GetObjectItem( root, "cwxx" )))
			if( LIB_OK == StrToInt( tmp_obj->valuestring, &error ))  //need to record
			if( (tmp_obj = cJSON_GetObjectItem( root, "sqrq" )))
			if( NULL != strcpy    ( sqrq,   tmp_obj->valuestring ))   	   	
			{
				if( 0 == error ) //response error code
				{	
					if( (lg_obj = cJSON_GetObjectItem( root, "lgs")))
						
					if( 0 <	(*NumOfInfo = cJSON_GetArraySize( lg_obj->child)) )	
					{
						//wcContent  = (char***)calloc( NumOfInfo, sizeof(char*)); 
						wcContent  = (INFO_GZM*)calloc( *NumOfInfo, sizeof(INFO_GZM));
						*ParseInfoArray=wcContent;
						for( int i=0; i<*NumOfInfo;i++ )
						{
							if( 0 == error)
							{
								int j=0; 
								wc_obj = cJSON_GetArrayItem( lg_obj->child,i);//"lg"
								/*
								wcContent[i] = (char**)calloc( NumOfItem, sizeof(char*) );   
								for( int n=0; n<NumOfItem; n++)
								{
							  	wcContent[i][n] = (char*)calloc( 200, sizeof(char) ); 
								}
								*/
								
								if( NULL != wc_obj )
								{
									
									if( NULL != strcpy( wcContent[i].sqrq, sqrq ))   
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "uid" )))   
									if( NULL != itm_obj->valuestring ) strcpy( wcContent[i].uid,   itm_obj->valuestring );
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "yxq" ))) 
									if( NULL != itm_obj->valuestring ) strcpy( wcContent[i].yxq,   itm_obj->valuestring );
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "gzm" ))) 
									if( NULL != itm_obj->valuestring ) strcpy( wcContent[i].gzm,   itm_obj->valuestring );
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "gzmcwxx" ))) 
									if( NULL != itm_obj->valuestring ) strcpy( wcContent[i].gzmcw, itm_obj->valuestring );	
									if( LIB_OK == StrToInt( itm_obj->valuestring, &error        ))
								
									/*
									if( NULL != strcpy( wcContent[i][j++], sqrq ))   
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "uid" )))   
									if( NULL != strcpy( wcContent[i][j++], itm_obj->valuestring ))
									if( NULL != strcpy( wcContent[i][j++], "0" ))   	
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "yxq" )))   
									if( NULL != strcpy( wcContent[i][j++], itm_obj->valuestring ))
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "gzm" ))) 
									if( NULL != strcpy( wcContent[i][j++], itm_obj->valuestring ))
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "gzmcwxx" )))   
									if( NULL != strcpy( wcContent[i][j],   itm_obj->valuestring ))	
									if( LIB_OK == StrToInt( itm_obj->valuestring, &error        )) 
									*/
									{
										if( (itm_obj = cJSON_GetObjectItem( wc_obj, "fbh" ))) strcpy( wcContent[i].fbh,   itm_obj->valuestring);	
										if( error > 0 ) 
										{	
											error += 200; //UP_DET_ERR
											//sprintf( info, "´íÎó£ºÀ×¹ÜUID%s", wcContent[i].uid );
											//MessagePopup ( "Error", info);
											*NumOfInfo=i;
											//char **DataArray =  wcContent[i];
											//SYSTEM_DumpDatatoLocalFile    ( DATAT_REPORT, Header, DataArray, NumOfItem );
										}
									}
								}
							}
							else
							{
								break;
							}
						}
						/*
						for( int i=0; i<NumOfInfo; i++ )
						{
							for( int j=0;j<NumOfItem;j++ )
							{
								if( wcContent[i][j] ) { free( wcContent[i][j] ); wcContent[i][j] = NULL;}
							}
							if( wcContent[i] ) { free( wcContent[i] ); wcContent[i] = NULL;} 
						}
	
						if( wcContent ) { free( wcContent ); wcContent = NULL;}   
						*/
					}
				}
				else
				{
					char cwxx[10]; 
					sprintf( cwxx, "%d", error );
					char *DataArray[] = { sqrq, "na",cwxx, "na", "na", "na",  "" };
					SYSTEM_DumpDatatoLocalFile    ( DATAT_REPORT, Header, DataArray, NumOfItem );
				}
			}
			cJSON_Delete( root ); 
		}
	}

	*Status = error;
	if( 0 <= error ) { return RET_NOERR;}
		
	ErrorListing ( MSG_ERROR, error, "DetonatorInfoParse " );  
	return RET_ERROR;
}
/*****************************************************************************************************/
RETVAL WORKINGCODESYS_InstruInfoParse ( char *SourceInfo, char ***ParseInfoArray, int *NumOfInfo, int *Status )
{
	int  error=DESERR_PARAM;  
	char **wcContent;  
	*NumOfInfo=0;
	
	if( NULL != SourceInfo &&  NULL != Status && NULL != ParseInfoArray ) 
	{
	 	error=DESERR_CJSON;
		cJSON *root, *wc_obj, *itm_obj, *sb_obj; 
		
		root	= cJSON_Parse( SourceInfo );
		if( NULL != root )
		{
			if( (error=DESERR_POSTRESP) )
			{
				error = 0; 	  
				if( (sb_obj = cJSON_GetObjectItem( root, "sbbhs")))	
				if( 0 <	(*NumOfInfo = cJSON_GetArraySize( sb_obj)) )	//if( 0 <	(*NumOfInfo = cJSON_GetArraySize( sb_obj->child)) )	
				{
					wcContent  = (char**)calloc( *NumOfInfo, sizeof(char*)); 
					*ParseInfoArray=wcContent;  
					for( int i=0; i<*NumOfInfo;i++ )
					{
						if( error > -1 )
						{
							error = -1;   
							wc_obj = cJSON_GetArrayItem( sb_obj, i);//"lg"		 _obj->child->child 	(obj)      		 obj->child->Next->child	(obj)
			
							wcContent[i] = (char*)calloc( 50, sizeof(char) );   
				
							if( NULL != wc_obj )
							{
								if( (itm_obj = cJSON_GetObjectItem( wc_obj, "sbbh" ))) 
								if( NULL != itm_obj->valuestring ) strcpy( wcContent[i], itm_obj->valuestring );
								{
									error = 0;
								}	
							}
						}
						else
						{
							break;
						}
					}
				}
			}
			cJSON_Delete( root ); 
		}
	}

	*Status = error;
	if( 0 <= error ) { return RET_NOERR;}   
		
	ErrorListing ( MSG_ERROR, error, " InstrumentParse" );  
	return RET_ERROR;
}
/*****************************************************************************************************/
RETVAL WORKINGCODESYS_PermitionAreaInfoParse ( char *SourceInfo, INFO_ZBQY **ParseInfoArray, int *NumOfInfo, int *Status )
{
	int  error=DESERR_PARAM;  
	INFO_ZBQY *wcContent;   
	*NumOfInfo=0;
	int  NumOfItem=6;

	if( NULL != SourceInfo &&  NULL != Status && NULL != ParseInfoArray )
	{
	 	error=DESERR_CJSON;
		cJSON *root, *wc_obj, *itm_obj, *zb_obj; 
		
		root	= cJSON_Parse( SourceInfo );
		if( NULL != root )
		{
			if( (zb_obj = cJSON_GetObjectItem( root, "zbqys")))	
			{
					error = 0; 	  	
					if( 0 <	(*NumOfInfo = cJSON_GetArraySize( zb_obj->child)) )	
					{
						wcContent  = (INFO_ZBQY*)calloc( *NumOfInfo, sizeof(INFO_ZBQY)); 
						*ParseInfoArray=wcContent;  
						for( int i=0; i<*NumOfInfo;i++ )
						{
							if( error > -1 )
							{
								int j=0; 
								error = -1;   
								wc_obj = cJSON_GetArrayItem( zb_obj->child,i);//"lg"
								/*
								wcContent[i] = (char**)calloc( NumOfItem, sizeof(char*) );   
								for( int n=0; n<NumOfItem; n++)
								{
							  	wcContent[i][n] = (char*)calloc( 200, sizeof(char) ); 
								}
								*/
								if( NULL != wc_obj )
								{
									/*
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "zbqymc" )))   
									if( NULL != strcpy( wcContent[i][j++], itm_obj->valuestring ))
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "zbqyjd" )))   
									if( NULL != strcpy( wcContent[i][j++], itm_obj->valuestring ))
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "zbqywd" ))) 
									if( NULL != strcpy( wcContent[i][j++], itm_obj->valuestring ))
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "zbqybj" )))   
									if( NULL != strcpy( wcContent[i][j],   itm_obj->valuestring ))	
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "zbqssj" )))   
									if( NULL != strcpy( wcContent[i][j],   itm_obj->valuestring ))	
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "zbjzsj" )))   
									if( NULL != strcpy( wcContent[i][j],   itm_obj->valuestring ))	
									*/
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "zbqymc" )))
									if( NULL != itm_obj->valuestring )	strcpy( wcContent[i].mc, itm_obj->valuestring  );
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "zbqyjd" ))) 
									if( NULL != itm_obj->valuestring )	strcpy( wcContent[i].jd, itm_obj->valuestring  );
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "zbqywd" )))
									if( NULL != itm_obj->valuestring ) 	strcpy( wcContent[i].wd, itm_obj->valuestring  );
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "zbqybj" )))
									if( NULL != itm_obj->valuestring )	strcpy( wcContent[i].bj, itm_obj->valuestring  );	
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "zbqssj" ))) 
									if( NULL != itm_obj->valuestring )  strcpy( wcContent[i].qssj, itm_obj->valuestring );
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "zbjzsj" ))) 
									if( NULL != itm_obj->valuestring )  strcpy( wcContent[i].jzsj, itm_obj->valuestring );
									{
										//error += 100;
										error = 0;
									}	
								}
							}
							else
							{
								break;
							}
						}
					}
				}
				
		}
			cJSON_Delete( root ); 
	}

	*Status = error;
	if( 0 <= error ) { *ParseInfoArray=wcContent; return RET_NOERR;}   
		
	ErrorListing ( MSG_ERROR, error, " PermitionAreaParse" );  
	return RET_ERROR;
}	
/*****************************************************************************************************/
RETVAL WORKINGCODESYS_InhibitionAreaInfoParse ( char *SourceInfo, INFO_JBQY **ParseInfoArray, int *NumOfInfo, int *Status )
{
	int  error=DESERR_PARAM;  
	INFO_JBQY *wcContent;        
	 *NumOfInfo=0;
	int  NumOfItem=5;

	if( NULL != SourceInfo &&  NULL != Status && NULL != ParseInfoArray ) 
	{
	 	error=DESERR_CJSON;
		cJSON *root, *wc_obj, *itm_obj, *jb_obj; 
		
		root	= cJSON_Parse( SourceInfo );
		if( NULL != root )
		{
			if( (jb_obj = cJSON_GetObjectItem( root, "jbqys")))	
			{
					error = 0; 	
					if( 0 <	(*NumOfInfo = cJSON_GetArraySize( jb_obj->child)) )	
					{
						wcContent  = (INFO_JBQY*)calloc( *NumOfInfo, sizeof(INFO_JBQY)); 
					  *ParseInfoArray=wcContent;  
						for( int i=0; i<*NumOfInfo;i++ )
						{
							if( error > -1 )
							{
								error = -1;
								int j=0; 
								wc_obj = cJSON_GetArrayItem( jb_obj->child,i);//"lg"
							  /*
								wcContent[i] = (char**)calloc( NumOfItem, sizeof(char*) );   
								for( int n=0; n<NumOfItem; n++)
								{
							  	wcContent[i][n] = (char*)calloc( 200, sizeof(char) ); 
								}
								*/
								if( NULL != wc_obj )
								if( (itm_obj = cJSON_GetObjectItem( wc_obj, "jbqyjd" )))   
								if( NULL != strcpy( wcContent[i].jd, itm_obj->valuestring ))
								if( (itm_obj = cJSON_GetObjectItem( wc_obj, "jbqywd" ))) 
								if( NULL != strcpy( wcContent[i].wd, itm_obj->valuestring ))
								if( (itm_obj = cJSON_GetObjectItem( wc_obj, "jbqybj" )))   
								if( NULL != strcpy( wcContent[i].bj,   itm_obj->valuestring ))	
								if( (itm_obj = cJSON_GetObjectItem( wc_obj, "jbqssj" )))   
								if( NULL != strcpy( wcContent[i].sj,   itm_obj->valuestring ))	
								{
									/*
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "jbqyjd" )))   
									if( NULL != strcpy( wcContent[i][j++], itm_obj->valuestring ))
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "jbqywd" ))) 
									if( NULL != strcpy( wcContent[i][j++], itm_obj->valuestring ))
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "jbqybj" )))   
									if( NULL != strcpy( wcContent[i][j],   itm_obj->valuestring ))	
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "jbqssj" )))   
									if( NULL != strcpy( wcContent[i][j],   itm_obj->valuestring ))	
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "jbjzsj" )))   
									if( NULL != strcpy( wcContent[i][j],   itm_obj->valuestring ))	
									*/
									{
										//error += 100;
										error = 0;
									}	
								}
							}
							else
							{
								break;
							}
						}
					}
				}
				
			}
			cJSON_Delete( root ); 
	}

	*Status = error;
	if( 0 <= error ) { return RET_NOERR;}   
		
	ErrorListing ( MSG_ERROR, error, " InhibitionAreaParse" );  
	return RET_ERROR;	
}
/*****************************************************************************************************/

/*
RETVAL WORKINGCODESYS_DownloadInfoParse ( char *DownloadInfo, int *Status )
{
	int  error=DESERR_PARAM;  
	char *** wcContent;  
	int  NumOfWorkingcode=0;
	int  NumOfAreazb=0;
	int  NumOfAreajb=0;
	int  MumOfInst=0;
	int  NumOfItem=6;
	char *Header[] = { "sqrq", "uid", "cwxx", "yxq", "gzm", "gzmcwxx", "" };  
	char sqrq[50] = "yyyy-mm-dd hh:mm:ss";
	
	if( NULL != DownloadInfo &&  NULL != Status )
	{
	 	error=DESERR_CJSON;
		cJSON *root, *tmp_obj, *wc_obj, *itm_obj, lg_obj, zb_obj, jb_obj, sb_obj; 
		
		root	= cJSON_Parse( DownloadInfo );
		if( NULL != root )
		{
			if( (error=DESERR_POSTRESP) )
			if( (tmp_obj = cJSON_GetObjectItem( root, "cwxx" )))
			if( LIB_OK == StrToInt( tmp_obj->valuestring, &error ))  //need to record
			if( (tmp_obj = cJSON_GetObjectItem( root, "sqrq" )))
			if( NULL != strcpy    ( sqrq,   tmp_obj->valuestring ))   	   	
			{
				if( 0 == error ) //response error code
				{	
					if( (lg_obj = cJSON_GetObjectItem( root, "lgs")))
					if( (zb_obj = cJSON_GetObjectItem( root, "zbqys")))	
					if( (jb_obj = cJSON_GetObjectItem( root, "jbqys")))
					if( (sb_obj = cJSON_GetObjectItem( root, "sbbhs")))	
						
					//	NumOfAreazb = cJSON_GetArraySize( zb_obj->child);
					//  NumOfAreajb = cJSON_GetArraySize( jb_obj->child);
					//	MumOfInst   = cJSON_GetArraySize( sb_obj->child);	
						
						
					if( 0 <	(NumOfWorkingcode = cJSON_GetArraySize( lg_obj->child)) )	
					{
						wcContent  = (char***)calloc( NumOfWorkingcode, sizeof(char*)); 
				
						for( int i=0; i<NumOfWorkingcode;i++ )
						{
							if( error > -1 )
							{
								int j=0; 
								wc_obj = cJSON_GetArrayItem( lg_obj->child,i);//"lg"
				
								wcContent[i] = (char**)calloc( NumOfItem, sizeof(char*) );   
								for( int n=0; n<NumOfItem; n++)
								{
							  	wcContent[i][n] = (char*)calloc( 200, sizeof(char) ); 
								}
					
								if( NULL != wc_obj )
								{
									if( NULL != strcpy( wcContent[i][j++], sqrq ))   
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "uid" )))   
									if( NULL != strcpy( wcContent[i][j++], itm_obj->valuestring ))
									if( NULL != strcpy( wcContent[i][j++], "0" ))   	
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "yxq" )))   
									if( NULL != strcpy( wcContent[i][j++], itm_obj->valuestring ))
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "gzm" ))) 
									if( NULL != strcpy( wcContent[i][j++], itm_obj->valuestring ))
									if( (itm_obj = cJSON_GetObjectItem( wc_obj, "gzmcwxx" )))   
									if( NULL != strcpy( wcContent[i][j],   itm_obj->valuestring ))	
									if( LIB_OK == StrToInt( itm_obj->valuestring, &error        ))  
									{
										//error += 100;
										char **DataArray =  wcContent[i];
										if( RET_NOERR != SYSTEM_DumpDatatoLocalFile    ( DATAT_REPORT, Header, DataArray, NumOfItem ))
										{
											error=-1; break; //how to handle with error   
										}
									}	
								}
							}
							else
							{
								break;
							}
						}
						
						for( int i=0; i<NumOfWorkingcode; i++ )
						{
							for( int j=0;j<NumOfItem;j++ )
							{
								if( wcContent[i][j] ) { free( wcContent[i][j] ); wcContent[i][j] = NULL;}
							}
							if( wcContent[i] ) { free( wcContent[i] ); wcContent[i] = NULL;} 
						}
	
						if( wcContent ) { free( wcContent ); wcContent = NULL;}   
				
					}
				}
				else
				{
					char cwxx[10]; 
					sprintf( cwxx, "%d", error );
					char *DataArray[] = { sqrq, "na",cwxx, "na", "na", "na", "" };
					SYSTEM_DumpDatatoLocalFile    ( DATAT_REPORT, Header, DataArray, NumOfItem );
				}
			}
			cJSON_Delete( root ); 
		}
	}

	*Status = error;
	if( 0 <= error ) { return RET_NOERR;}
	
	ErrorListing ( MSG_ERROR, error, " InfoParse" );  
	return RET_ERROR;
}
*/
/*****************************************************************************************************/
WORKINGCODESYS_cJSONParse( char *cJSONInfo, char *KeyValue )
{
	int  error=DESERR_PARAM;  
	if( NULL != cJSONInfo &&  NULL != KeyValue )
	{
	 	error=DESERR_CJSON;
		cJSON *root, *tmp_obj;
		root	= cJSON_Parse( cJSONInfo );
		if( NULL != root )
		if( (tmp_obj = cJSON_GetObjectItem( root, KeyValue )))
		{
			switch( tmp_obj->type )
			{
				case cJSON_Number:
					//StrToInt( tmp_obj->valuestring, &error ))   
					
					break;
					
				case cJSON_String:	
					//strcpy( tmp_obj->valuestring,  ); 	
					break;
					
				case cJSON_Array:
					cJSON_GetArraySize( tmp_obj);
					//cJSON *cJSON_GetArrayItem(cJSON *array,int item);	
					break;
					
				case cJSON_Object:
					break;
					
				default: 
					break;	
			}
		}
			
	}
	char info[100]; 
	strcpy( info, " cJSONParse_");   
	strcat( info, cJSON_GetErrorPtr( ));
	
	ErrorListing ( MSG_ERROR, error, info );  
	return RET_ERROR;	
}
/*****************************************************************************************************/
/*****************************************************************************************************/

