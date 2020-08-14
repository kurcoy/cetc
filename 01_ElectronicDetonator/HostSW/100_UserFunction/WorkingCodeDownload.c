#include <userint.h>
#include <cvidef.h>
#include <ansi_c.h>
#include "UserPanel.h"      
#include "WorkingCodeSys.h" 
#include <windows.h> 
#include "toolbox.h"
#include <winspool.h>  
#include "mysqlite.h" 
#include "comport.h"
#include "useroperation.h"    
/**************************************************************************************************************************/        
//TODO:

//HostSW optimization 
//test for process 
//validation for protocol 
//SW for blasting process and MCU embedding 

//network request timeout
//--reminder

//how to monitor location changing, selfcheck within 500 m, time interval < 1/5mins

//switch panel by step button

//upload every one or by file, xmbh should be the same within one file. all will be changed at the same time

//auto read result file and save into db

 //check file already open

/**************************************************************************************************************************/
#define  pii 3.14159265358979323846264338327950288419717
#define  rad_mean    6371
#define  rad_equator 6378137
#define  rad_pole    6356725 

#define FILE_KEY_LEN  6
extern int MAX_DET_NUM;
int   WCDLPanelHandle    = 0;
extern int  BlastingEnable;  

extern char SerialPort[10];   
extern int infoTableID, ruleTableID, resultTableID;     
extern char *DataBaseName;
extern char *ResultTable;
extern char *RuleTable;
extern char *InfoTable;
	 
extern int NumOfUIDs; 
int   UIDLen    = 13;
char *UIDHeader = "73";
int   rangeHeight=10;
int   rangeWidth =8;   			
int   cellnumber = 0;
static int DownloadThreadID=0; 
static int ThreadRunning = 0;
static int ThreadPanel   = 0;
static int DownLoadType  = 0;   
static int numOfFiles    = 0; 

extern int ComPortID;

char **fileList    = NULL; 
char **fileKeyList = NULL;      

extern char  **UIDArray;
char  **ExistedUID; 
static char  **WorkcodeInfo; //**valueArray
char   sbbh[20],jd[20],wd[20], gpstime[20], xmbh[20], htid[20], dwdm[20];
unsigned char Commmand_InstID[36]; 
unsigned char Commmand_location[36];

static int ExistedSel  = -1;
static int ExistedNum  = 0; 
static int NumOfWCInfo = 7;

char* DownLoadUrl = "http://test.mbdzlg.com/mbdzlgtxzx/servlet/DzlgMmxzJsonServlert"; 
extern unsigned char KeyStr[24]; 
extern unsigned char webKeyStr[18];

static int WorkCodeDownLoad  ( int panel, int *Status ); 
static int WorkCodeFileDecode( int panel, int *Status );       

int GetInstrumentIDByComport ( int ComID, char sbbh[20]);
int GetLocationByComport     ( int ComID, char jd[20], char wd[20], char gpstime[20] ); 
int CheckBlastingRule				 ( char* xmbh );  
extern int StrTimeToCalenderTime    (  char *strTime,  struct tm *cTime );  
extern int DisplayMessage( int Panel, int TextBoxCtrl, char* Info );       
typedef enum
{
	DT_LOCFILE = 1, 
	DT_OFFLINE = 2, 
	DT_ONLINE  = 3,
}
WC_DOWNLOADTYPE;

typedef enum
{
	INDEX_TYPE = 0, 
	INDEX_UID  = 1,   
}
PANEL_INDEX;

int proColor = 0x00CCFFFF;

