#include <windows.h> 
#include "toolbox.h"

#include <utility.h>
#include <cvirte.h>		
#include <userint.h>


#include "UserPanel.h"
#include "MySQLITE.h"
/**************************************************************************************************************************/  
char sysRootPath[300];   
char *DataBaseName = "BlastingInfo"; 
char *UserTable    = "UserTable"; 
char *DBKey        = "cetcoil";
int  userTableID = 0;
/**************************************************************************************************************************/  
int CVICALLBACK UserLoginCallBack (int panel, int control, int event, void *callbackdata, int eventdata1, int eventdata2)
{
	if( control == USER_PANEL_PSW_STRING || control == USER_PANEL_NAME_STRING )
	{
		switch (event) 
		{  
			case EVENT_KEYPRESS:
			{
				if( 1280 == eventdata1 ) //'ENTRY' key in 
				{
					char name[20], psw[20];
					char **RequestData;  
					int error=0, num=0; 
					
					GetCtrlAttribute( panel, USER_PANEL_NAME_STRING, ATTR_CTRL_VAL, name );  
					GetCtrlAttribute( panel, USER_PANEL_PSW_STRING,  ATTR_CTRL_VAL, psw  ); 
						
					if( strlen(name) )
					if( error = -1 )
					if(  num = SQLITE_SearchRequest( DataBaseName, UserTable, "name", name, "password", &RequestData ))
					if( RequestData )
					if( 0 == strcmp( psw,  *RequestData))
					if( 0 == strcmp( name, "admin"))  	
					{
						error=0;
						SQLITE_Decrypt ( DataBaseName, DBKey );
						
						for( int i=0; i<num; i++ )
	 					{
							if( RequestData[i] ) {free(RequestData[i]); RequestData[i] =NULL;}
							free(RequestData); RequestData = NULL;    
						}
					}
					
					if( 0 == error )
					{
						int mainPanelHandle; 
						if( (mainPanelHandle = LoadPanel ( 0, "100_UserFunction\\UserPanel.uir", MAIN_PANEL)) > 0 )
						{
							RECT rt;
							//nScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
							//nScreenHeight = GetSystemMetrics(SM_CYSCREEN);  
				 			SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);    
							int ScreenHeight = rt.bottom-25; 
						  int ScreenWidth  = rt.right;
						
							SetPanelAttribute(  mainPanelHandle, ATTR_HEIGHT, ScreenHeight ); 
							SetPanelAttribute(  mainPanelHandle, ATTR_WIDTH,  ScreenWidth );   
							SetPanelAttribute(  mainPanelHandle, ATTR_LEFT,   0 ); 
							SetPanelAttribute(  mainPanelHandle, ATTR_TOP,    20 );
						
							DisplayPanel (mainPanelHandle);
							CallPanelCallback ( mainPanelHandle, EVENT_PANEL_RESTORE, ScreenWidth, ScreenHeight, 0 ); 
							DiscardPanel (panel);  
						}
					}
					else
					{
						MessagePopup( "Warning", "用户名或密码错误，请重新输入"); 
					}
				}
			}
			break; 
			
			default: 
			break; 
		}
	}
	return(0);
}

