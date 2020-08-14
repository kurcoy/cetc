#include <windows.h>   
#include "toolbox.h"
#include <utility.h>
#include <ansi_c.h>
#include <userint.h>
#include "UserPanel.h"
#include "mysqlite.h"   
#include "ComPort.h"
#include <math.h>
#include "useroperation.h"    

#define  pii 3.14159265358979323846264338327950288419717
#define  rad_mean    6371
#define  rad_equator 6378137
#define  rad_pole    6356725 

HWND gExHwnd = NULL;  

PROCESS_INFORMATION info;   
HANDLE hFileMapping; 
char* sharedMemery;

extern char sysRootPath[300]; 
extern char SerialPort[10];

extern char *DataBaseName;
extern char *ResultTable;
extern char *RuleTable;
extern char *InfoTable;

extern int  NumOfUIDs;   
extern char **UIDArray;  
extern int  ComPortID;

extern char  xmbh[20], htid[20];

/*
char sbbh[20], jd[20], wd[20], gpstime[20];
extern int GetInstrumentIDByComport ( int ComID, char sbbh[20]); //119.499578 36.545535
extern int GetLocationByComport     ( int ComID, char jd[20], char wd[20], char gpstime[20] ); 
extern int StrTimeToCalenderTime    (  char *strTime,  struct tm *cTime );  
*/
/**************************************************************************************************************************/   
/*
double CheckTimeLimitation( char* strTime )
{
 	struct tm str_Time;  	
	if( strTime   ) 
	{	   
		int error    = -1;  
		int daycount = 3;
		double delta = 0;
		
	  time_t time_T;
		time ( &time_T ); //may will use the time from GPS or controller
		StrTimeToCalenderTime ( strTime,   &str_Time );
		time_t time_count   = mktime( &str_Time   ); 
		
		return delta = difftime(time_T, time_count); 
	}
	return 0;
}
*/
/**************************************************************************************************************************/ 
/*
double CalculateDistance( double a_longitude, double a_latitude, double b_longitude, double b_latitude )
{
	//double a_latitude=36.545535,b_latitude=40,a_longitude=119.499578,b_longitude=120; 
  double dLat = (b_latitude  - a_latitude)  * pii / 180.0;
  double dLon = (b_longitude - a_longitude) * pii / 180.0;

  // Converte para Radianos
  double lat1 = (a_latitude) * pii / 180.0;
  double lat2 = (b_latitude) * pii / 180.0;

  // Formula
  double aa = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);
  double c = 2 * asin(sqrt(aa));
  return c =  rad_equator * c;
}
*/
/**************************************************************************************************************************/  
BOOL CALLBACK EnumWindowsProcGetWndTitle(HWND hWnd,LPARAM lParam)
{
	DWORD ProcID = 0;
	GetWindowThreadProcessId(hWnd,&ProcID);
	if(ProcID==lParam)
	{
		//gExHwnd = hWnd;
		//return FALSE;
	
		HWND pWnd = GetParent(hWnd);
		while(GetParent(pWnd)!=NULL)
		{
			pWnd = GetParent(pWnd);
			gExHwnd = pWnd;
		return FALSE;
		}

	}
	return TRUE;
}
/**************************************************************************************************************************/ 
/*
int CheckBlastingRule( char* xmbh )
{
	int error=-1; 
	double currjd, currwd;
	
	if( RET_NOERR == COMPORT_Open     ( ComPortID ))	  
	if( 0 == GetInstrumentIDByComport ( ComPortID, sbbh ))
	if( 0 == GetLocationByComport     ( ComPortID, jd, wd, gpstime )) 
	if( RET_NOERR == COMPORT_Close    ( ComPortID ))	     	
	{				
		currjd = atof( jd );
		currwd = atof( wd );     

		char **RequestData;
		int num = SQLITE_SearchRequest( DataBaseName, RuleTable, "xmbh", xmbh, "*", &RequestData );
		if( num>0 && RequestData )
		{
			double jbjd, jbwd, jbbj, zbjd, zbwd, zbbj; 
			char jbsj[20], zbqssj[20], zbjssj[20]; 
			int index=0; 
			
			const char s[2] = ",";
	    char *token;
			token = strtok(*RequestData, s);

	    while( token != NULL ) 
			{
				switch( index++)
				{
					case 1:  jbjd = atof( token );   break;
					case 2:  jbwd = atof( token );   break;    
					case 3:  jbbj = atof( token );   break;    
					case 4:  strcpy(jbsj, token) ; 	 break;    
					case 6:  zbjd = atof( token ); 	 break; //116.4431950000000029   
					case 7:  zbwd = atof( token );   break; //37.4243450000000024
					case 8:  zbbj = atof( token );   break;   
					case 9:  strcpy(zbqssj, token) ; break;    
					case 10: strcpy(zbjssj, token) ; break;      
				}
	      token = strtok( NULL, s );
			}

			double jbdis =  CalculateDistance  ( currjd, currwd, jbjd, jbwd);     
			double zbdis =  CalculateDistance  ( currjd, currwd, zbjd, zbwd); //288.5076235604772137, 288.326, 289.04   

			//int timeCheck = CheckTimeLimitation( jbsj, zbqssj, zbjssj );
			double t_jb   = CheckTimeLimitation( jbsj   );   
			double t_zbqs = CheckTimeLimitation( zbqssj ); 
			double t_zbjs = CheckTimeLimitation( zbjssj ); 
			
			if( (error=-2) && jbbj  < jbdis )
			if( (error=-3) && zbbj  > zbdis ) 
			if( (error=-4) && t_jb  > 0     )
			if( (error=-5) && t_zbqs > 0    )
			if( (error=-5) && t_zbjs < 0    ) 	
			{
				//compare time
			}
				
			if( RequestData )
			{
				for( int i=0; i<num; i++ )
				{
					if( RequestData[i] ) { free(RequestData[i]); RequestData[i] = NULL; } //288.494km/ 179.262 
				}
				free(RequestData); RequestData = NULL;
			}

		}
	}
	
	return error;
}
*/
/**************************************************************************************************************************/  
int SendInformaitonToShareMemory( char** sharedFile )
{
	char *pbFile;
	int  BufferSize=4096; //30byte/pc, max quantity = 40, occupiedsize = 40*30 = 1200 byte
	
	//read from local database
	char **param = (char**) calloc( NumOfUIDs+1, sizeof(char*) );
	char **RequestData; 
	int num, qty;
	
	for( int i=0; i<NumOfUIDs; i++ )
	{
		num = SQLITE_SearchRequest( DataBaseName, InfoTable, "UID", UIDArray[i], "gzm", &RequestData ); //rule table
		if( num>0 && RequestData )
		{
			param[i] = calloc( 30, sizeof(char) ); 
			sprintf( param[i], "UID:%sPSW:%s", UIDArray[i], *RequestData );
			
			for( int i=0; i<num; i++ )
			{
				if( RequestData[i] ) {free(RequestData[i]); RequestData[i] =NULL;}
				free(RequestData); RequestData = NULL;    
			}
			
		}
	}
	param[NumOfUIDs]= 0x00;
	qty=NumOfUIDs;
	/*   	
	qty=51; 
	char *param[] = { "UID:1000000000001PSW:12345678", "UID:1000000000002PSW:12345678", "UID:1000000000003PSW:12345678", "UID:1000000000004PSW:12345678",
										"UID:1000000000005PSW:12345678", "UID:1000000000006PSW:12345678", "UID:1000000000007PSW:12345678", "UID:1000000000008PSW:12345678",
										"UID:1000000000009PSW:12345678", "UID:1000000000010PSW:12345678", "UID:1000000000011PSW:12345678", "UID:1000000000012PSW:12345678",
										"UID:1000000000013PSW:12345678", "UID:1000000000014PSW:12345678", "UID:1000000000015PSW:12345678", "UID:1000000000016PSW:12345678",
										"UID:1000000000017PSW:12345678", "UID:1000000000018PSW:12345678", "UID:1000000000019PSW:12345678", "UID:1000000000020PSW:12345678",
										"UID:1000000000021PSW:12345678", "UID:1000000000022PSW:12345678", "UID:1000000000023PSW:12345678", "UID:1000000000024PSW:12345678",
										"UID:1000000000025PSW:12345678", "UID:1000000000026PSW:12345678", "UID:1000000000027PSW:12345678", "UID:1000000000028PSW:12345678",
										"UID:1000000000029PSW:12345678", "UID:1000000000030PSW:12345678", "UID:1000000000031PSW:12345678", "UID:1000000000032PSW:12345678",
										"UID:1000000000033PSW:12345678", "UID:1000000000034PSW:12345678", "UID:1000000000035PSW:12345678", "UID:1000000000036PSW:12345678",
										"UID:1000000000037PSW:12345678", "UID:1000000000038PSW:12345678", "UID:1000000000039PSW:12345678", "UID:1000000000040PSW:12345678",  
										"UID:1000000000041PSW:12345678", "UID:1000000000042PSW:12345678", "UID:1000000000043PSW:12345678", "UID:1000000000044PSW:12345678",  
										"UID:1000000000045PSW:12345678", "UID:1000000000046PSW:12345678", "UID:1000000000047PSW:12345678", "UID:1000000000048PSW:12345678",  
										"UID:1000000000049PSW:12345678", "UID:1000000000050PSW:12345678", "UID:1000000000051PSW:12345678", "UID:1000000000052PSW:12345678", 
										"" };   //26bytes for one
	*/	 
	hFileMapping = CreateFileMapping( 0xFFFFFFFF, NULL, PAGE_READWRITE,0, BufferSize, "BlastingInfo");
	pbFile = (char*)MapViewOfFile( hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, BufferSize);

	if( pbFile && qty > 0 )	
	{
		int index=0; 
		index += sprintf( &pbFile[index], "ServerStatus:%d\n",     1 ); 
		index += sprintf( &pbFile[index], "ClientStatus:%d\n",     1 ); 
	
		index += sprintf( &pbFile[index], "Quantity:%d\n", qty );
		index += sprintf( &pbFile[index], "Infos:\n" );   

		for( int i=0;i<qty;i++ )
		{
			if(i>0)	index += sprintf( &pbFile[index], "%s", "" ); 	     
			index += sprintf( &pbFile[index], "%s", param[i]   ); 	
		}
		
		*sharedFile = pbFile;   
		
		return 1;
		
	}
	return -1;
}	
/**************************************************************************************************************************/  
/*
int CVICALLBACK BlastingCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			BOOL  fSuccess    = 0; 
			int   nNumberDely = 1000;
			DWORD dwProcessId = 0;
			
			//check location and time rule
			CheckBlastingRule(  xmbh );
			
			if( 1 == SendInformaitonToShareMemory( &sharedMemery )) 
			if( sharedMemery )
			{
				char filename[500]; 
				char *appName = "ElecDetMonitor.exe";
				strcpy  ( filename, sysRootPath ); 
				strcat  ( filename, "\\800_External\\" );
				strcat  ( filename, appName );  
				
				LPCSTR cmdLine  = (LPCSTR)SerialPort;//" Server parameter passed in "; // leave space at the begining of char array	 //SerialPort
			
				STARTUPINFO si;
				memset( &si, 0, sizeof(si) );
		    si.cb = sizeof(si);
		    memset( &info, 0,  sizeof(info) );
			 
				fSuccess =1;
				fSuccess = CreateProcess((LPCSTR)filename,cmdLine,NULL,NULL,FALSE,NULL,NULL,NULL,&si,&info);
				//int nError = GetLastError();	
				if( 0 != fSuccess )
				{
					dwProcessId = info.dwProcessId;

					while(EnumWindows(EnumWindowsProcGetWndTitle,dwProcessId) && nNumberDely>0 )
					{
						nNumberDely--;
					}
					static int handle;
					GetPanelAttribute (panel, ATTR_SYSTEM_WINDOW_HANDLE, &handle);   
					//HWND hwnd = FindWindow((LPCSTR)NULL, (LPCSTR)"油气井用电子雷管控制系统1.10-EL"); 
					RECT rt;  
					SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);    
					int ScreenHeight = rt.bottom+25; 
				  int ScreenWidth  = rt.right;
					
					LONG lWindowStyle = GetWindowLong(gExHwnd, GWL_STYLE);
					lWindowStyle &= ~WS_SYSMENU; 
					lWindowStyle &= ~WS_CAPTION; 
					SetWindowLong(gExHwnd, GWL_STYLE, lWindowStyle);
					
					MoveWindow  ( gExHwnd,0, 0, ScreenWidth, ScreenHeight, FALSE);
					SetWindowPos( gExHwnd, HWND_BOTTOM, 0, 0, ScreenWidth, ScreenHeight, SWP_NOACTIVATE||SWP_SHOWWINDOW );  
					SetParent   ( gExHwnd, (HWND)handle);
					SetCtrlAttribute  ( panel, MAIN_PANEL_TIMER, ATTR_ENABLED, 1 ); 
				}
			}
			
			if( !fSuccess )
			{
				MessagePopup ( "Warning", "起爆程序开启错误，请重新启动程序！"); 
			}
		}
		break;
		
	}
	return 0;
}
*/
/**************************************************************************************************************************/  
int LoadBlastingApp ( int panel )
{
	int error = -1;
	BOOL  fSuccess    = 0; 
	int   nNumberDely = 1000;
	DWORD dwProcessId = 0;
	
	//check location and time rule
	//CheckBlastingRule(  xmbh );
	
	if( 1 == SendInformaitonToShareMemory( &sharedMemery )) 
	if( sharedMemery )
	{
		char filename[500]; 
		char *appName = "ElecDetMonitor.exe";
		strcpy  ( filename, sysRootPath ); 
		strcat  ( filename, "\\800_External\\" );
		strcat  ( filename, appName );  
		
		LPCSTR cmdLine  = (LPCSTR)SerialPort;//" Server parameter passed in "; // leave space at the begining of char array	
	
		STARTUPINFO si;
		memset( &si, 0, sizeof(si) );
    si.cb = sizeof(si);
    memset( &info, 0,  sizeof(info) );
	 
		fSuccess =1;
		fSuccess = CreateProcess((LPCSTR)filename,cmdLine,NULL,NULL,FALSE,NULL,NULL,NULL,&si,&info);
		//int nError = GetLastError();
		SYSTEM_SleepTime( 300 ); 	
		if( 0 != fSuccess )
		{
			dwProcessId = info.dwProcessId;

			while(EnumWindows(EnumWindowsProcGetWndTitle,dwProcessId) && nNumberDely>0 )
			{
				nNumberDely--;
			}
			static int handle;
			RECT rt;  
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);    
			int ScreenHeight = rt.bottom-25; 
		  int ScreenWidth  = rt.right;

			LONG lWindowStyle = GetWindowLong(gExHwnd, GWL_STYLE);
			GetPanelAttribute (panel, ATTR_SYSTEM_WINDOW_HANDLE, &handle);   
			SetParent   ( gExHwnd, (HWND)handle);
			
			lWindowStyle &= ~WS_SYSMENU; 
			lWindowStyle &= ~WS_CAPTION; 
			SetWindowLong(gExHwnd, GWL_STYLE, lWindowStyle);
			//MoveWindow  ( gExHwnd,0, 0, ScreenWidth, ScreenHeight, FALSE);
			
			//GetPanelAttribute (panel, ATTR_SYSTEM_WINDOW_HANDLE, &handle);   
			//SetParent   ( gExHwnd, (HWND)handle);
			//SYSTEM_SleepTime( 200 ); 
		
			SetWindowPos( gExHwnd, HWND_BOTTOM, 0, 0, ScreenWidth, ScreenHeight, SWP_NOACTIVATE || SWP_SHOWWINDOW ); // 

			error = 0;																												
		}
	}
	
	if( !fSuccess )
	{
		error = -1; 
		MessagePopup ( "Warning", "起爆程序开启错误，请重新启动程序！"); 
	}

	return error;
}
/**************************************************************************************************************************/  
int CVICALLBACK BlastingTimerCB (int panel, int control, int event, void *callbackData, int eventData1, int eventData2) 
{
	switch (event)
	{
		case EVENT_TIMER_TICK: 
		{
			if( sharedMemery )
			{
			 char buffer[4069];
			 strcpy( buffer, sharedMemery ); 
			 if( '3' == buffer[28] ) 
			 {
			 	 SetPanelAttribute( panel, ATTR_VISIBLE, 0 ); 	  		 
			 }
			 if( '4' == buffer[28] )
			 {
				 SetCtrlAttribute   ( panel, BST_PANEL_TIMER, ATTR_ENABLED, 0 );
				 CallCtrlCallback   ( panel, BST_PANEL_TIMER, EVENT_DISCARD, 0, 0, 0 );  
			 }
			}
		}
		break;
		
		case EVENT_DISCARD:
		{
			if( sharedMemery ) { UnmapViewOfFile(sharedMemery); sharedMemery = NULL; }
			if( hFileMapping ) { CloseHandle    (hFileMapping); hFileMapping = NULL; }
			
			//SetPanelAttribute( panel, ATTR_VISIBLE, 1 );  
		}
		
	}
	return 0;
}
/**************************************************************************************************************************/
int CVICALLBACK BlastingPanelCallBack (int panel, int event, void *callbackData, int eventData1, int eventData2)   
{
	switch (event)
	{
		case EVENT_PANEL_RESTORE: 
		{
			int parentPanel;
			
		  SetCtrlAttribute  ( panel, BST_PANEL_TIMER, ATTR_ENABLED, 1 );
			GetPanelAttribute ( panel, ATTR_PANEL_PARENT, &parentPanel  );  
	  	LoadBlastingApp   ( parentPanel ); 
		}
			break;
		case EVENT_LOST_FOCUS:

			break;
			
		case EVENT_DISCARD: case EVENT_CLOSE:
		{
			//SetWindowPos( gExHwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE||SWP_HIDEWINDOW );   
			if( gExHwnd ) SendMessage(gExHwnd, WM_CLOSE , 0, 0); gExHwnd = 0;
			//CloseHandle ( info.hProcess );
			//CloseHandle ( info.hThread );
			 
			if( sharedMemery ) { UnmapViewOfFile(sharedMemery); sharedMemery = NULL; }
			if( hFileMapping ) { CloseHandle(hFileMapping);     hFileMapping = NULL; }
			
			SetCtrlAttribute   ( panel, BST_PANEL_TIMER, ATTR_ENABLED, 0 );  
		}
		break;
	}
	return 0;	
}
/**************************************************************************************************************************/   
