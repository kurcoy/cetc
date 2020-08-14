#include <windows.h>   
#include "toolbox.h"
#include "progressbar.h"
#include <utility.h>
#include <ansi_c.h>
#include <userint.h>
#include "UserOperation.h"
#include "UserPanel.h"
#include "WorkingCodeSys.h"

#include "mysqlite.h"

int UserModuleID;

int infoTableID, ruleTableID, resultTableID;  

unsigned char KeyStr[24]= "jadl12345678912345678912";  
unsigned char webKeyStr[18]= "jadl12345678901234";  

extern char *DataBaseName; 
extern char *DBKey;
char *ResultTable  = "ResultTable";
char *RuleTable    = "RuleTable"; 
char *InfoTable    = "InfoTable"; 

char SerialPort[10];

extern char sysRootPath[300]; 

int  MAX_DET_NUM = 40;      
int  NumOfUIDs = 0; 
char **UIDArray; 
int ComPortID = 0; 

int MainpanelHandle;
int WorkCodePanelHandle   = 0;
int BlastingPanelHandle   = 0;  
int WorkResultPanelHandle = 0;

int BlastingEnable = 0;   

int nScreenWidth, nScreenHeight;    

int CVICALLBACK ExplosureCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int CVICALLBACK MainPanelCallBack (int panel, int event, void *callbackData, int eventData1, int eventData2); 

/**************************************************************************************************************************/
RETVAL USEROPERATION_GetErrorText ( int numError, char **infoError ) 
{
	char *MessageText = "End of List";
  switch( numError )
  {
    case UP_INST_MODULE:                MessageText = "Installation failed";                       break;
		case UP_ENCRYT:                     MessageText = "��������ܴ���";                            break;   
		case UP_DECRYT:                     MessageText = "��������ܴ���";                            break;  	
		case UP_NETWORK:                    MessageText = "���粻���ã������¼��";                    break;   
		case UP_DATABASE:                   MessageText = "���ݿ��������";                            break;   
		case UP_INFOPARSE:							  	MessageText = "�������������";                            break; 
		case UP_UIDNUM:										  MessageText = "�׹�UID�Ĺ����벻����";                     break;
		case UP_UPLOADED:									  MessageText = "�׹���ҵ��Ϣ���ϴ�";                        break;  
		case UP_DEV_INVALID:							  MessageText = "�𱬿��������Ӵ���";                        break;  
		
		case UP_JBDIS:									 	  MessageText = "���Ƶص㴦�ڽ������Ʒ�Χ��";   		         break;  
		case UP_ZBDIS:									 	  MessageText = "���Ƶص㲻��׼����Χ��";      			         break;  
		case UP_JBSJ:									    	MessageText = "����ʱ�䴦�ڽ�������ʱ����";  			         break;  
		case UP_ZBSJ:									 	    MessageText = "����ʱ�䲻��׼��ʱ�䷶Χ��";  			         break;  
																																												  
		case UP_DL_REQUEST:  						 	  MessageText = "�����飩�Ƿ���������Ϣ";             			  break;
		case UP_DL_NOTFOUND:  					 	  MessageText = "�����飩δ�ҵ��������豸��Ϣ������δ�������񣬻��׹���ʹ��";  break;  
		case UP_DL_NOTTASK:  						 	  MessageText = "�����飩������δ������δ��������";         break;
		case UP_DL_BLACKLIST: 					 	  MessageText = "�����飩�����ں�������";                  	break;  
		case UP_DL_BEYOND: 						  	  MessageText = "�����飩��λ�ò�����������";       	    break;  
		case UP_DL_WITHIN: 						 	    MessageText = "�����飩����Wλ���ڽ���������";         	  break;  
		case UP_DL_NOTEXIST: 						 	  MessageText = "�����飩������ע��/����";               	  break; 
		case UP_DL_NOTASK: 						 	    MessageText = "�����飩��������";                       	  break; 
		case UP_DL_PROJECT: 		  			 	  MessageText = "�����飩��ҵ��ͬ������Ŀ";                   break;   
		case UP_DL_NOTSETAREA:   				 	  MessageText = "�����飩��ҵ����δ����׼������";          	  break;   
		case UP_DL_OFFLINE: 		  			 	  MessageText = "�����飩�������ز�֧�����������Ա�";         break;  
		case UP_DL_NOTSET: 		  				 	  MessageText = "�����飩Ӫҵ�Ե�λ�������ú�ͬ����Ŀ";       break;    
		case UP_DL_NETWORK: 		  			 	  MessageText = "�����飩��������ʧ��";                       break;   
		
		case UP_DET_BLACKLIST:     			 	  MessageText = "�����飩�׹��ں�������";                     break; 
		case UP_DET_USE:    				 	      MessageText = "�����飩�׹���ʹ��";                         break; 
		case UP_DET_NOTEXIST:     			 	  MessageText = "�����飩������׹�UID������";                break; 
		
    default:                            numError    = -1; MessageText = "Unknown Error";            break;
  }
	
	if( infoError ) strcpy( infoError, MessageText);	
	
	return RET_NOERR;
}