/**************************************************************************************************************************/
int Thread_WorkCodeDownLoad( void *funcData )
{
	ThreadRunning = 1; 
	
	int  *p = (int*)(funcData);
	int  panel = *p;
	int  Status=-1; 
	char info[100], Errorinfo[500];  ;
	int  color = VAL_RED; 
	SetCtrlAttribute  ( panel, TABPANEL_3_PROGRESSBAR, ATTR_CTRL_VAL, 10.0);   
	if( NumOfUIDs == ExistedNum )
	{
		Status = 0;		
	}
	else
	{
		if( DT_LOCFILE == DownLoadType )	 WorkCodeFileDecode( panel, &Status ); 
		else															 WorkCodeDownLoad  ( panel, &Status );
	}
	
	if( 0 == Status )
	{
		strcpy( info, "正在对工作码的准爆禁爆规则进行验证...");
		DisplayMessage( panel, TABPANEL_3_TEXTBOX, info );  
		
		Status = CheckBlastingRule( xmbh ); 
		
		if( 0 == Status )	 
		{
			BlastingEnable = 1;    
			strcpy( info, "工作码准爆禁爆规则验证成功"); 
			color = proColor;
		}
		else
		{
			USEROPERATION_GetErrorText( Status, &Errorinfo ); 
			sprintf( info, "工作码准爆禁爆规则验证失败, 错误代码%d, %s", Status, Errorinfo );    
		}
	}
	else
	{
		USEROPERATION_GetErrorText( Status, &Errorinfo ); 
		sprintf( info, "工作码下载错误, 错误代码%d, %s", Status, Errorinfo );
	}
	DisplayMessage( panel, TABPANEL_3_TEXTBOX, info );  
	
	SetCtrlAttribute  ( panel, TABPANEL_3_WLBUTTON,   ATTR_VISIBLE, 0 ); 
	SetCtrlAttribute  ( panel, TABPANEL_3_DONEBUTTON, ATTR_VISIBLE, 1 );  

	SetCtrlAttribute  ( WCDLPanelHandle, WCDL_PANEL_STEP4, ATTR_OFF_COLOR , color ); 
	
	ThreadRunning = 0; 
	return 0;
}
/**************************************************************************************************************************/
int CVICALLBACK WCDownloadPanelCallBack (int panel, int event, void *callbackData, int eventData1, int eventData2)   
{
	switch (event)
	{
		case EVENT_PANEL_RESTORE: 
		{
			// check local denotator info and update
			WCDLPanelHandle = panel;

			if( 0 == DownloadThreadID )
			if(RET_ERROR == SYSTEM_CreateThreadIdentifier ( 1, &DownloadThreadID ))
			{
				MessagePopup      ( "Error", "工作码下载线程开启失败，请重新启动程序"); 	
				CallPanelCallback ( panel, EVENT_CLOSE, 0, 0, 0 );   
	 		}  

			if( 0 == cellnumber )
			{
				cellnumber = rangeHeight*rangeWidth; 
				
				if( MAX_DET_NUM < cellnumber/2 ) MessagePopup( "Error", "雷管UID超过最大级数限制，请重新检查！"); 
				
			  if( !WorkcodeInfo ) { WorkcodeInfo   = (char**) calloc( NumOfWCInfo, sizeof(char*) ); }	 
				
				for( int i=0; i<NumOfWCInfo; i++ )
				{
					WorkcodeInfo[i] = calloc( 100, sizeof(char));	
				}
				
				if( !ExistedUID )     { ExistedUID  = (char**) calloc( MAX_DET_NUM, sizeof(char*) ); }	 
				for( int i=0; i<MAX_DET_NUM; i++ )
				{
					ExistedUID[i] = calloc( 20, sizeof(char));
				}
				
				memset( SerialPort, NULL, 10*sizeof(char));
				
				CallCtrlCallback  ( WCDLPanelHandle, WCDL_PANEL_TAB, EVENT_ACTIVE_TAB_CHANGE, 0, 0, 0 );  
			}	
			
		}
			break;
		case EVENT_LOST_FOCUS:

			break;
			
		case EVENT_DISCARD: case EVENT_CLOSE:
		{
			if( DownloadThreadID ) { SYSTEM_TerminateThread( DownloadThreadID); DownloadThreadID=0;}   
			if( ComPortID )				 { COMPORT_Close         ( ComPortID ); }
			
			if( ExistedUID )
			{ 
				for( int i=0; i<MAX_DET_NUM; i++ )
				{
					if(ExistedUID[i]) { free(ExistedUID[i]); ExistedUID[i] = NULL;} 
				}
				free(ExistedUID  ); ExistedUID  = NULL;
			}
			
			if( WorkcodeInfo )
			{ 
				for( int i=0; i<NumOfWCInfo; i++ )
				{
					if(WorkcodeInfo[i]) { free(WorkcodeInfo[i]); WorkcodeInfo[i] = NULL;} 
				}
				free(WorkcodeInfo  ); WorkcodeInfo  = NULL;
			}
			
			if (fileList)
		  {
			  for( int i=0; i<numOfFiles;i++ )
			  {
			 	 if( fileList[i] )	{ free( fileList[i] ); fileList[i] = NULL; }	 
			  }
			 free( fileList ); fileList = NULL;
		  }
			
			if (fileKeyList)
		  {
			  for( int i=0; i<numOfFiles;i++ )
			  {
			 	 if( fileKeyList[i] )	{ free( fileKeyList[i] ); fileKeyList[i] = NULL; }	 
			  }
			 free( fileKeyList ); fileKeyList = NULL;
		  }
			
		  numOfFiles    = 0;  
			cellnumber = 0; 
			
			//DiscardPanel (panel);
			HidePanel(panel);   
		}
		break;
	}
	return 0;	
}
/**************************************************************************************************************************/   
int EnumComPort( int tabPanelHandle )
{
	//enum available com ports
	HKEY   hkey;     
  LONG   lRes  =  RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", NULL, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_READ, &hkey);     
  if   (lRes   ==   ERROR_SUCCESS)     
  { 
      TCHAR   tchKey[MAX_PATH];     
      TCHAR   tchValue[20]; 
      DWORD   dwIndex = 0; 
      DWORD   dwType  = REG_SZ; 
      int i=0;
			while(lRes == ERROR_SUCCESS) 
      { 
        DWORD dwCount   =   MAX_PATH; 
        DWORD dwVCount   =   20; 
        lRes = RegEnumValue(hkey, dwIndex++,  tchKey, &dwCount, NULL, &dwType, (LPBYTE)tchValue, &dwVCount); 
        if(lRes == ERROR_SUCCESS) 
        if((dwVCount > 0) && (dwCount > 0)) { InsertListItem ( tabPanelHandle, TABPANEL_0_RING, i, tchValue, i); i++; } //can improve 
       } 
  }
	RegCloseKey(hkey);
	return 0;
}
/**************************************************************************************************************************/  
int CVICALLBACK WCDLTabCtrlCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int currIndex, tabPanelHandle; 
	GetActiveTabPage          ( panel, WCDL_PANEL_TAB, &currIndex );  
	GetPanelHandleFromTabPage ( panel, WCDL_PANEL_TAB, currIndex, &tabPanelHandle); 
		
	switch (event)
	{
	////////////////////////////////////////////////////////////////////////////////////////////
		case EVENT_ACTIVE_TAB_CHANGE:
		{
			// check local denotator info and update
			switch( currIndex )
			{
				case INDEX_TYPE: 
				{
					EnumComPort( tabPanelHandle );   
					//CallPanelCallback ( panel, EVENT_PANEL_RESTORE, 0, 0, 0 );
					//SetCtrlAttribute  ( panel, WCDL_PANEL_TAB, ATTR_TABS_VISIBLE, 0 );
				}break;
				
				case 3: 
				{
					char label[2][20];
					
					if( DT_OFFLINE == DownLoadType ) 
					{
						strcpy(label[0],"箱条码");
						strcpy(label[1],"盒条码");
					}
					else//if( DT_ONLINE == DownLoadType ) 
					{
						strcpy(label[0],"经度位置");
						strcpy(label[1],"纬度位置");
					
						
						SetCtrlAttribute  ( tabPanelHandle, TABPANEL_2_LOCATIONBTN, ATTR_VISIBLE, 1 );
						if( jd ) 	 SetCtrlAttribute  ( tabPanelHandle, TABPANEL_2_STRING_6, ATTR_CTRL_VAL, jd   );
						if( wd ) 	 SetCtrlAttribute  ( tabPanelHandle, TABPANEL_2_STRING_5, ATTR_CTRL_VAL, wd   );
					}
					SetCtrlAttribute  ( tabPanelHandle, TABPANEL_2_NUMERIC, ATTR_CTRL_VAL , NumOfUIDs  ); 
					SetCtrlAttribute  ( tabPanelHandle, TABPANEL_2_STRING_6, ATTR_LABEL_TEXT, label[0] ); 
					SetCtrlAttribute  ( tabPanelHandle, TABPANEL_2_STRING_5, ATTR_LABEL_TEXT, label[1] ); 
					
					if( sbbh ) SetCtrlAttribute  ( tabPanelHandle, TABPANEL_2_STRING_3, ATTR_CTRL_VAL, sbbh );    
					
				}break;
				
				case 4:
				{
					SetCtrlAttribute  ( WCDLPanelHandle, WCDL_PANEL_STEP4, ATTR_DIMMED , 0 );  
					SetCtrlAttribute  ( WCDLPanelHandle, WCDL_PANEL_STEP4, ATTR_OFF_COLOR , proColor ); 
					
					if( ExistedNum == NumOfUIDs )  
					{
						SetCtrlAttribute  ( tabPanelHandle, TABPANEL_3_TEXTMSG_2, ATTR_CTRL_VAL, "5）验证工作码" );
						SetCtrlAttribute  ( tabPanelHandle, TABPANEL_3_WLBUTTON,  ATTR_LABEL_TEXT, "验证"  );    
					}
					else
					{
						SetCtrlAttribute  ( tabPanelHandle, TABPANEL_3_WLBUTTON, ATTR_LABEL_TEXT , "开始下载"  );  
						DefaultCtrl       ( tabPanelHandle, TABPANEL_3_TEXTMSG_2 );
					}
					
					
					
				}break;
			}
		}
		break;
			
		////////////////////////////////////////////////////////////////////////////////////////////  	
		case EVENT_LOST_FOCUS:

			break;
		////////////////////////////////////////////////////////////////////////////////////////////  	
		case EVENT_GOT_FOCUS:	 
		{
			switch( currIndex )
			{
				case INDEX_TYPE: 
				{
					CallPanelCallback ( panel, EVENT_PANEL_RESTORE, 0, 0, 0 );
					SetCtrlAttribute  ( panel, WCDL_PANEL_TAB, ATTR_TABS_VISIBLE, 0 );
				}break;
				
				case INDEX_UID: 
				{
					SetTableCellRangeAttribute (tabPanelHandle, TABPANEL_1_TABLE, VAL_TABLE_COLUMN_RANGE (1), ATTR_CELL_DIMMED, 1);
					SetTableCellRangeAttribute (tabPanelHandle, TABPANEL_1_TABLE, VAL_TABLE_COLUMN_RANGE (3), ATTR_CELL_DIMMED, 1);
					SetTableCellRangeAttribute (tabPanelHandle, TABPANEL_1_TABLE, VAL_TABLE_COLUMN_RANGE (5), ATTR_CELL_DIMMED, 1);
					SetTableCellRangeAttribute (tabPanelHandle, TABPANEL_1_TABLE, VAL_TABLE_COLUMN_RANGE (7), ATTR_CELL_DIMMED, 1);
				}break;
			}
			
		}
		break;
	 ////////////////////////////////////////////////////////////////////////////////////////////	
		
	}
	return 0;	
}
  