/**************************************************************************************************************************/   
int SystemFileFolderInit( void )
{
	if( sysRootPath )
	{
		char FilePath[100];
		char DownloadResult[100]; 
		char ErrorReport   [100];  
		char ResultReport  [100];  
		
		strcpy  ( FilePath, sysRootPath ); 
		strcat  ( FilePath, "\\900_ResultFile" );
		if( 1 > FileExists ( FilePath, 0 ))   MakeDir( FilePath );
		
		strcpy( DownloadResult, FilePath );
		strcat( DownloadResult, "\\DownloadResult" );		
		if( 1 > FileExists ( DownloadResult, 0 ))   MakeDir( DownloadResult );
		
		strcpy( ErrorReport, FilePath );
		strcat( ErrorReport, "\\ErrorReport" );		 
		if( 1 > FileExists ( ErrorReport, 0 ))      MakeDir( ErrorReport );
		
		strcpy( ResultReport, FilePath );
		strcat( ResultReport, "\\ResultReport" );		 
		if( 1 > FileExists ( ResultReport, 0 ))     MakeDir( ResultReport );
		
	}
	return 0;
}
/**************************************************************************************************************************/   
int CVICALLBACK UserPanelCallBack (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:
		{
			 if( 0 == userTableID )
			 {	 
				 
				 char *userTableItem[] ={ "type", "password", "name", "" };
				 
				 SYSTEM_GetPath ( sysRootPath ); 
				 SystemFileFolderInit(  );
				 SQLITE_Encrypt ( DataBaseName, DBKey ); 
				 userTableID = SQLITE_OpenDataBase ( sysRootPath, DataBaseName, UserTable, userTableItem );
				 
				 char **RequestData; int num;
				 if( 0 == (num = SQLITE_SearchRequest( DataBaseName, UserTable, "name", "admin", "*", &RequestData )) )
				 {
					 char *DataItemList[500];  
					 DataItemList[0] = "1,admin,admin"; 
					 SQLITE_InsertIntoDB ( userTableID, DataItemList, 1, 5 );
					}
				
				 for( int i=0; i<num; i++ )
				 {
					 if( RequestData[i] ) {free(RequestData[i]); RequestData[i] =NULL;}
				 	 free(RequestData); RequestData = NULL;    
				 }
				 
				 if( userTableID <= 0 )
				 {
				  	MessagePopup( "Error", "数据库开启错误，请重新启动程序"); 
						CallPanelCallback ( panel, EVENT_CLOSE, 0, 0, 0 );  
						userTableID = -1;
				 }
			 }
		}
		break;
		
		case EVENT_CLOSE:
		{
			userTableID = 0; 
			QuitUserInterface (0);
		}
		break;
		
		case EVENT_PANEL_SIZE:
		{
			int HeightValue, WidthValue, LeftValue, TopValue;
			int CtrHeightValue, CtrWidthValue, CtrLeftValue, CtrTopValue; 
			int updateHeight, updateWidth, updateLeft, updateTop; 
			
			/**Get panel size**/ 
			GetPanelAttribute ( panel, ATTR_HEIGHT, &HeightValue);
			GetPanelAttribute ( panel, ATTR_WIDTH,  &WidthValue ); 

			/**Set panel size**/    
			GetCtrlAttribute	( panel, USER_PANEL_TEXTMSG, ATTR_WIDTH,  &CtrWidthValue );
			CtrLeftValue = WidthValue/2 - CtrWidthValue/2;
			SetCtrlAttribute ( panel, USER_PANEL_TEXTMSG,   ATTR_LEFT, CtrLeftValue ); 

			GetCtrlAttribute	( panel, USER_PANEL_TEXTMSG_4, ATTR_HEIGHT,  &CtrHeightValue );  
			GetCtrlAttribute	( panel, USER_PANEL_TEXTMSG_4, ATTR_WIDTH,   &CtrWidthValue );
			CtrTopValue  = HeightValue   - CtrHeightValue*2;   
			CtrLeftValue = WidthValue/2  - CtrWidthValue/2;   
			SetCtrlAttribute ( panel, USER_PANEL_TEXTMSG_4,   ATTR_LEFT, CtrLeftValue );
			SetCtrlAttribute ( panel, USER_PANEL_TEXTMSG_4,   ATTR_TOP,  CtrTopValue );   
			
			GetCtrlAttribute	( panel, USER_PANEL_NAME_STRING, ATTR_HEIGHT,  &CtrHeightValue );  
			GetCtrlAttribute	( panel, USER_PANEL_NAME_STRING, ATTR_WIDTH,  &CtrWidthValue );
			CtrTopValue  = HeightValue/2   - CtrHeightValue; 
			CtrLeftValue = WidthValue/2 - CtrWidthValue/2;   
			SetCtrlAttribute ( panel, USER_PANEL_NAME_STRING,   ATTR_LEFT, CtrLeftValue );
			SetCtrlAttribute ( panel, USER_PANEL_NAME_STRING,   ATTR_TOP,  CtrTopValue ); 

			GetCtrlAttribute	( panel, USER_PANEL_PSW_STRING, ATTR_HEIGHT,  &CtrHeightValue );  
			GetCtrlAttribute	( panel, USER_PANEL_PSW_STRING, ATTR_WIDTH,  &CtrWidthValue );
			CtrTopValue  = HeightValue/2   + CtrHeightValue;  
			CtrLeftValue = WidthValue/2 - CtrWidthValue/2;   
			SetCtrlAttribute ( panel, USER_PANEL_PSW_STRING,   ATTR_LEFT, CtrLeftValue );
			SetCtrlAttribute ( panel, USER_PANEL_PSW_STRING,   ATTR_TOP,  CtrTopValue ); 		
			
			GetCtrlAttribute	( panel, USER_PANEL_TEXTMSG_2, ATTR_HEIGHT,  &CtrHeightValue );   
		  GetCtrlAttribute	( panel, USER_PANEL_TEXTMSG_2, ATTR_WIDTH,  &CtrWidthValue );
			CtrTopValue  = HeightValue/2   - CtrHeightValue;         
			CtrLeftValue = WidthValue/2 - CtrWidthValue/2-120;   
			SetCtrlAttribute ( panel, USER_PANEL_TEXTMSG_2,   ATTR_LEFT, CtrLeftValue );
			SetCtrlAttribute ( panel, USER_PANEL_TEXTMSG_2,   ATTR_TOP,  CtrTopValue ); 
			
			GetCtrlAttribute	( panel, USER_PANEL_TEXTMSG_3, ATTR_HEIGHT,  &CtrHeightValue );   
		  GetCtrlAttribute	( panel, USER_PANEL_TEXTMSG_3, ATTR_WIDTH,  &CtrWidthValue );
			CtrTopValue  = HeightValue/2   + CtrHeightValue;       
			CtrLeftValue = WidthValue/2 - CtrWidthValue/2-120;   
			SetCtrlAttribute ( panel, USER_PANEL_TEXTMSG_3,   ATTR_LEFT, CtrLeftValue );
			SetCtrlAttribute ( panel, USER_PANEL_TEXTMSG_3,   ATTR_TOP,  CtrTopValue ); 
		
			/**Picture auto sizing**/
			//SetCtrlAttribute	( panel, USER_PANEL_PICTURE, ATTR_HEIGHT, HeightValue);
			//SetCtrlAttribute	( panel, USER_PANEL_PICTURE, ATTR_WIDTH,  WidthValue ); 
			
		}
			break;
	}
	return 0;
}
/**************************************************************************************************************************/   
