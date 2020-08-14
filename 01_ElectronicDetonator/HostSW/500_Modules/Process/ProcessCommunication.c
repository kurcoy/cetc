//==============================================================================
//
// Title:		Porcess.c
// Purpose:		A short description of the implementation.
//
// Created on:	2020/4/13 at 10:23:15 by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

//#include "Porcess.h"

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

#include <stdio.h> 
#include <windows.h> 
#include "ProcessCommunication.h"

#define BUFSIZE 4096 
 
HANDLE hInputFile, hStdout; 
 
int CreatePipeOut(HANDLE *hChildStdoutRdDup, HANDLE *hChildStdoutWr); 
int CreatePipeIn (HANDLE *hChildStdinRd, HANDLE *hChildStdinWrDup);        

BOOL CreateChildProcess(const char *ProcessName, HANDLE *hChildStdinWrDup, HANDLE *hChildStdoutRdDup, HANDLE *hChildStdoutWr); 
void WriteToPipe (HANDLE hChildStdinWrDup ); 
void ReadFromPipe(HANDLE hChildStdoutRdDup, HANDLE hChildStdoutWr, char result[BUFSIZE]);   
void ErrorExit(LPTSTR);

/*************************************************************************************/
/*************************************************************************************/ 
char* test( const char *ProcessName, const char *filename )
{
	 BOOL fSuccess;  
// Now create the child process.
   char result[BUFSIZE];
	 HANDLE hChildStdinWrDup, hChildStdoutRdDup, hChildStdoutWr;
	 
	 fSuccess = CreateChildProcess( ProcessName, &hChildStdinWrDup, &hChildStdoutRdDup, &hChildStdoutWr);
   if (! fSuccess) 
      ErrorExit("Create process failed"); 

   hInputFile = CreateFile(filename, GENERIC_READ, 0, NULL, 
      OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL); 
 
   if (hInputFile == INVALID_HANDLE_VALUE) 
      ErrorExit("CreateFile failed\n"); 
 
// Write to pipe that is the standard input for a child process. 
 
   WriteToPipe(hChildStdinWrDup); 
 
// Read from pipe that is the standard output for child process. 
 
   ReadFromPipe(hChildStdoutRdDup, hChildStdoutWr,result); 
 
	 CloseHandle( hInputFile );
	 
   return result; 
} 
/*************************************************************************************/
/*************************************************************************************/ 
int CreatePipeOut(HANDLE *hChildStdoutRdDup, HANDLE *hChildStdoutWr) 
{ 
   SECURITY_ATTRIBUTES saAttr; 
   BOOL fSuccess; 
	 HANDLE hChildStdoutRd;
 	 int error=0;
	 
// Set the bInheritHandle flag so pipe handles are inherited. 
   saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
   saAttr.bInheritHandle = TRUE; 
   saAttr.lpSecurityDescriptor = NULL; 
 
// Get the handle to the current STDOUT. 
   hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
 
// Create a pipe for the child process's STDOUT
// Create noninheritable read handle and close the inheritable read handle.       
   if( error =1 && 0 != CreatePipe( &hChildStdoutRd, hChildStdoutWr, &saAttr, 0)) 
	 if( error =2 && 0 != DuplicateHandle(GetCurrentProcess(), hChildStdoutRd, GetCurrentProcess(), hChildStdoutRdDup , 0, FALSE, DUPLICATE_SAME_ACCESS) )	 
	 {
		 error=0;		 
	 }	 
   
	 if(error)
	 {
		 switch(error)
		 {
			 case 1:
			 {
				 ErrorExit("Stdout pipe creation failed\n");    
			 }
			 break; 
			 
			 case 2:
			 {
				 ErrorExit("DuplicateHandle failed");     
			 }
			 break; 
			 
			 default: 
				 break;
		 }
	 }

   CloseHandle(hChildStdoutRd);

		return error; 
}	
/*************************************************************************************/ 
/*************************************************************************************/ 
int CreatePipeIn(HANDLE *hChildStdinRd, HANDLE *hChildStdinWrDup)  	
{		
// Create a pipe for the child process's STDIN. 
   SECURITY_ATTRIBUTES saAttr; 
   BOOL fSuccess;
	 HANDLE hChildStdinWr;
	 int error=0;  
	 
// Set the bInheritHandle flag so pipe handles are inherited. 
   saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
   saAttr.bInheritHandle = TRUE; 
   saAttr.lpSecurityDescriptor = NULL; 
 
// Get the handle to the current STDOUT. 
   hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
 
// Create a pipe for the child process's STDOUT
// Create noninheritable read handle and close the inheritable read handle.       
   if( error =1 && 0 !=  CreatePipe(hChildStdinRd, &hChildStdinWr, &saAttr, 0)) 
	 if( error =2 && 0 != DuplicateHandle(GetCurrentProcess(), hChildStdinWr, GetCurrentProcess(), hChildStdinWrDup, 0, FALSE, DUPLICATE_SAME_ACCESS) )	 
	 {
		 error=0;		 
	 }	 
   
	 if(error)
	 {
		 switch(error)
		 {
			 case 1:
			 {
				 ErrorExit("Stdout pipe creation failed\n");    
			 }
			 break; 
			 
			 case 2:
			 {
				 ErrorExit("DuplicateHandle failed");     
			 }
			 break; 
			 
			 default: 
				 break;
		 }
	 }

   CloseHandle(hChildStdinWr);
		return error; 
}
/*************************************************************************************/ 
/*************************************************************************************/
BOOL CreateChildProcess( const char *ProcessName, HANDLE *hChildStdinWrDup, HANDLE *hChildStdoutRdDup, HANDLE *hChildStdoutWr ) 
{ 
  PROCESS_INFORMATION piProcInfo; 
  STARTUPINFO siStartInfo;
  BOOL bFuncRetn = FALSE; 
	
	HANDLE hChildStdinRd;

	CreatePipeOut( hChildStdoutRdDup, hChildStdoutWr); 
  
	CreatePipeIn ( &hChildStdinRd,  hChildStdinWrDup);
	
// Set up members of the PROCESS_INFORMATION structure. 
   ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
 
// Set up members of the STARTUPINFO structure. 
 
   ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
   siStartInfo.cb = sizeof(STARTUPINFO); 
   siStartInfo.hStdError = *hChildStdoutWr;
   siStartInfo.hStdOutput = *hChildStdoutWr;
   siStartInfo.hStdInput = hChildStdinRd;
   siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
 
	 LPCSTR cmdLine  = (LPCSTR)" this is the parameter passed in ";
		
// Create the child process. 
    
   bFuncRetn = CreateProcess( ProcessName, 
      cmdLine,       // command line 
      NULL,          // process security attributes 
      NULL,          // primary thread security attributes 
      TRUE,          // handles are inherited 
      0,             // creation flags 
      NULL,          // use parent's environment 
      NULL,          // use parent's current directory 
      &siStartInfo,  // STARTUPINFO pointer 
      &piProcInfo);  // receives PROCESS_INFORMATION 
  
	 CloseHandle(hChildStdinRd  );    
	 
   if (bFuncRetn == 0) 
      ErrorExit("CreateProcess failed");
   else 
   {
      CloseHandle(piProcInfo.hProcess);
      CloseHandle(piProcInfo.hThread);
      return bFuncRetn;
   }
}
/*************************************************************************************/ 
/*************************************************************************************/ 
void WriteToPipe( HANDLE hChildStdinWrDup ) 
{ 
   DWORD dwRead, dwWritten; 
   CHAR chBuf[BUFSIZE]; 
 
// Read from a file and write its contents to a pipe. 
 
   for (;;) 
   { 
      if (! ReadFile(hInputFile, chBuf, BUFSIZE, &dwRead, NULL) || 
         dwRead == 0) break; 
      if (! WriteFile(hChildStdinWrDup, chBuf, dwRead, 
         &dwWritten, NULL)) break; 
   } 
 
// Close the pipe handle so the child process stops reading. 
 
   if (! CloseHandle(hChildStdinWrDup)) 
      ErrorExit("Close pipe failed"); 
} 

/*************************************************************************************/ 
/*************************************************************************************/ 
void ReadFromPipe( HANDLE hChildStdoutRdDup, HANDLE hChildStdoutWr, char result[BUFSIZE]) 
{ 
   DWORD dwRead, dwWritten; 
   CHAR chBuf[BUFSIZE]; 

// Close the write end of the pipe before reading from the 
// read end of the pipe. 

   if (!CloseHandle(hChildStdoutWr)) 
      ErrorExit("CloseHandle failed"); 

// Read output from the child process, and write to parent's STDOUT. 
 
   for (;;) 
   { 
      if( !ReadFile( hChildStdoutRdDup, chBuf, BUFSIZE, &dwRead, 
         NULL) || dwRead == 0) break; 
      if (! WriteFile(hStdout, chBuf, dwRead, &dwWritten, NULL)) 
         break; 
   }
	 
	 strcpy( result, chBuf );
	 
} 
 
void ErrorExit (LPTSTR lpszMessage) 
{ 
   fprintf(stderr, "%s\n", lpszMessage); 
   ExitProcess(0); 
}