/**************************************************************************************************************************/ 
int CVICALLBACK TableCtrCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)       
{
		
	switch (event)
	{
		////////////////////////////////////////////////////////////////////////////////////////////
		case EVENT_VAL_CHANGED://EVENT_ACTIVE_CELL_CHANGE:
		{
			//May need to select to the right cell when scaning into different column
			
			int currIndex, tabPanelHandle; 
			GetActiveTabPage  ( WCDLPanelHandle, WCDL_PANEL_TAB, &currIndex );  
		}
		break; 
		////////////////////////////////////////////////////////////////////////////////////////////
		case EVENT_COMMIT: 
		{
			//int tabPanelHandle;  
			//char currentVal[50];   
			//GetTableCellVal     ( panel, TABPANEL_1_TABLE, MakePoint(eventData1,eventData2), currentVal);    
		}
		break;
		////////////////////////////////////////////////////////////////////////////////////////////
	}
	return 0;	 	
}
/**************************************************************************************************************************/
int GetInstrumentIDByComport( int DevID, char sbbh[20] )
{
	int  index;
	int  CommandLen=36;
	char InstID[36];
	char ReadBuffer[36]; 
	
	unsigned char CheckSum_ID=0x00;

	Commmand_InstID[0]    = 0x55; 
	Commmand_InstID[1]    = 0x49;
	Commmand_InstID[2]    = 0x01; 
	Commmand_InstID[35]   = 0xAA; 

	memset(&Commmand_InstID  [3], 0x00, 32);
	/*
	for( int i=2; i<33; i++ )
	{
		CheckSum_ID 			+= Commmand_InstID  [i]; 
		CheckSum_location += Commmand_location[i];      
	}
	*/
	Commmand_InstID[34]   = 0x01; 
	
	if( DevID > 0 )	
	if( RET_NOERR == COMPORT_WriteBuffer   ( DevID, Commmand_InstID, CommandLen ))         	
	if( RET_NOERR == COMPORT_GetReadBuffer ( DevID, InstID )) 
	//if( strstr ( InstID, UIDHeader ))
	if( NULL !=	strncpy( sbbh, &InstID[3], 11))		 
	//if( NULL !=	strncpy( sbbh, "111122223333", 12)) //11	
	{
		return 0;	
	}
	
	return -1;			
}
/**************************************************************************************************************************/  
#define MAX_HEX_STR_LEN 8
int hexToDec(char *shex, int *idec)
{
    size_t i = 0, len = 0;
    int mid = 0;

    len = strlen(shex);
    if (len > MAX_HEX_STR_LEN) {
        return -1;
    }

    idec = 0;
    for (i = 0; i < len; i++) {
        mid = 0;
        if (shex[i] >= '0' && shex[i] <= '9') {
            mid = shex[i] - '0';
        } else if (shex[i] >= 'a' && shex[i] <= 'f') {
            mid = shex[i] - 'a' + 10;
        } else if (shex[i] >= 'A' && shex[i] <= 'F') {
            mid = shex[i] - 'A' + 10;
        } else {
            return -1;
        }
        mid <<= ((len - i - 1) << 2);
        idec += mid;
    }
    return 0;
}

/**************************************************************************************************************************/
int GetLocationByComport( int DevID, char jd[20], char wd[20], char gpstime[20] )
{
	int  index;
	int  CommandLen=36;
	char ReadBuffer[36];
	//char ReadBuffer[36] = {0x55, 0x49, 0x15, 0x46, 0x37, 0x33, 0x68, 0x74, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x4E, 0x00, 0x04, 0x79, 0x55, 0x45, 0x00, 0x10, 0x37, 0xED, 0x20, 0x06, 0x12, 0x14, 0x19, 0x58, 0x00, 0x00, 0x00, 0x00, 0x2C, 0xAA }; 
	//char ReadBuffer[36] = {0x55, 0x49, 0x15, 0x4E, 0x01, 0xB4, 0x42, 0x74, 0x45, 0x06, 0xAB, 0x8E, 0x20, 0x08, 0x06, 0x14, 0x32, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0xAA };
	unsigned char CheckSum_location=0x00;

	Commmand_location[0]  = 0x55;
	Commmand_location[1]  = 0x49;
	Commmand_location[2]  = 0x05;
	Commmand_location[35] = 0xAA;

	memset(&Commmand_location[3], 0x00, 32); 
	/*
	for( int i=2; i<33; i++ )
	{
		CheckSum_location += Commmand_location[i];      
	}
	*/
	Commmand_location[34] = 0x05;

	if( DevID > 0 )	
		
	if( RET_NOERR == COMPORT_WriteBuffer    ( DevID, Commmand_location, CommandLen ))         	
	if( RET_NOERR == COMPORT_GetReadBuffer  ( DevID, ReadBuffer ))
	/*
	if( index = strspn ( ReadBuffer, "jd"))
	if( NULL !=	strncpy( jd, ReadBuffer[index], 10))	
	if( index = strspn ( ReadBuffer, "wd")) 	
	if( NULL !=	strncpy( wd, ReadBuffer[index], 10))
	*/
	{
 		char buffer[4],time[20];int index=0; 
		int i = 0; 
		int value=0;
		
		memcpy ( wd,     &ReadBuffer[3],1); //14	3
		memcpy ( buffer, &ReadBuffer[4],4); //15  4

		int a = (((int)buffer[0])<<24) & 0xFF000000; 
	  int b = (((int)buffer[1])<<16) & 0x00FF0000; //careful about left shift, the high will all be 1. 
		int c = (((int)buffer[2])<<8)  & 0x0000FF00; 
		int d = (int)buffer[3]; 
		value = (a + b + c + d);
		sprintf( &wd[i], "%d.%04d", value/1000000, (value%1000000)/60 ); 
		
		memcpy( jd,      &ReadBuffer[8],1); //19			8
		memcpy( buffer,  &ReadBuffer[9],4); //20			9
		
		a = (((int)buffer[0])<<24) & 0xFF000000; 
	  b = (((int)buffer[1])<<16) & 0x00FF0000;  
		c = (((int)buffer[2])<<8)  & 0x0000FF00; 
		d = (int)buffer[3]; 
		
		value =(a + b + c + d);	
		sprintf( &jd[i], "%d.%04d", value/1000000, (value%1000000)/60 ); 

		index= 0;
		strcpy ( gpstime, "20"); 
		index += sprintf( &time[index],  "%02x",ReadBuffer[13] ); //24
		index += sprintf( &time[index],  "%02x",ReadBuffer[14] ); //25  
		index += sprintf( &time[index],  "%02x",ReadBuffer[15] ); //26  
		index += sprintf( &time[index],  "%02x",ReadBuffer[16] );   
		index += sprintf( &time[index],  "%02x",ReadBuffer[17] );
		index += sprintf( &time[index],  "%02x",ReadBuffer[18] ); //29
		strncat( gpstime, time, 12);
		
		return 0; 
	}
	return -1;
}
/**************************************************************************************************************************/    
int CVICALLBACK ComPortGetCtrCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		////////////////////////////////////////////////////////////////////////////////////////////
		case EVENT_COMMIT:
		{	
			int ringIdx, comID; char ringLabel[10];
			int  CommandLen=36; 
			
			GetCtrlIndex      (panel, TABPANEL_0_RING, &ringIdx);
			GetLabelFromIndex (panel, TABPANEL_0_RING, ringIdx, ringLabel);

			if( comID = strspn ( ringLabel, "COM") ) //strpbrk ( ringLabel, "COM" ))
			{
				if( !(strspn ( SerialPort, "COM")) ) 
				{
					StrToInt ( &ringLabel[comID], &comID );	
					if( ComPortID = COMPORT_Init		  ( "BlastingDev", comID, 115200, CommandLen, CommandLen )) 
					if( RET_NOERR == COMPORT_Open     ( ComPortID ))	
					if( 0 == GetInstrumentIDByComport ( ComPortID, sbbh )) strcpy( SerialPort, ringLabel );   
					else																				 { MessagePopup ( "Error", "获取起爆控制仪信息失败，请重新检查！");	break;}					
				}

				//check download type
				if( 0 == DownLoadType ) MessagePopup      ( "Warning", "请选择工作码下载类型！"); 
			  else
			  {
				  int lastIndex, currIndex, tabPanelHandle;
				  GetActiveTabPage  ( WCDLPanelHandle, WCDL_PANEL_TAB, &lastIndex );   
					currIndex = ( DT_LOCFILE == DownLoadType )? lastIndex+1 : lastIndex+2; 
				  SetCtrlAttribute  ( WCDLPanelHandle, WCDL_PANEL_TAB, ATTR_CTRL_INDEX, ++lastIndex);//lastIndex+DownLoadType	
				
					SetCtrlAttribute  ( WCDLPanelHandle, WCDL_PANEL_STEP2, ATTR_DIMMED , 0 );    
					SetCtrlAttribute  ( WCDLPanelHandle, WCDL_PANEL_STEP2, ATTR_OFF_COLOR , proColor );
				}
			}
			else
			{
				MessagePopup ( "Warning", "未找到有效的串口号，请重新检查！"); 
			}
		}break; 
	}
 
	return 0;
}

/**************************************************************************************************************************/   

