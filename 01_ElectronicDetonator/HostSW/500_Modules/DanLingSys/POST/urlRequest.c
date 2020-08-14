//==============================================================================
//
// Title:		urlRequest.c
// Purpose:		A short description of the implementation.
//
// Created on:	2020/5/11 at 17:42:27 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include "urlRequest.h"
#include <ansi_c.h>
#include <stdio.h>
#include <string.h>
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
static int respSize=0; 

//==============================================================================
// Global functions

/***************************************************************************************************************/
//url post by libcurl
/***************************************************************************************************************/ 
int write_data(void *buffer, int size, int nmemb, void **fptr ) 
{
	if( NULL != fptr )
	{
		//int written = fwrite(buffer, size, nmemb, (FILE*)fptr);
		//strncpy( (char*)resp, (char*)buffer, size*nmemb);
		//char *resp = (char*)fptr;
		respSize = size*nmemb;  
		char *resp = (char*) calloc( sizeof(char), respSize+1);	//realloc
		*fptr = resp;
		memcpy( resp, buffer, respSize); 
		resp[respSize] = '\0';
		return respSize;
	}
	return -1;
}

int urlRequest_Post( char *url, char *Params, char** Response, int *ResLength )
{
	if( NULL != url && NULL !=Params && NULL != Response && NULL != ResLength )
	{
		CURL *curl;
	  CURLcode res;
	  FILE *fptr;
		int error=-1;
		
		struct curl_slist *headers = NULL;   
		
	  /* In windows, this will init the winsock stuff */ 
	  curl_global_init(CURL_GLOBAL_ALL);
 		curl = curl_easy_init();
	  if(curl) 
		{
	  	//fptr=fopen( "respone.txt", "wb");
			//*Response = (char*)calloc(sizeof(char), 1024);   
			if( 1 )
			{
				respSize = 0;
				
				//urlencode post data 
				char *param_encode = curl_easy_escape(curl, Params, strlen(Params));
		    if (!param_encode) { param_encode = Params;}
				
				int   param_len  = strlen(param_encode)+strlen("param=");
				char *param_post = calloc( param_len+1, sizeof(char));   
				strcpy( param_post, "param=");
				strcat( param_post, param_encode);
				param_post[param_len] = '\0';
				
				curl_easy_setopt(curl, CURLOPT_POST, 1);     
		 		curl_easy_setopt(curl, CURLOPT_URL, url);	
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, param_post);
		  	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		  	curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA,  Response);
		  	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers  );
				
		  	res = curl_easy_perform(curl);
		
		    if(res != CURLE_OK)
				{
					*Response = (char*)calloc(sizeof(char), 1024);   
					strcpy( *Response, curl_easy_strerror(res));
					*ResLength = -1;
				}
				else
				{
					//strncpy( Response, fptr->buffer, respSize );
					*ResLength = respSize;
					error = 0;
				}
					 
				//if( param_encode ) free(param_encode);
				 if( param_post ) { free(param_post); param_post = NULL; } 
			}
 		
			//fclose(fptr);

	    /* always cleanup */ 
	    curl_easy_cleanup(curl);
	  }
	  curl_global_cleanup();
	  if(!error) return 1;
	}
	return -1;
}	
/***************************************************************************************************************/
//urlencode and urldecode
/***************************************************************************************************************/ 
#define BURSIZE 2048

int hex2dec(char c)
{
    if ('0' <= c && c <= '9') 
    {
        return c - '0';
    } 
    else if ('a' <= c && c <= 'f')
    {
        return c - 'a' + 10;
    } 
    else if ('A' <= c && c <= 'F')
    {
        return c - 'A' + 10;
    } 
    else 
    {
        return -1;
    }
}

char dec2hex(short int c)
{
    if (0 <= c && c <= 9) 
    {
        return c + '0';
    } 
    else if (10 <= c && c <= 15) 
    {
        return c + 'A' - 10;
    } 
    else 
    {
        return -1;
    }
}


int urlRequest_Encode( unsigned char *urlIn, unsigned char **urlEncoded, int *lens)
{
    int i = 0;
    int len = strlen(urlIn);
    int res_len = 0;
    char res[BURSIZE];
    for (i = 0; i < len; ++i) 
    {
        char c = urlIn[i];
        if (    ('0' <= c && c <= '9') ||
                ('a' <= c && c <= 'z') ||
                ('A' <= c && c <= 'Z') || 
                c == '/' || c == '.') 
        {
            res[res_len++] = c;
        } 
        else 
        {
            int j = (short int)c;
            if (j < 0)
                j += 256;
            int i1, i0;
            i1 = j / 16;
            i0 = j - i1 * 16;
            res[res_len++] = '%';
            res[res_len++] = dec2hex(i1);
            res[res_len++] = dec2hex(i0);
        }
    }
    res[res_len] = '\0';
		*lens = res_len; 
		if( res_len > 0 )
		{
			*urlEncoded = calloc( res_len+1, sizeof(char)); 
			memcpy( *urlEncoded, res, res_len+1 );  
			return 1; 
		}
		else
		{
			return -1;
		}
    //strcpy(url, res);
}


void urlRequest_Dencode( char *url )
{
    int i = 0;
    int len = strlen(url);
    int res_len = 0;
    char res[BURSIZE];
    for (i = 0; i < len; ++i) 
    {
        char c = url[i];
        if (c != '%') 
        {
            res[res_len++] = c;
        }
        else 
        {
            char c1 = url[++i];
            char c0 = url[++i];
            int num = 0;
            num = hex2dec(c1) * 16 + hex2dec(c0);
            res[res_len++] = num;
        }
    }
    res[res_len] = '\0';
    strcpy(url, res);
}
/***************************************************************************************************************/
/***************************************************************************************************************/