RETIDX ErrorListing ( MSGTYPE Type, int numError, char *addInfoText )
{
  char MessageText[500]; 
	USEROPERATION_GetErrorText ( numError, &MessageText );    
	
  SYSTEM_GenerateMessage ( Type, UserModuleID, numError, MessageText );

  return numError;
}

static RETVAL UnInstall ( int SysModuleID,  int PanelID,   void *funcData )
{
	return RET_NOERR;
}

RETVAL USEROPERATION_Install  ( int _SysModuleID,  int PanelID, void *funcData )
{
	UserModuleID = _SysModuleID; 
	
	SYSTEM_SetSysModuleUninstFunc    ( UserModuleID, UnInstall, 0  );
  SYSTEM_SetSysModuleErrListFunc   ( UserModuleID, ErrorListing  );
	
	return RET_NOERR;  
}
/**************************************************************************************************************************/                        
int DisplayMessage( int Panel, int TextBoxCtrl, char* Info )
{
	if(Info)
	{
		time_t time_T;
		time ( &time_T );
		struct tm *tmTime = localtime(&time_T);
		char* format = "%Y-%m-%d %H:%M:%S";
  	char strTime[100];
  	strftime(strTime, sizeof(strTime), format, tmTime);	
		
		char mesg[1000];
		int  index=0; 
		index += sprintf( &mesg[index], "[%s] ", strTime ); 
		strcat( mesg, Info );
		
		InsertTextBoxLine ( Panel, TextBoxCtrl, -1, mesg);
	}
	
	return 0;
}