int CVICALLBACK UIDGetCtrCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		////////////////////////////////////////////////////////////////////////////////////////////
		case EVENT_COMMIT:
		{
			int error = -1;
			Rect  cellRange=MakeRect ( 1, 1, rangeHeight, rangeWidth);    
			///////////////////////////////////////////////////////////////////////
			if( TABPANEL_1_UIDBUTTON == control )
			{
					Rect  cellRange=MakeRect ( 1, 1, rangeHeight, rangeWidth);
					Point cell     =MakePoint( 1,1 ); 
					int   uidCheck=1;
					char **valueArray     = (char**) calloc( cellnumber,   sizeof(char*) ); 
					if( valueArray ) 
					{
						NumOfUIDs = 0;
						for( int i=0; i<cellnumber/2; i++ )
						{
							memset( UIDArray[i], NULL, 20*sizeof(char));  
						}
					}
					GetTableCellRangeVals ( panel, TABPANEL_1_TABLE, cellRange, valueArray, VAL_COLUMN_MAJOR ); 
					//UID validation
					ExistedNum = 0; error = 0; 
					for( int i=0; i<rangeWidth/2; i++ )
					{
						for( int j=0;j<rangeHeight;j++ )
						{
						  if( 0 != strcmp ( "",   valueArray[(i*2+1)*10+j]  )) 
							{
								//int headcmp = strncmp( UIDHeader, valueArray[(i*2+1)*10+j], 2 );
								//int lens    = strlen(valueArray[(i*2+1)*10+j] ); 
								//if( 0 == headcmp && UIDLen == lens)
								{
									//memcpy( UIDArray[j+i*rangeHeight], valueArray[(i*2+1)*10+j], 20*sizeof(char)); 
									//NumOfUIDs++; 
									memcpy( UIDArray[NumOfUIDs++], valueArray[(i*2+1)*10+j], 20*sizeof(char));  
									char WorkCodeNO[10];
									char **Request_DueTime=NULL, **Request_UploadTime=NULL, **Request_Info=NULL;;
									if( SQLITE_SearchRequest( DataBaseName, InfoTable,   "UID", valueArray[(i*2+1)*10+j], "yxq",        &Request_DueTime )    > 0 )
									if( SQLITE_SearchRequest( DataBaseName, ResultTable, "UID", valueArray[(i*2+1)*10+j], "UploadTime",  &Request_UploadTime ) > 0 ) 	
									{
										div_t coord = div (NumOfUIDs-1, 10);
										char *defdata = "null";
										if ( 0 == strncmp( *Request_UploadTime, defdata, sizeof(defdata) ))//haven't been used
										{
											memcpy( ExistedUID[ExistedNum++], valueArray[(i*2+1)*10+j], 20*sizeof(char));    
											SetTableCellAttribute ( panel, TABPANEL_1_TABLE, MakePoint ((coord.quot+1)*2, coord.rem+1), ATTR_TEXT_BGCOLOR, VAL_YELLOW);				//existed and available,     no need to download
								   	}
										else
										{
											//existed and not available, must remove
											error=-1;
											SetTableCellAttribute ( panel, TABPANEL_1_TABLE, MakePoint ((coord.quot+1)*2, coord.rem+1), ATTR_TEXT_BGCOLOR, VAL_RED ); 
											MessagePopup ( "Error", "非法雷管UID（已使用），请重新输入！");
											SetCtrlAttribute  ( panel, TABPANEL_1_UIDBUTTON,  ATTR_DIMMED,  1 );  
											break;
										} 
									}
									//not existed,need to download

									//will affect how many quantity of uid, only when whole download process passed, uidarray available 
									if( Request_DueTime )    { free(Request_DueTime);    Request_DueTime    = NULL; }   
									if( Request_UploadTime ) { free(Request_UploadTime); Request_UploadTime = NULL; } 
									
								}
								//else
								//{
								//	SetActiveTableCell (panel, TABPANEL_1_TABLE, MakePoint (i*2+2, j+1));
								//	uidCheck = 0; break;
								//}
							}
						}
						if(error) break;
					}
					
					if((0==error) && NumOfUIDs>0 && uidCheck >0 )
					{
						/* 
						ExistedNum = 0; error = 0;
						for( int i=0; i<NumOfUIDs; i++ )
						{
							char WorkCodeNO[10];
							char **Request_DueTime=NULL, **Request_UploadTime=NULL, **Request_Info=NULL;;
							if( SQLITE_SearchRequest( DataBaseName, InfoTable,   "UID", UIDArray[i], "yxq",        &Request_DueTime )    > 0 )
							if( SQLITE_SearchRequest( DataBaseName, ResultTable, "UID", UIDArray[i], "UploadTime", &Request_UploadTime ) > 0 ) 	
							{
								div_t coord = div (i, 10);
								char *defdata = "null";
								if ( 0 == strncmp( *Request_UploadTime, defdata, sizeof(defdata) ))//haven't been used
								{
									memcpy( ExistedUID[ExistedNum++], UIDArray[i], 20*sizeof(char));    
									SetTableCellAttribute ( panel, TABPANEL_1_TABLE, MakePoint ((coord.quot+1)*2, coord.rem+1), ATTR_TEXT_BGCOLOR, VAL_YELLOW);				//existed and available,     no need to download
						   	}
								else
								{
									//existed and not available, must remove
									error=-1;
									SetTableCellAttribute ( panel, TABPANEL_1_TABLE, MakePoint ((coord.quot+1)*2, coord.rem+1), ATTR_TEXT_BGCOLOR, VAL_RED ); 
									MessagePopup ( "Error", "非法雷管UID（已使用），请重新输入！");
									SetCtrlAttribute  ( panel, TABPANEL_1_UIDBUTTON,  ATTR_DIMMED,  1 );  
									break;
								} 
							}
							//not existed,need to download

							//will affect how many quantity of uid, only when whole download process passed, uidarray available 
							if( Request_DueTime )    { free(Request_DueTime);    Request_DueTime    = NULL; }   
							if( Request_UploadTime ) { free(Request_UploadTime); Request_UploadTime = NULL; } 
							
						}
						*/ 
						char info[100];
						sprintf( info, "输入雷管UID总共有 %d 个，已有工作码 %d 个在本地数据库中", NumOfUIDs, ExistedNum);
						SetCtrlAttribute  ( panel, TABPANEL_1_TEXTMSG, ATTR_CTRL_VAL, info );
						
						if( 0 == error )
						{
							SetCtrlAttribute  ( panel, TABPANEL_1_NEXTBUTTON, ATTR_VISIBLE, 1 );
							SetCtrlAttribute  ( panel, TABPANEL_1_UIDBUTTON,  ATTR_VISIBLE, 0 );
						}
					}
					else
					{
						//MessagePopup ( "Warning", "无效的雷管UID！先点击“清除”按钮，再双击非法UID所在单元格，再DEL键删除！");  
						char info[100]; 
						sprintf( info, "无效的雷管UID！先点击“清除”按钮，再双击非法UID所在单元格，再DEL键删除！");
						SetCtrlAttribute  ( panel, TABPANEL_1_TEXTMSG, ATTR_CTRL_VAL, info );
					}
					
					if(valueArray) { free(valueArray); valueArray = NULL; }
			}
			///////////////////////////////////////////////////////////////////////
			if( TABPANEL_1_CLNBUTTON == control )
			{
				DefaultCtrl       ( panel, TABPANEL_1_TABLE );
				SetTableCellRangeAttribute (panel, TABPANEL_1_TABLE, VAL_TABLE_ENTIRE_RANGE, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT); 
				SetCtrlAttribute  ( panel, TABPANEL_1_NEXTBUTTON, ATTR_VISIBLE, 0 );
				SetCtrlAttribute  ( panel, TABPANEL_1_UIDBUTTON,  ATTR_VISIBLE, 1 );
				SetCtrlAttribute  ( panel, TABPANEL_1_UIDBUTTON,  ATTR_DIMMED,  0 ); 
			}
			/////////////////////////////////////////////////////////////////////// 
			if( TABPANEL_1_NEXTBUTTON == control )
			{
				int lastIndex, currIndex;     
				GetActiveTabPage  ( WCDLPanelHandle, WCDL_PANEL_TAB, &lastIndex ); 
				
				if( NumOfUIDs == ExistedNum ) 			MessagePopup  ( "Warning", "UID对应的工作码已经存在且有效，无需下载" );   
				else if( 0 < ExistedNum ) 				  MessagePopup  ( "Warning", "继续下载将覆盖部分已存在的工作码" );            
				
				currIndex = ( DT_LOCFILE == DownLoadType )? lastIndex+1 : lastIndex+2;  
				
				if( ExistedNum == NumOfUIDs )  currIndex = lastIndex+2; 	// no need to re-download
										 
				SetCtrlAttribute  ( WCDLPanelHandle, WCDL_PANEL_TAB,   ATTR_CTRL_INDEX, currIndex );   
				CallCtrlCallback  ( WCDLPanelHandle, WCDL_PANEL_TAB,   EVENT_ACTIVE_TAB_CHANGE, 0, 0, 0 ); 
				SetCtrlAttribute  ( WCDLPanelHandle, WCDL_PANEL_STEP3, ATTR_DIMMED , 0 );    
				SetCtrlAttribute  ( WCDLPanelHandle, WCDL_PANEL_STEP3, ATTR_OFF_COLOR , proColor ); 	
			}
		}
		break;
	////////////////////////////////////////////////////////////////////////////////////////////
	}
	return 0;
}
/**************************************************************************************************************************/    
int CVICALLBACK SelectionCtrCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)       
{
	int radioGroup[3] = { TABPANEL_0_LOCFILEBTN, TABPANEL_0_OFFLINEBTN, TABPANEL_0_ONLINEBTN };	    
	
	switch (event)
	{
		//////////////////////////////////////////////////////////////////////////////////////////// 
		case EVENT_COMMIT: 
		{
			//////////////////////////////////////////////////////////////////////////////////////////// 
			DownLoadType = 0;
			for( int i=0;i<3;i++ )
		  {
			  if( control != radioGroup[i] )
			  {
				  SetCtrlVal (panel, radioGroup[i], 0);
			  }
			  else
			  {
				  DownLoadType = i+1; 
			  }
		  }
		}break; 
		//////////////////////////////////////////////////////////////////////////////////////////// 
		
	}
	
	return 0; 
}

/**************************************************************************************************************************/
int CVICALLBACK LocationGetCtrCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2) 
{
	switch (event)
		{
			//////////////////////////////////////////////////////////////////////////////////////////// 
			case EVENT_COMMIT:
			{
				if( 0 == GetInstrumentIDByComport( ComPortID, sbbh )) 
				if( 0 == GetLocationByComport    ( ComPortID, jd, wd, gpstime ))
				{
					SetCtrlAttribute  ( panel, TABPANEL_2_STRING_3, ATTR_CTRL_VAL, sbbh );
					SetCtrlAttribute  ( panel, TABPANEL_2_STRING_6, ATTR_CTRL_VAL, jd   );	
		  		SetCtrlAttribute  ( panel, TABPANEL_2_STRING_5, ATTR_CTRL_VAL, wd   );
					SetCtrlAttribute  ( panel, TABPANEL_2_INFOBUTTON,  ATTR_DIMMED, 0   );
				}
				else
				{
					MessagePopup ( "Error", "串口读取经纬度信息失败，请重新检查！"); 
				}
			}break; 
		
		}
		return 0;
}
/**************************************************************************************************************************/
int CheckProjectInfo( int panel )
{ 
	int error = 0;  
	if( 0 < ExistedNum )
	{
		for( int i=0; i<ExistedNum; i++ )
		{
			char **RequestData =NULL; error = -1;   
			if( SQLITE_SearchRequest( DataBaseName, ResultTable,   "UID", ExistedUID[i], "*", &RequestData ) > 0 )
			{
				int item;
				if( RequestData )
				{
					const char del[2] = ",";
			    char *token; int index=0;
					token = strtok(*RequestData, del);
					error = 0; 
					
			    while( token != NULL && (0==error) ) 
					{
						error = -1;
						switch( index++)
						{
							case 1:  if( 0 == strcmp( token, WorkcodeInfo[0] )) error=0; break;
							case 7:  if( 0 == strcmp( token, WorkcodeInfo[6] )) error=0; break; 
							case 8:  if( 0 == strcmp( token, WorkcodeInfo[3] )) error=0; break;   
							case 9:  if( 0 == strcmp( token, WorkcodeInfo[4] )) error=0; break;    
							case 10: if( 0 == strcmp( token, WorkcodeInfo[5] )) error=0; break; 
							
							default: error=0; 
						}
			      token = strtok( NULL, del);
					}
				}

				if(error)
				{
					SetCtrlAttribute  ( panel, TABPANEL_2_INFOBUTTON,  ATTR_DIMMED, 1); break;
				}
			}
		  if( RequestData ) { free(RequestData); RequestData=NULL; }
		}
	}	
	return error;
}
/**************************************************************************************************************************/  
int CVICALLBACK InfoGetCtrCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		//////////////////////////////////////////////////////////////////////////////////////////// 
		case EVENT_COMMIT:
		{
			int check=1; 
			
			//memset( WorkcodeInfo[i], NULL, 20 );
			//workcodeInfo = { sbbm, jd, wd, UIDArray, xmbh, htid, dwdm }; 
	
			GetCtrlAttribute  ( panel, TABPANEL_2_STRING_3, ATTR_CTRL_VAL, WorkcodeInfo[0]);	  
			GetCtrlAttribute  ( panel, TABPANEL_2_STRING_6, ATTR_CTRL_VAL, WorkcodeInfo[1]);	
		  GetCtrlAttribute  ( panel, TABPANEL_2_STRING_5, ATTR_CTRL_VAL, WorkcodeInfo[2]);	 
			GetCtrlAttribute  ( panel, TABPANEL_2_STRING_2, ATTR_CTRL_VAL, WorkcodeInfo[3]); //xmbh
			GetCtrlAttribute  ( panel, TABPANEL_2_STRING,   ATTR_CTRL_VAL, WorkcodeInfo[4]); //htid
			GetCtrlAttribute  ( panel, TABPANEL_2_STRING_4, ATTR_CTRL_VAL, WorkcodeInfo[5]); //dwdm    
			GetCtrlAttribute  ( panel, TABPANEL_2_STRING_7, ATTR_CTRL_VAL, WorkcodeInfo[6]); 

			strcpy( xmbh, WorkcodeInfo[4] );
			strcpy( htid, WorkcodeInfo[5] );  
			strcpy( sbbh, WorkcodeInfo[0] ); 
			strcpy( jd,   WorkcodeInfo[1] ); 
			strcpy( wd,   WorkcodeInfo[2] ); 
			
			if( 0 == CheckProjectInfo( panel ))
			{
			 //check info validation and build json format
				for( int i=0; i<NumOfWCInfo; i++ )
				{
					if( NULL == WorkcodeInfo[i] )
					{
						check=0; break;
					}
				}

				if( 0 == check || NumOfUIDs <= 0 )
				{
					MessagePopup ( "Error", "申请工作码信息不完整，请重新检查！");
				}
				else
				{
					int lastIndex, currIndex, tabPanelHandle; 
					GetActiveTabPage  ( WCDLPanelHandle, WCDL_PANEL_TAB, &lastIndex );   
			  	SetCtrlAttribute  ( WCDLPanelHandle, WCDL_PANEL_TAB, ATTR_CTRL_INDEX, ++lastIndex );
					CallCtrlCallback  ( WCDLPanelHandle, WCDL_PANEL_TAB, EVENT_ACTIVE_TAB_CHANGE, 0, 0, 0  );
				}
			}
			else
			{
				MessagePopup ( "Error", "已下载雷管的工作码项目信息不一致，请重新检查！");  
			}
		}
		break;
 
	}
	return 0;
}
/**************************************************************************************************************************/ 
static int getDetonatorInfo( int panel, char *SourceInfo, int *Status )
{
	INFO_GZM  *DetonatorInfo=NULL; 
	int NumOfInfo;
	int error=UP_INFOPARSE;
	*Status = UP_INFOPARSE;
	int uidcheck=0;
		
	if( RET_NOERR == WORKINGCODESYS_DetonatorInfoParse ( SourceInfo, &DetonatorInfo, &NumOfInfo, &error )) 
	if( 0 == error )//if( (error=-100) && NumOfUIDs <= NumOfInfo )
	{
	 *Status = -1; 
		
		for( int i=0; i<NumOfUIDs; i++ )
		{
			if( uidcheck != i ) break;
			for( int j=0;j<NumOfInfo;j++) 
			{
				if( 0 == strcmp( UIDArray[i], DetonatorInfo[j].uid ))
				{
					uidcheck++; break;
				}
			}
		}	
		if( error=UP_UIDNUM )
		if( uidcheck == NumOfUIDs )
		{
			error =UP_DATABASE;
			char **infoData   = (char**) calloc( NumOfInfo, sizeof(char*) );
			char **resultData = (char**) calloc( NumOfInfo, sizeof(char*) ); 
			int    index=0;  
			/*
			char   time[50];
			int    month, day, year;
			GetSystemDate        ( &month, &day, &year       ); 
			index += sprintf ( &time[index], "%04u-%02u-%02u", year, month, day);  
			index += sprintf ( &time[index], " %s", TimeStr() );   
			*/
			time_t time_T;
			time ( &time_T );
			struct tm *tmTime = localtime(&time_T);
			char* format = "%Y-%m-%d %H:%M:%S";
    	char strTime[100];
    	strftime(strTime, sizeof(strTime), format, tmTime);
			
			for( int i=0;i<NumOfInfo;i++)
			{
				infoData[i]   = calloc( 1000, sizeof(char) );
				resultData[i] = calloc( 1000, sizeof(char) );  
				
				int iindex=0;
				int rindex=0;
				
				iindex += sprintf( &infoData[i][iindex], "%s,", strTime ); //yyyy-mm-dd hh:mm:ss
				iindex += sprintf( &infoData[i][iindex], "%s,", DetonatorInfo[i].sqrq  );    
				iindex += sprintf( &infoData[i][iindex], "%s,", DetonatorInfo[i].yxq   ); 
				iindex += sprintf( &infoData[i][iindex], "%s,", sbbh );    
				iindex += sprintf( &infoData[i][iindex], "%s,", DetonatorInfo[i].gzm   );  
				//iindex += sprintf( &infoData[i][iindex], "%s,", DetonatorInfo[i].fbh   );  
				iindex += sprintf( &infoData[i][iindex], "%s,", DetonatorInfo[i].gzmcw );  
				iindex += sprintf( &infoData[i][iindex], "%s",  DetonatorInfo[i].uid   ); 
				
				rindex += sprintf( &resultData[i][rindex], "%s,", sbbh );	 //sbbh
				rindex += sprintf( &resultData[i][rindex], "%s,", "null" );  //jd
				rindex += sprintf( &resultData[i][rindex], "%s,", "null" );  //wd
				rindex += sprintf( &resultData[i][rindex], "%s,", "null" );	 //bpsj
				rindex += sprintf( &resultData[i][rindex], "%s,", "null" );	 //UploadTime
				rindex += sprintf( &resultData[i][rindex], "%s,", strTime );	 //DownloadTime  
				rindex += sprintf( &resultData[i][rindex], "%s,", WorkcodeInfo[6] ); //BPRYSFZ
				rindex += sprintf( &resultData[i][rindex], "%s,", WorkcodeInfo[3] ); //XBBH   
				rindex += sprintf( &resultData[i][rindex], "%s,", WorkcodeInfo[4] ); //HTID   
				rindex += sprintf( &resultData[i][rindex], "%s,", WorkcodeInfo[5] ); //DWDM
				rindex += sprintf( &resultData[i][rindex], "%s",  DetonatorInfo[i].uid   );
			}

			error=UP_DATABASE; //*Status =UP_DATABASE;
			if( RET_NOERR == SQLITE_InsertIntoDB ( infoTableID,   infoData,   NumOfInfo, 5 ))  
			if( RET_NOERR == SQLITE_InsertIntoDB ( resultTableID, resultData, NumOfInfo, 5 ))  { error=0; }  // *Status = 0;
			
			if( infoData )
			{ 
				for( int i=0; i<NumOfInfo; i++ )
				{
					if(infoData[i]) { free(infoData[i]); infoData[i] = NULL;} 
				}
				free(infoData  ); infoData  = NULL;
			}
			
			if( resultData )
			{ 
				for( int i=0; i<NumOfInfo; i++ )
				{
					if(resultData[i]) { free(resultData[i]); resultData[i] = NULL;} 
				}
				free(resultData ); resultData  = NULL;
			}	
		}
		/*
		else
		{
			char info[200];
			sprintf( info, "-->输入的第%d个雷管UID %s 工作码不存在，请重新检查！", uidcheck+1, UIDArray[uidcheck]);
			DisplayMessage( panel, TABPANEL_3_TEXTBOX, info );  
		}
		*/
	}

	if( error>100 )
	{
		char info[200]; 
		switch( error )
		{
			case UP_UIDNUM:         sprintf( info, "-->输入的第%d个雷管UID %s工作码不存在，请重新检查！", uidcheck+1,  UIDArray[uidcheck]);   break;   
			case UP_DET_USE:        sprintf( info, "-->输入的第%d个雷管UID %s已被使用，请重新检查！",     NumOfInfo+1, UIDArray[NumOfInfo]);  break; 
			case UP_DET_BLACKLIST:  sprintf( info, "-->输入的第%d个雷管UID %s已在黑名单中，请重新检查！", NumOfInfo+1, UIDArray[NumOfInfo]);  break;         
			case UP_DET_NOTEXIST: 	sprintf( info, "-->输入的第%d个雷管UID %s不存在，请重新检查！",       NumOfInfo+1, UIDArray[NumOfInfo]);  break;         
		}
		DisplayMessage( panel, TABPANEL_3_TEXTBOX, info ); 
	}

	if( DetonatorInfo )  { free(DetonatorInfo); DetonatorInfo = NULL; } 
	*Status = error;
	return error;
	
}
/**************************************************************************************************************************/ 
static int getInhibitionAreaInfo( int panel, char *SourceInfo, int *Status )
{
	int NumOfZBInfo,NumOfJBInfo;
	INFO_JBQY *InArea=NULL;  
	INFO_ZBQY *PerArea=NULL;  
	//*Status = UP_INFOPARSE;
	int error = UP_INFOPARSE;
	if( RET_NOERR == WORKINGCODESYS_InhibitionAreaInfoParse ( SourceInfo, &InArea,  &NumOfJBInfo,  &error )) 
	if( RET_NOERR == WORKINGCODESYS_PermitionAreaInfoParse  ( SourceInfo, &PerArea, &NumOfZBInfo,  &error )) 
	{
		//*Status = -1;
		int NumOfInfo = (NumOfZBInfo>NumOfJBInfo)? NumOfZBInfo:NumOfJBInfo;
		char **DataItemList = (char**) calloc( NumOfInfo, sizeof(char*) );
		int i;
		for( i=0;i<NumOfInfo;i++)
		{
			DataItemList[i] = calloc( 2000, sizeof(char) );
			int index=0; 
			if( i < NumOfJBInfo )
			{
				index += sprintf( &DataItemList[i][index], "%s,", InArea[i].jd );    
				index += sprintf( &DataItemList[i][index], "%s,", InArea[i].wd ); 
				index += sprintf( &DataItemList[i][index], "%s,", InArea[i].bj );  
				index += sprintf( &DataItemList[i][index], "%s,", InArea[i].sj); 
			}
			else
			{
				index += sprintf( &DataItemList[i][index], "%s,", "null" );    
				index += sprintf( &DataItemList[i][index], "%s,", "null" ); 
				index += sprintf( &DataItemList[i][index], "%s,", "null" );  
				index += sprintf( &DataItemList[i][index], "%s,", "null" ); 	
			}
			if( i < NumOfZBInfo ) 
			{
				index += sprintf( &DataItemList[i][index], "%s,", PerArea[i].mc );    
				index += sprintf( &DataItemList[i][index], "%s,", PerArea[i].jd ); 
				index += sprintf( &DataItemList[i][index], "%s,", PerArea[i].wd );  
				index += sprintf( &DataItemList[i][index], "%s,", PerArea[i].bj );
				
				if( strcmp( PerArea[i].qssj, "") )	index += sprintf( &DataItemList[i][index], "%s,", PerArea[i].qssj); 
				else											 				  index += sprintf( &DataItemList[i][index], "%s,", "null" );   	
				if( strcmp( PerArea[i].jzsj, "") )	index += sprintf( &DataItemList[i][index], "%s,", PerArea[i].jzsj);
				else										  					index += sprintf( &DataItemList[i][index], "%s,", "null" );   
				
			}
			else
			{
				index += sprintf( &DataItemList[i][index], "%s,", "null" );    
				index += sprintf( &DataItemList[i][index], "%s,", "null" ); 
				index += sprintf( &DataItemList[i][index], "%s,", "null" );  
				index += sprintf( &DataItemList[i][index], "%s",  "null" ); 
				index += sprintf( &DataItemList[i][index], "%s,", "null" ); 
				index += sprintf( &DataItemList[i][index], "%s,", "null" ); 
			}
			index += sprintf( &DataItemList[i][index], "%s,", WorkcodeInfo[3] ); //xmbh    
			index += sprintf( &DataItemList[i][index], "%s",  WorkcodeInfo[4] ); //htid
				
		}
		//*Status = UP_DATABASE; 
		error = UP_DATABASE;
		if( RET_NOERR == SQLITE_InsertIntoDB ( ruleTableID, DataItemList, NumOfInfo, 5 ))  
		{ error=0;}	//*Status =0; 
	}
	*Status = error; 
	return error;
}
/**************************************************************************************************************************/      
static int WorkCodeFileDecode( int panel, int *Status )
{
  //read from local file; 
	int error=0;
	if( numOfFiles > 0 )
	{
		DisplayMessage( panel, TABPANEL_3_TEXTBOX, "正在对离线文件进行工作码解码..." );
		
		for( int i =0; i<numOfFiles; i++ )
		{
			if( fileList[i] && 0 == error )
			{
				error = -1;
				FILE *stream; 
				char buffer[5000];
				char *TargetStr;

				char deskey[24];
				 
				strncpy ( deskey, webKeyStr,     18); 
				strncat ( deskey, fileKeyList[i], 6);
				
				if( stream = fopen (fileList[i], "r" ))
				if( 0< fread ( buffer, sizeof(char), 5000, stream))
				if(  0 == DisplayMessage( panel, TABPANEL_3_TEXTBOX, "正在对离线文件进行工作码解码..." ))               	
				if( RET_NOERR == WORKINGCODESYS_Decryption( buffer, strlen(buffer), deskey, &TargetStr  ))

				if(  0 == DisplayMessage( panel, TABPANEL_3_TEXTBOX, "正在读取雷管UID..."  )) 	
				if(  0 == getDetonatorInfo	 ( panel, TargetStr, &error )) 
				if(  0 == DisplayMessage( panel, TABPANEL_3_TEXTBOX,"雷管UID已经保存在数据库中..."   )) 	
				if(  0 == DisplayMessage( panel, TABPANEL_3_TEXTBOX, "正在读取准爆禁爆规则..." )) 	
				if(  0 == getInhibitionAreaInfo( panel, TargetStr, &error))
				if(  0 == DisplayMessage( panel, TABPANEL_3_TEXTBOX, "准爆禁爆规则已经保存在数据库中..."  )) 	
				if(  0 == DisplayMessage( panel, TABPANEL_3_TEXTBOX, "工作码下载成功."  )) 	
				{
					SetCtrlAttribute ( panel, TABPANEL_3_PROGRESSBAR, ATTR_CTRL_VAL, (double)(100*(i+1)/numOfFiles));
					error = 0;
				}
				
				//release
				if( TargetStr )      { free(TargetStr);     TargetStr = NULL; } 

				memset( buffer, NULL, 5000);  
				memset( deskey, NULL, 25  );  
				fclose (stream);   
			}
	
		}
	}
	
	//if(error) User_ErrorListing ( MSG_ERROR, error, "" );  
		
	return 0; 
}
/**************************************************************************************************************************/   
static int WorkCodeDownLoad( int panel, int *Status )
{
	char info[100];
	if( DT_OFFLINE == DownLoadType )	 strcpy( info, "正在离线下载工作码..." );  
	else															 strcpy( info, "正在在线下载工作码..." );   
	DisplayMessage( panel, TABPANEL_3_TEXTBOX, info );
	
	char *WorkcodeUID;
	int error=-1;
	if( NumOfUIDs>0 ) WorkcodeUID = calloc( 20*NumOfUIDs, sizeof(char) ); 
	
	int index=0; 
	for( int i=0; i<NumOfUIDs; i++ )
	{
		index += sprintf( &WorkcodeUID[index], "%s,", UIDArray[i] );
	}
	
	SetCtrlAttribute  ( panel, TABPANEL_3_PROGRESSBAR, ATTR_CTRL_VAL, 20.0); 
	
	if( WorkcodeUID )
	{
		cJSON *wcObject = cJSON_CreateObject();
		if( DT_ONLINE == DownLoadType  || DT_LOCFILE == DownLoadType)
		{
    	cJSON_AddStringToObject(wcObject, "sbbh", WorkcodeInfo[0]);
    	cJSON_AddStringToObject(wcObject, "jd",   WorkcodeInfo[1]);
			cJSON_AddStringToObject(wcObject, "wd",   WorkcodeInfo[2]);      
			cJSON_AddStringToObject(wcObject, "uid",  WorkcodeUID    );      
			cJSON_AddStringToObject(wcObject, "xmbh", WorkcodeInfo[3]);      
			cJSON_AddStringToObject(wcObject, "htid", WorkcodeInfo[4]);      
			cJSON_AddStringToObject(wcObject, "dwdm", WorkcodeInfo[5]);  
		}
		if( DT_OFFLINE == DownLoadType )
		{
    	cJSON_AddStringToObject(wcObject, "xtm",  WorkcodeInfo[1]);
			cJSON_AddStringToObject(wcObject, "htm",  WorkcodeInfo[2]);      
			cJSON_AddStringToObject(wcObject, "uid",  WorkcodeUID    );      
			cJSON_AddStringToObject(wcObject, "xmbh", WorkcodeInfo[3]);      
			cJSON_AddStringToObject(wcObject, "htid", WorkcodeInfo[4]);
			cJSON_AddStringToObject(wcObject, "sbbh", WorkcodeInfo[0]);
			cJSON_AddStringToObject(wcObject, "dwdm", WorkcodeInfo[5]);  
		}

		memset( WorkcodeUID, NULL, 20*NumOfUIDs*sizeof(char));    
		char *paramStr =  cJSON_PrintUnformatted( wcObject ); 
	
		char *EncryptStr;  
		char *repsonse; 
		char *TargetStr=NULL;
		
		int  length; 
		int  color=VAL_RED;
	  char errinfo[50];

		SetCtrlAttribute  ( panel, TABPANEL_3_PROGRESSBAR, ATTR_CTRL_VAL, 40.0); 

		if(  0 == DisplayMessage( panel, TABPANEL_3_TEXTBOX, "正在对申请信息进行加密..."  )) 	 
		if( (error=UP_ENCRYT) && RET_NOERR == WORKINGCODESYS_Encryption( paramStr, KeyStr, &EncryptStr ))  //(*Status = UP_ENCRYT) 
		if(  0 == DisplayMessage( panel, TABPANEL_3_TEXTBOX,  "正在通过网络下载工作码..." )) 	 
		if( (error=UP_NETWORK) && RET_NOERR == WORKINGCODESYS_HTTPPost  ( DownLoadUrl,EncryptStr, &repsonse, &length ))
		{
			if(  0 == DisplayMessage( panel, TABPANEL_3_TEXTBOX, "正在解密工作码..." )) 	 
			if( (error=UP_DECRYT) && RET_NOERR == WORKINGCODESYS_Decryption( repsonse, length, KeyStr, &TargetStr  ))
			if(  0 == DisplayMessage( panel, TABPANEL_3_TEXTBOX, "正在读取雷管UID..."  )) 	 
			if(  0 ==  getDetonatorInfo	  ( panel, TargetStr,   &error ))
			if(  0 == DisplayMessage( panel, TABPANEL_3_TEXTBOX, "雷管UID已经保存在数据库中..." )) 	 
			if(  0 == DisplayMessage( panel, TABPANEL_3_TEXTBOX, "正在读取准爆禁爆规则..." )) 	
			if(  0 ==  getInhibitionAreaInfo( panel, TargetStr, &error ))
			if(  0 == DisplayMessage( panel, TABPANEL_3_TEXTBOX, "禁爆准爆规则已经保存在数据库中..."   )) 	 
			if(  0 == DisplayMessage( panel, TABPANEL_3_TEXTBOX, "工作码下载成功."  )) 	 
			{
				SetCtrlAttribute  ( panel, TABPANEL_3_PROGRESSBAR, ATTR_CTRL_VAL, 100.0); 
				error=0;
			}
			if( NULL != TargetStr ) { free(TargetStr); TargetStr = NULL; }   
		}

		SetCtrlAttribute  ( panel, TABPANEL_3_PROGRESSBAR, ATTR_FILL_COLOR, color);    
		
		if( paramStr   ) { free(paramStr);  paramStr=NULL;    }  
		if( EncryptStr ) { free(EncryptStr); EncryptStr= NULL; } 
		if( repsonse   ) { free(repsonse);   repsonse  = NULL; }  
	} 
	if( WorkcodeUID ) { free(WorkcodeUID); WorkcodeUID = NULL; } 	
	//if(error) User_ErrorListing ( MSG_ERROR, error, "" );
	*Status = error;
	return error;
}
/**************************************************************************************************************************/   
double CheckTimeLimitation( char* strTime )
{
 	struct tm str_Time;
	double delta = 0; 	
	if( strTime  ) 
	{	   
		int error    = -1;  
		int daycount = 3;

	  time_t time_T;
		time ( &time_T ); //may will use the time from GPS or controller
		StrTimeToCalenderTime ( strTime,   &str_Time );
		time_t time_count   = mktime( &str_Time   ); 
		
		return delta = difftime(time_T, time_count); 
	}
	
	return 0;
 
	/*
	struct tm jbsjTime, zbqssjTime, zbjssjTime;  	
	if( jbsj   ) StrTimeToCalenderTime ( jbsj,   &jbsjTime   );   
	if( zbqssj ) StrTimeToCalenderTime ( zbqssj, &zbqssjTime ); 
	if( zbjssj ) StrTimeToCalenderTime ( zbjssj, &zbjssjTime );
	
	int error    = -1;  
	int daycount = 3;
	
  time_t time_T;
	time ( &time_T ); //may will use the time from GPS or controller
	
	time_t time_jb   = mktime( &jbsjTime   ); 
	time_t time_zbqs = mktime( &zbqssjTime );   
	time_t time_zbjs = mktime( &zbjssjTime );   
	
  if( zbqssj )
	if( zbjssj )	
	if( difftime(time_T, time_zbqs) > 0 && difftime(time_T, time_zbjs) < 0 )	error = 0;
	
	if( 0 == error )
	if( jbsj       )
	if( difftime(time_T, time_jb) > 0)  error = 0;   

	return error; 
	*/ 
}
/**************************************************************************************************************************/ 
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
/**************************************************************************************************************************/ 
int CheckBlastingRule( char* xmbh )
{
	int error=-1; 
	double currjd, currwd;
	
	//if( RET_NOERR == COMPORT_Open     ( ComPortID ))	  
	//if( 0 == GetInstrumentIDByComport ( ComPortID, sbbh )) && 0 == GetLocationByComport     ( ComPortID, jd, wd, gpstime ))       
	if(error=UP_DEV_INVALID) 
	//if( RET_NOERR == COMPORT_Close    ( ComPortID ))	     	
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
			//double t_jb   = CheckTimeLimitation( jbsj   );   
			//double t_zbqs = CheckTimeLimitation( zbqssj ); 
			//double t_zbjs = CheckTimeLimitation( zbjssj ); 
			error = 0;
			if( 0 != strcmp( "null", jbsj ) && 0 ==error )
			if( (error=UP_JBSJ) && CheckTimeLimitation( jbsj   )  >= 0 ) error=0;
			if( 0 != strcmp( "null", zbqssj ) && 0 ==error )  
			if( (error=UP_ZBSJ) && CheckTimeLimitation( zbqssj )  >= 0 ) error=0;  
			if( 0 != strcmp( "null", zbjssj ) && 0 ==error )   
			if( (error=UP_ZBSJ) && CheckTimeLimitation( zbjssj )  <= 0 ) error=0;  
				
			if( 0 ==error )  	
			if( (error=UP_ZBDIS) && zbbj  >= zbdis ) 										 error=0;
			if( 0 ==error ) 
			if( (error=UP_JBDIS) && jbbj  <= jbdis )										 error=0; 
			{
				//compare time
				//error=0;
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
/**************************************************************************************************************************/ 
 int CVICALLBACK WCDownloadCtrCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		//////////////////////////////////////////////////////////////////////////////////////////// 
		case EVENT_COMMIT:
		{
			char info[100]; 
			SetCtrlAttribute  ( panel, TABPANEL_3_WLBUTTON, ATTR_LABEL_TEXT , "正在处理" );
			ThreadPanel = panel;                                            
			SYSTEM_CreateThread ( DownloadThreadID, Thread_WorkCodeDownLoad, &ThreadPanel);        
		}
		break;

	}
	return 0;
}
/**************************************************************************************************************************/ 
int CVICALLBACK DownloadDoneCtrCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		//////////////////////////////////////////////////////////////////////////////////////////// 
		case EVENT_COMMIT:
		{
			CallPanelCallback ( WCDLPanelHandle, EVENT_CLOSE, 0, 0, 0 );    
		}
		break; 
	}
	
	return 0; 
}
/**************************************************************************************************************************/  