/**************************************************************************************************************************/
int StrTimeToCalenderTime(  char *strTime,  struct tm *cTime )
{
	if( NULL != strTime && NULL != cTime)
	{
		char str[3], stry[5]; 
		int value; 
		str[2]  = 0x00;
		stry[4] = 0x00;  
		strncpy ( stry, strTime,  4  );
		cTime->tm_year = ( atoi ( stry ) - 1900 );
	
		strncpy ( str, &strTime[5], 2 );
		cTime->tm_mon = ( atoi( str ) -1 );

		strncpy ( str, &strTime[8], 2 );
		cTime->tm_mday = atoi( str );

		strncpy ( str, &strTime[11], 2 );
		cTime->tm_hour = atoi( str     );

		strncpy ( str, &strTime[14], 2 );
		cTime->tm_min = atoi( str      );
		
		strncpy ( str, &strTime[17], 2 );
		cTime->tm_sec = atoi( str      );
		
	return 1;
	}
	return -1; 
}
/**************************************************************************************************************************/   
int DetonatorMonitor( void )
{
	char **RequestData;
	int NumOfWait = SQLITE_SearchRequest( DataBaseName, ResultTable, "UploadTime", "null", "Downloadtime", &RequestData );	

	double delta = 0;  
	int daycount = 3;
	
  time_t time_T;
	time ( &time_T );
	
	for(int i=0; i<NumOfWait; i++)
	{
		struct tm cTime; 
		StrTimeToCalenderTime( RequestData[i], &cTime ); 
		time_t time_start = mktime( &cTime ); 
	  delta = difftime(time_T, time_start);

		if( delta > daycount *24*60*60 ) break; 
	}
	
	if( delta < daycount *24*60*60 ) return 0;

	return -1; 
}		
/**************************************************************************************************************************/
int InitInfoDataBase( void )
{
	char *infoTableItem[] ={ "TimeStamp", "sqrq", "yxq", "sbbh", "gzm", "gzmcw","UID" ,"" }; //"fbh", "gzmcw","UID" ,""
	char *ruleTableItem[] ={ "jbqyjd", "jbqywd", "jbqybj", "jbjzsj", "zbqymc",  "zbqyjd", "zbqywd", "zbqybj","zbqssj", "zbjzsj", "htid", "xmbh", "" };    
	char *resultTabItem[] ={ "sbbh", "jd", "wd", "bpsj", "UploadTime", "DownloadTime", "bprysfz",  "xmbh", "htid", "dwdm", "UID", "" }; 
   
	if( (infoTableID   = SQLITE_OpenDataBase ( sysRootPath, DataBaseName, InfoTable,   infoTableItem ))>0 )
	if( (ruleTableID   = SQLITE_OpenDataBase ( sysRootPath, DataBaseName, RuleTable,   ruleTableItem ))>0 )
	if( (resultTableID = SQLITE_OpenDataBase ( sysRootPath, DataBaseName, ResultTable, resultTabItem ))>0 )  	
	{
		return 0;
	}
	
	return -1; 
}
/**************************************************************************************************************************/ 
int CVICALLBACK MainPanelCallBack (int panel, int event, void *callbackData, int eventData1, int eventData2 )
{
	switch (event)
	{
		case EVENT_PANEL_RESTORE:
		{
			if( 0 == MainpanelHandle )
			{
				MainpanelHandle = panel;
				if( eventData1 > 0 ) 	 nScreenWidth  = eventData1;
				if( eventData2 > 0 )	 nScreenHeight = eventData2;

				int error; 
				if( (error=1) && 0 != InitInfoDataBase(  ))
				{
					MessagePopup      ( "Error", "���ݿ⿪��������������������"); 
					//generate panel close event
				}
				
				if( (error=2) && 0 != DetonatorMonitor(  ))
				{
					MessagePopup      ( "Warning", "���ڳ����涨����δ�ϴ����׹ܣ��뼰ʱ�ϴ���ҵ��Ϣ"); 
					//generate panel close event
				}
				
				if( !UIDArray )     { UIDArray  = (char**) calloc( MAX_DET_NUM, sizeof(char*) ); }	 
				for( int i=0; i<MAX_DET_NUM; i++ )
				{
					UIDArray[i] = calloc( 20, sizeof(char));
				}
				
				//check denotator info from local database
			}
		}
			break;
		case EVENT_LOST_FOCUS:

			break;
			
		case EVENT_GOT_FOCUS: 
		{
			if( BlastingEnable>0 )	SetCtrlAttribute  ( panel, MAIN_PANEL_EX_BUTTON, ATTR_DIMMED, 0);	
			if( BlastingEnable>0 )	SetCtrlAttribute  ( panel, MAIN_PANEL_UL_BUTTON, ATTR_DIMMED, 0);	    
		}
		break;
		
		case EVENT_CLOSE:
		{
			if( WorkCodePanelHandle )
			{
				DiscardPanel (WorkCodePanelHandle);		 
				WorkCodePanelHandle = 0;
			}
			else if( WorkResultPanelHandle ) 
		  {
				DiscardPanel (WorkResultPanelHandle);	
				WorkResultPanelHandle = 0; 
		  }
			else if( BlastingPanelHandle ) 
		  {
				DiscardPanel (BlastingPanelHandle);	
				BlastingPanelHandle = 0; 
		  }
			else
			{
				NumOfUIDs = 0;
				if( UIDArray   )   
				{ 
					for( int i=0; i<MAX_DET_NUM; i++ )
					{
					  if(UIDArray[i]) { free(UIDArray[i]); UIDArray[i] = NULL;} 
					}
					free(UIDArray ); UIDArray = NULL;
				}

				SQLITE_Encrypt ( DataBaseName, DBKey ); 
				
				DiscardPanel (panel);
				QuitUserInterface (0); 
			}
		}
		break;
	}
	return 0;
}
/**************************************************************************************************************************/  
int CVICALLBACK BlastingCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			BlastingPanelHandle = LoadPanel ( MainpanelHandle, "100_UserFunction\\UserPanel.uir", BST_PANEL );

			SetPanelAttribute(  BlastingPanelHandle, ATTR_HEIGHT, nScreenHeight ); 
			SetPanelAttribute(  BlastingPanelHandle, ATTR_WIDTH,  nScreenWidth );   
			SetPanelAttribute(  BlastingPanelHandle, ATTR_LEFT,   0 ); 
			SetPanelAttribute(  BlastingPanelHandle, ATTR_TOP,    0 );

			CallPanelCallback ( BlastingPanelHandle, EVENT_PANEL_RESTORE, nScreenWidth, nScreenHeight, 0 ); 
		}
		break;
		
	}
	return 0;
}
/**************************************************************************************************************************/  
int CVICALLBACK InfoUploadCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			WorkResultPanelHandle = LoadPanel ( MainpanelHandle, "100_UserFunction\\UserPanel.uir", WRUL_PANEL );
			
			//CallPanelCallback ( WorkResultPanelHandle, EVENT_PANEL_RESTORE, nScreenWidth, nScreenHeight, 0 ); 
			SetPanelAttribute(  WorkResultPanelHandle, ATTR_HEIGHT, nScreenHeight ); 
			SetPanelAttribute(  WorkResultPanelHandle, ATTR_WIDTH,  nScreenWidth );   
			SetPanelAttribute(  WorkResultPanelHandle, ATTR_LEFT,   0 ); 
			SetPanelAttribute(  WorkResultPanelHandle, ATTR_TOP,    0 );
			
			DisplayPanel (WorkResultPanelHandle); 
			
		}
		break;
		
	}
	return 0;
}
/**************************************************************************************************************************/  
int CVICALLBACK InfoDownloadCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			WorkCodePanelHandle = LoadPanel ( MainpanelHandle, "100_UserFunction\\UserPanel.uir", WCDL_PANEL);
			
			//CallPanelCallback ( WorkCodePanelHandle, EVENT_PANEL_RESTORE, nScreenWidth, nScreenHeight, 0 );
			SetPanelAttribute(  WorkCodePanelHandle, ATTR_HEIGHT, nScreenHeight ); 
			SetPanelAttribute(  WorkCodePanelHandle, ATTR_WIDTH,  nScreenWidth );   
			SetPanelAttribute(  WorkCodePanelHandle, ATTR_LEFT,   0 ); 
			SetPanelAttribute(  WorkCodePanelHandle, ATTR_TOP,    0 );
			
			DisplayPanel (WorkCodePanelHandle);   
		}
			break;
		
		default:
			break; 
	}
	return 0;
}