int CVICALLBACK FileBrowserCtrCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		//////////////////////////////////////////////////////////////////////////////////////////// 
		case EVENT_COMMIT:
		{
			////////////////////////////////////////////////////////////////////////////////////////////  
			if( TABPANEL_L_FILBUTTON == control )
		  {
			 	char title[100] = "请选择待解密的工作码离线文件";
			  if( 0 < MultiFileSelectPopupEx ("", ".txt", ".txt", title, 0, 1, &numOfFiles, &fileList ))
				{
					SetTableCellRangeVals (panel, TABPANEL_L_TABLE, MakeRect( 1, 2, numOfFiles, 1), fileList, VAL_COLUMN_MAJOR );
					SetCtrlAttribute      ( panel, TABPANEL_L_NEXTBUTTON, ATTR_DIMMED, 0 );
				}
			}
			
			////////////////////////////////////////////////////////////////////////////////////////////  
			if( TABPANEL_L_NEXTBUTTON == control )
			{
				if( numOfFiles )	fileKeyList = (char**)calloc( numOfFiles, sizeof(char*) );
				for( int i=0; i<numOfFiles; i++ )
				{
					fileKeyList[i] = calloc( 50, sizeof(char)); 			
				}
				
				GetTableCellRangeVals (panel, TABPANEL_L_TABLE, MakeRect( 1, 3, numOfFiles, 1), fileKeyList, VAL_COLUMN_MAJOR ); 
				
				int keyCheck=1;
				for( int i=0; i<numOfFiles; i++ )  
				{
					if( 0 == strcmp( "", fileKeyList[i] ) || FILE_KEY_LEN != strlen( fileKeyList[i] ))
					{
						SetActiveTableCell (panel, TABPANEL_L_TABLE, MakePoint ( 3, i+1));
						MessagePopup ( "Warning", "无效的文件序列号D，请重新检查！");   
						keyCheck = 0; break;
					}
				}

				if( keyCheck )
				{
					int lastIndex;     
					GetActiveTabPage  ( WCDLPanelHandle, WCDL_PANEL_TAB, &lastIndex );      
					SetCtrlAttribute  ( WCDLPanelHandle, WCDL_PANEL_TAB, ATTR_CTRL_INDEX, ++lastIndex); //3-DT_LOCFILE+lastIndex
					CallCtrlCallback  ( WCDLPanelHandle, WCDL_PANEL_TAB, EVENT_ACTIVE_TAB_CHANGE, 0, 0, 0 );  
				}
			}
		}
		break;
		//////////////////////////////////////////////////////////////////////////////////////////// 
		case EVENT_DISCARD:
		{
					
		}break;
	}
	return 0;
}
