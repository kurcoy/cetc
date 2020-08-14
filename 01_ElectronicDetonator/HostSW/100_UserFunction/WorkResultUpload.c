#include <userint.h>
#include <cvidef.h>
#include <ansi_c.h>
#include "UserPanel.h"      
#include "WorkingCodeSys.h" 
#include <windows.h> 
//#include "MSCOMCT2.h"
#include "toolbox.h"
#include "mysqlite.h"
#include "useroperation.h"
 /**************************************************************************************************************************/  
 //TODO: 
 //1. single or multipile selection and upload
 //2. all the info for one file should be the same,
 //3. the table should check the item 2.
/**************************************************************************************************************************/  
typedef enum
{
	UL_READ, 
	UL_UPLOAD,
}UL_EVENT;

UL_EVENT uploadEvent;    
int WRUPPanelHandle=0;

char **ResultfileName;
//char *rootPath= "C:\\Users\\Cliff\\Documents\\National Instruments\\CVI\\HostSW\\900_ResultFile\\DownloadResult\\";
char *fileType= "*.txt";
int  FileNumber=0;
int  *NumOfInfo;
int  *SelNumOfInfo;  
int  TotalNumOfInfo;
int  NumOfResultItem=0;  
int  NumOfDBItem;
int  NumOfDispalyItem=11;
int numCols, numRows;      
	
static int UploadThreadID = 0;
static int ThreadRunning = 0;
static int ThreadPanel = 0;

char* UpLoadUrl = "http://test.mbdzlg.com/mbdzlgtxzx/servlet/DzlgSysbJsonServlert"; 
extern unsigned char KeyStr[24]; 
extern int infoTableID, ruleTableID, resultTableID;
extern char *DataBaseName;
extern char *ResultTable;
extern char *RuleTable;
extern char *InfoTable;

extern char sysRootPath[300];  

char **InfoUpload;
char **InfoString;
char **tableInfo;

static char ***UIDArray, ***SelArray, ***WorkcodeInfo;  

INFO_RESULT	 *ResultArray; 

static int WorkResultUpLoad						( int panel, char *paramStr );
static int WorkResultRead				      ( int panel );
static int WorkResultReadFromDataBase (  int panel, int NumOfInfo );
extern int DisplayMessage( int Panel, int TextBoxCtrl, char* Info );   
/**************************************************************************************************************************/
int Thread_WorkResultUpLoad( void *funcData )
{
	ThreadRunning = 1;   
	int *Event = (int*)(funcData);
	
	int currIndex, panel; 
	//GetActiveTabPage  ( WRUPPanelHandle, WCDL_PANEL_TAB, &currIndex );  
	//GetPanelHandleFromTabPage ( WRUPPanelHandle, WCDL_PANEL_TAB, currIndex, &panel);
	GetActiveTabPage  				( WRUPPanelHandle, WRUL_PANEL_TAB, &currIndex );  
	GetPanelHandleFromTabPage ( WRUPPanelHandle, WRUL_PANEL_TAB, currIndex, &panel); 
	
	switch( *Event )
	{
		case UL_UPLOAD: 
		{
			SetCtrlAttribute  ( panel, TABPANEL_7_WRULBUTTON, ATTR_LABEL_TEXT , "正在上传" );
			SetCtrlAttribute  ( panel, TABPANEL_7_WRULBUTTON, ATTR_DIMMED,      1          ); 
			char info[500];
	 
			int uploaded = 0;
			int error=0;
			int color = VAL_GREEN;  
	
			sprintf( info, "总共有 %d 个文件 %d 发雷管作业信息", FileNumber, TotalNumOfInfo);
			DisplayMessage( panel, TABPANEL_7_TEXTBOX, info ); 
			for( int i=0; i<FileNumber; i++ )
			{
				if( 0 == SelNumOfInfo[i]) continue;
				
				//sprintf( info, "-->正在上传第 %d 个文件 %d 发雷管作业信息", i+1, SelNumOfInfo[i]);
				sprintf( info, "-->正在上传第 %d 个文件", i+1);   
				DisplayMessage( panel, TABPANEL_7_TEXTBOX, info ); 
		
				sprintf( info, "-->需要上传 %d 发雷管作业信息，用户取消上传 %d 发雷管作业信息", SelNumOfInfo[i], NumOfInfo[i]-SelNumOfInfo[i]);     
				DisplayMessage( panel, TABPANEL_7_TEXTBOX, info );   
				
				error = WorkResultUpLoad  ( panel,InfoString[i] );
				
				if( 0 == error )
				{
					sprintf( info, "--><OK>成功上传第%d文件%d发雷管作业信息", i+1, SelNumOfInfo[i]);
					
					//update into database, 3-d array can not direct address
					char **uidstr  = UIDArray[i];  
					char **selSta  = SelArray[i];  
					int    index=0, n=0, k=0; 
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
					
					char **resultData = (char**) calloc( NumOfInfo[i], sizeof(char*) ); 
					char **instr = WorkcodeInfo[n++]; //TODOTODOTODO    
					for( int j=0;j<NumOfInfo[i];j++ )
					{
						//char **instr = WorkcodeInfo[n++]; //TODOTODOTODO              
						
						resultData[j] = calloc( 1000, sizeof(char) );
						if( 0x00 != *uidstr[j] )
						{	
							int rindex = 0; 
							rindex += sprintf( &resultData[k][rindex], "%s,", instr[1] );
							rindex += sprintf( &resultData[k][rindex], "%s,", instr[2] ); 
							rindex += sprintf( &resultData[k][rindex], "%s,", instr[3] );
							rindex += sprintf( &resultData[k][rindex], "%s,", instr[4] );
							rindex += sprintf( &resultData[k][rindex], "%s,", strTime );	 //UploadTime
							
							rindex += sprintf( &resultData[k][rindex], "%s,", instr[6] );	 //DownloaTime 
							rindex += sprintf( &resultData[k][rindex], "%s,", instr[7] ); //BPRYSFZ
							rindex += sprintf( &resultData[k][rindex], "%s,", instr[8] ); //XBBH   
							rindex += sprintf( &resultData[k][rindex], "%s,", instr[9] ); //HTID   
							rindex += sprintf( &resultData[k][rindex], "%s,", instr[10] ); //DWDM
							rindex += sprintf( &resultData[k][rindex], "%s",  uidstr[j] ); 
							k++;
						}
					}
					
					error = UP_DATABASE;
					if( RET_NOERR == SQLITE_InsertIntoDB ( resultTableID, resultData, SelNumOfInfo[i], 5 )) 
				  {
						uploaded += SelNumOfInfo[i];
						error = 0;
						for( int n=0;n<SelNumOfInfo[i];n++ )	 { strcpy( selSta[n], "成功"); }
					}
		
					if( resultData )
					{
						for( int k=0;k<NumOfInfo[i];k++ )
						{
							if( resultData[k] ) { free(resultData[k]); resultData[k]=NULL; }
						}
						free(resultData); resultData=NULL;
					}
					
					char oldFileName[500], newFileName[500]; 
					strcpy( oldFileName, sysRootPath );
					strcat( oldFileName, "\\800_External\\Result\\" );  
				  strcat( oldFileName, ResultfileName[i] );
		
					strcpy( newFileName, sysRootPath ); 
					strcat( newFileName, "\\900_ResultFile\\ResultReport\\" ); 
				  strcat( newFileName, ResultfileName[i] );
		
					rename ( oldFileName, newFileName);
 
				}
				else
				{
					char Errorinfo[500];
					USEROPERATION_GetErrorText( error, &Errorinfo );        
					sprintf( info, "--><NOK>失败上传第%d文件%d发雷管作业信息,错误代码%d, %s", i+1, SelNumOfInfo[i], error, Errorinfo);
					
					color = VAL_RED; 
					//to get error info and display in here
				}
				DisplayMessage( panel, TABPANEL_7_TEXTBOX, info );  
				SetCtrlAttribute  ( panel, TABPANEL_7_PROGRESSBAR, ATTR_CTRL_VAL, (double)(100*(i+1)/FileNumber));   
			}
			
			char result[100];
			SetCtrlAttribute  ( panel, TABPANEL_7_TEXTBOX, ATTR_TEXT_COLOR, color ); 
			sprintf( result, "雷管作业信息上传成功%d", uploaded);
			DisplayMessage( panel, TABPANEL_7_TEXTBOX, result ); 
		
			SetCtrlAttribute  ( panel, TABPANEL_7_WRULBUTTON, ATTR_VISIBLE, 0 ); 
			SetCtrlAttribute  ( panel, TABPANEL_7_NEXTBUTTON, ATTR_VISIBLE, 1 );  
			
			
			//SetCtrlAttribute  ( panel, TABPANEL_7_PROGRESSBAR, ATTR_FILL_COLOR, color);  
			ThreadRunning = 0; 
		}
		break; 
		
		case UL_READ:
		{
			WorkResultRead( panel );	
		}
		break;
	}
	return 0;
}
/**************************************************************************************************************************/     
int RecurseFileFolder( char *FilePath, char***FileNames )
{
	if( NULL != FilePath && NULL != FileNames )
	{
		long Handle;
		int  number=0;
		WIN32_FIND_DATAW FindFileData;
 
		Handle = FindFirstFile(FilePath, &FindFileData );
		if( INVALID_HANDLE_VALUE != Handle )
		{
			do
			{
				if(FindFileData.dwFileAttributes == FILE_ATTRIBUTE_ARCHIVE )
				{
					char fileName[50]; 
					strcpy( fileName, FindFileData.cFileName ); ++number;
				 }
			 }while( FindNextFile(Handle, &FindFileData) );
			
			 *FileNames = (char**)calloc( number, sizeof(char*)); 
			 
			 Handle = FindFirstFile(FilePath, &FindFileData );  
			 for( int i=0; i<number; i++ )
			 {
			   (*FileNames)[i] = calloc( 50, sizeof(char)); 
				 strcpy( (*FileNames)[i], FindFileData.cFileName ); 
				 FindNextFile(Handle, &FindFileData);
			 }
			}	
		return number;
	}
}

/**************************************************************************************************************************/      
int CVICALLBACK InfoPanelCallBack (int panel, int event, void *callbackData, int eventData1, int eventData2 )
{
	switch (event)
	{
		case EVENT_PANEL_RESTORE: 
		{
			// check local denotator info and update
			WRUPPanelHandle = panel;
			
			//SetPanelAttribute(  panel, ATTR_HEIGHT, eventData2 ); 
			//SetPanelAttribute(  panel, ATTR_WIDTH,  eventData1  );  
			//SetPanelAttribute(  panel, ATTR_LEFT,   0 ); 
			//SetPanelAttribute(  panel, ATTR_TOP,    0 );			
			//SetPanelAttribute ( panel, ATTR_WINDOW_ZOOM, VAL_MAXIMIZE );
			
			if( 0 == UploadThreadID )
			if(RET_ERROR == SYSTEM_CreateThreadIdentifier ( 1, &UploadThreadID ))
			{
				MessagePopup      ( "Error", "作业信息上传线程开启失败，请重新启动程序"); 	
				CallPanelCallback ( panel, EVENT_CLOSE, 0, 0, 0 );   
	 		}  
		}
		break; 
		
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_DISCARD: case EVENT_CLOSE:    
		{
			SYSTEM_TerminateThread( UploadThreadID); 
			if( InfoString )
			{
				for( int i=0; i<FileNumber; i++ )
				{
					if( InfoString[i] ) { free(InfoString[i]); InfoString[i] =NULL; }
				}
				
				free(InfoString); InfoString=NULL;
			} 
			
			if( NumOfInfo )    { free(NumOfInfo);    NumOfInfo    =NULL;}
			if( SelNumOfInfo ) { free(SelNumOfInfo); SelNumOfInfo =NULL;}  
			if( ResultfileName )																		
			{
				for( int i=0; i<FileNumber; i++ )	
				{
					if( ResultfileName[i] ) { free(ResultfileName[i]); ResultfileName[i] = NULL; }	
				}
				 free(ResultfileName); ResultfileName = NULL;
			}
			
			if( WorkcodeInfo )
			{
				for( int i=0; i<FileNumber; i++ )	
				{
					if( WorkcodeInfo[i] ) { free(WorkcodeInfo[i]); WorkcodeInfo[i] = NULL; }	
				}
				 free(WorkcodeInfo); WorkcodeInfo = NULL;
			}  
			
			if( UIDArray )
			{
				for( int i=0; i<FileNumber; i++ )	
				{
					if( UIDArray[i] ) { free(UIDArray[i]); UIDArray[i] = NULL; }	
				}
				 free(UIDArray); UIDArray = NULL;
			}  
			
			if( SelArray )
			{
				for( int i=0; i<FileNumber; i++ )	
				{
					if( SelArray[i] ) { free(SelArray[i]); SelArray[i] = NULL; }	
				}
				 free(SelArray); SelArray = NULL;
			}  
			
			if(tableInfo)
			{
				for( int i=0; i<(numCols-1)*numRows; i++ )	
				{
					if( tableInfo[i] ) { free(tableInfo[i]); tableInfo[i] = NULL; }	
				}
				 free(tableInfo); tableInfo = NULL;	
			}
			
			FileNumber=0;
		  NumOfResultItem=0;
			TotalNumOfInfo = 0;
			//UploadThreadID = 0;
			//DiscardPanel (panel);
			HidePanel(panel); 
			
		}break;
	}
	return 0;
}
/**************************************************************************************************************************/  
int CVICALLBACK InfoTabCtrlCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:
		{
			CallPanelCallback ( panel, EVENT_PANEL_RESTORE, 0, 0, 0 );
			SetCtrlAttribute  ( panel, WRUL_PANEL_TAB, ATTR_TABS_VISIBLE, 0 );  
		}
		break;
	}
	return 0;
}
/**************************************************************************************************************************/  
/*
int CVICALLBACK DateGetCtrCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2 )
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			//get calendar date
			char currIndex[100];
			CAObjHandle DBobjectHandle, DEobjectHandle;
			ERRORINFO errorInfo[200];
			VARIANT bvDay, bvMonth, bvYear, evDay, evMonth, evYear;
			int bday, bmonth, byear, eday, emonth, eyear; 
				
			if( 0 == GetObjHandleFromActiveXCtrl (panel, TABPANEL_4_DTPICKER_BEGIN, &DBobjectHandle )) 
			if( 0 == GetObjHandleFromActiveXCtrl (panel, TABPANEL_4_DTPICKER_END, &DEobjectHandle ))  	
			if( 0 == MSComCtl2_IDTPickerGetDay   ( DBobjectHandle, errorInfo,  &bvDay))
			if( 0 == MSComCtl2_IDTPickerGetMonth ( DBobjectHandle, errorInfo,  &bvMonth))  
			if( 0 == MSComCtl2_IDTPickerGetYear  ( DBobjectHandle, errorInfo,  &bvYear))
			if( 0 == MSComCtl2_IDTPickerGetDay   ( DEobjectHandle, errorInfo,  &evDay))
			if( 0 == MSComCtl2_IDTPickerGetMonth ( DEobjectHandle, errorInfo,  &evMonth))  
			if( 0 == MSComCtl2_IDTPickerGetYear  ( DEobjectHandle, errorInfo,  &evYear))
			{
				bday   = bvDay.n1.decVal.Lo32;
				bmonth = bvMonth.n1.decVal.Lo32;  
				byear  = bvYear.n1.decVal.Lo32;  
				eday   = evDay.n1.decVal.Lo32;
				emonth = evMonth.n1.decVal.Lo32;  
				eyear  = evYear.n1.decVal.Lo32;  
			}
			CA_DiscardObjHandle (DBobjectHandle);
			CA_DiscardObjHandle (DEobjectHandle);
		}break;
			
		case EVENT_DISCARD:

			break;
	}
	return 0;
}
*/
/***************************************************************************************************************************/
//precondition: the info for 1 file is exactly same.					TODOTODOTODOTODOTODOTODOTODO
/***************************************************************************************************************************/   
int GenerateFormatString( int panel, char **paramStr, char ***infoStr, char ***uidStr, char ***selStr, int CurrNumOfUIDs, int *NumOfUIDs )
{
	int error=-1, availableUID=0;
	char **resultInfo  = (char**) calloc( NumOfDispalyItem, sizeof(char*) );
	*infoStr = resultInfo;  
	for( int i=0; i<NumOfDispalyItem; i++ )
	{
		resultInfo[i] = calloc( 50, sizeof(char));	
	}
	
	char **usersel = (char**) calloc( CurrNumOfUIDs, sizeof(char*) );     
	char **uids    = (char**) calloc( CurrNumOfUIDs, sizeof(char*) );
	*uidStr = uids; *selStr = usersel;        
	for( int j=0; j<CurrNumOfUIDs; j++ )
	{
		uids[j]    = calloc( 20, sizeof(char));
		usersel[j] = calloc( 20, sizeof(char));      
	}
	
	int top = TotalNumOfInfo-CurrNumOfUIDs+1;
	Rect infoRange=MakeRect ( top, 2, 1, NumOfDispalyItem);
	Rect UIDRange =MakeRect ( top, 2, CurrNumOfUIDs, 1);
	Rect selRange =MakeRect ( top, numCols, CurrNumOfUIDs, 1); 
	
	GetTableCellRangeVals   ( panel, TABPANEL_5_TABLE, infoRange, resultInfo, VAL_ROW_MAJOR   ); 
	GetTableCellRangeVals   ( panel, TABPANEL_5_TABLE, UIDRange,  uids,       VAL_COLUMN_MAJOR);
	GetTableCellRangeVals   ( panel, TABPANEL_5_TABLE, selRange,  usersel,    VAL_COLUMN_MAJOR); 
	
	char *WorkcodeUID = calloc( 20*(CurrNumOfUIDs), sizeof(char) ); 
	int index=0, i=0; 
	for( i=0; i<CurrNumOfUIDs; i++ )
	{
		if( 0 == strcmp( usersel[i], "Yes"))
		{
			if( i>0 )	index += sprintf( &WorkcodeUID[index], "%s", "," ); 
			index += sprintf( &WorkcodeUID[index], "%s", uids[i] );
			availableUID++;
			strcpy( usersel[i], "失败"); 
		}
		else
		{
			memset( uids[i], NULL, 20);	
			strcpy( usersel[i], "用户取消"); 
		}
	}
	
	*NumOfUIDs = availableUID; 
	
	if( WorkcodeUID && *NumOfUIDs > 0 )
	{
		cJSON *wcObject = cJSON_CreateObject(  );
  	cJSON_AddStringToObject(wcObject, "sbbh",    resultInfo[1]);
  	cJSON_AddStringToObject(wcObject, "jd",      &resultInfo[2][2]);
		cJSON_AddStringToObject(wcObject, "wd",      &resultInfo[3][2]); 
		cJSON_AddStringToObject(wcObject, "bpsj",    resultInfo[4]);  
		cJSON_AddStringToObject(wcObject, "bprysfz", resultInfo[7]);
	
		cJSON_AddStringToObject(wcObject, "uid",     WorkcodeUID  );      
		cJSON_AddStringToObject(wcObject, "xmbh", 	 resultInfo[8]);      
		cJSON_AddStringToObject(wcObject, "htid", 	 resultInfo[9]);      
		cJSON_AddStringToObject(wcObject, "dwdm",    resultInfo[10]);      
 
		*paramStr =  cJSON_PrintUnformatted( wcObject );
		error = 0;
	}
	
	if( WorkcodeUID ) { free(WorkcodeUID); WorkcodeUID = NULL; }   
	
	return error;	
}
/***************************************************************************************************************************/ 
static int WorkResultReadFromDataBase(  int panel, int NumOfInfo ) 
{
	//read info from local database
	char **RequestData; 																																											
	char uidValue[20];
	int  num, error=UP_DATABASE;
	int  NumOfDBItem=0; 
	if( 0 == GetTableCellVal (panel, TABPANEL_5_TABLE, MakePoint (2, NumOfInfo), uidValue ))
	if(uidValue )
	if( num = SQLITE_SearchRequest( DataBaseName, ResultTable, "UID", uidValue, "*", &RequestData ))
	if( num>0 && RequestData )
	{
		error=0;
		const char s[2] = ",";
    char *token;
		token = strtok(*RequestData, s);

    while( token != NULL) 
		{
      ++NumOfDBItem;

			if( NumOfDBItem >= 7 && NumOfDBItem <= 11) SetTableCellVal (panel, TABPANEL_5_TABLE, MakePoint (NumOfDBItem+1, NumOfInfo), token ); 
			else
			{
				if( 6 == NumOfDBItem )	
				if( 0 != strcmp("null", token ) ) {	error=UP_UPLOADED; break; }
			}
      token = strtok(NULL, s);
		}

		if( RequestData )
		{
			for( int i=0; i<num; i++ )
			{
				if( RequestData[i] ) { free(RequestData[i]); RequestData[i] = NULL; }
			}
			free(RequestData); RequestData = NULL;
		}
		
		//if( NumOfDBItem=12 ) error=0; 
	}
	
	return error;
}
/***************************************************************************************************************************/ 
static int WorkResultRead(  int panel )
{
	char info[500];
	char path[300]; 
	
	strcpy( path, sysRootPath ); 
	strcat( path, "\\800_External\\Result\\" );   
	strcat( path, fileType );    
	int error=-1; 
	int number=0;
	
	FileNumber = RecurseFileFolder( path,  &ResultfileName);

	int index = sprintf( info, "总共%d个文件", FileNumber);  
	SetCtrlAttribute  ( panel, TABPANEL_5_TEXTMSG, ATTR_VISIBLE, 1 );
	SetCtrlAttribute  ( panel, TABPANEL_5_TEXTMSG, ATTR_CTRL_VAL, info ); 
	
	InfoString   = ( char**) calloc( FileNumber, sizeof(char*)); 
	NumOfInfo    = ( int* )  calloc( FileNumber, sizeof(int));
	SelNumOfInfo = ( int* )  calloc( FileNumber, sizeof(int)); 
	GetNumTableColumns ( panel, TABPANEL_5_TABLE, &numCols);
	GetNumTableRows    ( panel, TABPANEL_5_TABLE, &numRows);
	
	//for( int i=0; i<FileNumber; i++ )
	for( int i=0; i<1; i++ ) 
	{
		number++;
		
		sprintf( &info[index], ",正在读取第%d个文件...", i+1);
		SetCtrlAttribute  ( panel, TABPANEL_5_TEXTMSG, ATTR_CTRL_VAL, info ); 
		
		char filepath[500]; 
		strcpy( filepath, sysRootPath ); 
		strcat( filepath, "\\800_External\\Result\\" ); 
	  strcat( filepath, ResultfileName[i] );    

		FILE *stream; 
		char buffer[1000];
		
		stream = fopen (filepath, "r" );
		while( fgets ( buffer,200, stream ))	///REMOVE THE "\r\n"
		{
			if( 0x0A == buffer[strlen(buffer)-1] ) buffer[strlen(buffer)-1] = 0x00;
			error=-1;
			NumOfInfo[i] += 1;
			TotalNumOfInfo += 1;
			if( numRows<TotalNumOfInfo )
			{
				InsertTableRows ( panel, TABPANEL_5_TABLE, -1, 1, VAL_CELL_STRING ); 
			}
		
			char NumInfo[5];
			sprintf( NumInfo, "%d", TotalNumOfInfo );
			SetTableCellVal      ( panel, TABPANEL_5_TABLE, MakePoint (1, TotalNumOfInfo), NumInfo );  
				
			const char s[2] = ",";
	    char *token;
	    token = strtok(buffer, s);
			NumOfResultItem=1;        
			
	    while( token != NULL && NumOfResultItem <=6 ) 
			{
	      ++NumOfResultItem;
				SetTableCellVal (panel, TABPANEL_5_TABLE, MakePoint (NumOfResultItem, TotalNumOfInfo), token );  
	      token = strtok(NULL, s);
			}
			memset( buffer, NULL, 1000 );
			
			if( 7 == NumOfResultItem ) 
			if( error=WorkResultReadFromDataBase(  panel, TotalNumOfInfo ) ) 
			{
				char MessageText[100]; 
				USEROPERATION_GetErrorText ( error, &MessageText );  
				
				sprintf( &info[index], ",从数据库中读取第%d个雷管数据出错，错误信息：%s ！", NumOfInfo[i], MessageText);
				SetCtrlAttribute  ( panel, TABPANEL_5_TEXTMSG, ATTR_CTRL_VAL, info );
				SetTableCellAttribute ( panel, TABPANEL_5_TABLE, MakePoint (2, TotalNumOfInfo), ATTR_TEXT_BGCOLOR, VAL_RED ); 
			}
			
			if( 0 != error ) break;
		}
		fclose (stream);
		
		if( 0 != error ) break; 
	}
	
	SetTableCellRangeAttribute   ( panel, TABPANEL_5_TABLE, MakeRect(1,numCols,TotalNumOfInfo,1), ATTR_CELL_TYPE, VAL_CELL_RING );	  
	InsertTableCellRangeRingItem ( panel, TABPANEL_5_TABLE, MakeRect(1,numCols,TotalNumOfInfo,1), -1, "Yes");
  InsertTableCellRangeRingItem ( panel, TABPANEL_5_TABLE, MakeRect(1,numCols,TotalNumOfInfo,1), -1, "No" );
	
	SetTableCellRangeAttribute   ( panel, TABPANEL_5_TABLE, VAL_TABLE_ENTIRE_RANGE, ATTR_TEXT_POINT_SIZE, 16);
	SetTableCellRangeAttribute   ( panel, TABPANEL_5_TABLE, VAL_TABLE_ENTIRE_RANGE, ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED );	
	
	if( 0  == error )	sprintf( info, "总共%d个文件，读取完成！", FileNumber);
	else
	{
		//if( -2 == error ) sprintf( info, "总共%d个文件，读取第%d文件数据库发生错误！", FileNumber, number);      
		if( -1 == error )	sprintf( info, "总共%d个文件，读取第%d文件发生错误！", FileNumber, number); 
		SetCtrlAttribute  ( panel, TABPANEL_5_TEXTMSG, ATTR_TEXT_COLOR, VAL_RED );  
	}
	
	SetCtrlAttribute  ( panel, TABPANEL_5_TEXTMSG, ATTR_CTRL_VAL, info ); 

	SetCtrlAttribute  ( panel, TABPANEL_5_INFOBUTTON, ATTR_LABEL_TEXT, "读取作业信息" );
	SetCtrlAttribute  ( panel, TABPANEL_5_INFOBUTTON, ATTR_DIMMED,  0 ); 
	if( TotalNumOfInfo>0 && 0 == error )
	{
		SetCtrlAttribute  ( panel, TABPANEL_5_INFOBUTTON, ATTR_VISIBLE, 0 ); 
		SetCtrlAttribute  ( panel, TABPANEL_5_NEXTBUTTON, ATTR_VISIBLE, 1 );

		GetNumTableColumns ( panel, TABPANEL_5_TABLE, &numCols);
   	GetNumTableRows    ( panel, TABPANEL_5_TABLE, &numRows);
	
		tableInfo  = (char**) calloc((numCols-1)*numRows, sizeof(char*) );
		for( int i=0; i<(numCols-1)*numRows; i++ )
		{
			tableInfo[i] = calloc( 50, sizeof(char));	
		}

		GetTableCellRangeVals ( panel, TABPANEL_5_TABLE, MakeRect(1,1,numRows,numCols-1), tableInfo, VAL_ROW_MAJOR );
	}

	return 0;
}
/**************************************************************************************************************************/ 
int CVICALLBACK InfoFillCtrCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2 )
{
	switch (event)
	{
		case EVENT_COMMIT:
		{
			///////////////////////////////////////////////////////////////////////
			if( TABPANEL_5_INFOBUTTON  == control ) 
			{
				//read local file and get info   
				
				SetCtrlAttribute  ( panel, TABPANEL_5_INFOBUTTON, ATTR_LABEL_TEXT, "正在读取" );  
				SetCtrlAttribute  ( panel, TABPANEL_5_INFOBUTTON, ATTR_DIMMED,              1 );   
				uploadEvent = UL_READ;
				SYSTEM_CreateThread ( UploadThreadID, Thread_WorkResultUpLoad, &uploadEvent);
					
			}
			///////////////////////////////////////////////////////////////////////
			if( TABPANEL_5_CLNBUTTON == control )
			{
				TotalNumOfInfo = 0; 

				if( NumOfInfo ) 
				{	
					for( int i=0; i<FileNumber; i++ )
					{
						NumOfInfo[i]=0;
					}	
				}
				
				SetCtrlAttribute  ( panel, TABPANEL_5_TEXTMSG, ATTR_CTRL_VAL, "等待读取文件" ); 
				SetCtrlAttribute  ( panel, TABPANEL_5_TEXTMSG, ATTR_TEXT_COLOR, VAL_BLACK    );  
				SetCtrlAttribute  ( panel, TABPANEL_5_NEXTBUTTON, ATTR_VISIBLE, 0 );  
				SetCtrlAttribute  ( panel, TABPANEL_5_INFOBUTTON, ATTR_VISIBLE, 1 ); 
				DefaultCtrl			  ( panel, TABPANEL_5_TABLE );	
				SetTableCellRangeAttribute (panel, TABPANEL_5_TABLE, VAL_TABLE_ENTIRE_RANGE, ATTR_TEXT_BGCOLOR, VAL_TRANSPARENT);     
			}
			///////////////////////////////////////////////////////////////////////
			if( TABPANEL_5_NEXTBUTTON == control )
			{
				int currIndex;
				int num = FileNumber*TotalNumOfInfo;
				int error=0; 

				if( FileNumber>0 ) 
				{
					UIDArray     = (char***) calloc( FileNumber, sizeof(char**) );       
				  WorkcodeInfo = (char***) calloc( num, sizeof(char**) );
					SelArray     = (char***) calloc( FileNumber, sizeof(char**) );
						
					for( int i=0; i<FileNumber; i++ )
					{
						GenerateFormatString( panel, &(InfoString[i]), &WorkcodeInfo[i], &UIDArray[i], &SelArray[i], NumOfInfo[i], &SelNumOfInfo[i] ); 
		    	  if( SelNumOfInfo[i] > 0 )
						if( NULL == InfoString[i] ) { error=-1; break;}
					}
					if( 0 ==error )
					{
						GetActiveTabPage  ( WRUPPanelHandle, WRUL_PANEL_TAB, &currIndex );      
						SetCtrlAttribute  ( WRUPPanelHandle, WRUL_PANEL_TAB, ATTR_CTRL_INDEX, ++currIndex );
					}
					else
					{
						MessagePopup      ( "Error", "生成作业信息失败，请重新检查作业记录！"); 	
					}
				}else
				{
					MessagePopup      ( "Warning", "未找到本地文件，请重新检查！"); 	
				}
			}
			
		}break;

		case EVENT_DISCARD:

			break;
	}
	return 0;
}
/**************************************************************************************************************************/     
static int WorkResultUpLoad( int panel, char *paramStr )
{
	char *EncryptStr;  
	char *repsonse; 
	
	int length; 
	int Status=-1;
	int color=VAL_RED;
  char errinfo[50];
	
	if( ( Status=UP_ENCRYT)  && RET_NOERR == WORKINGCODESYS_Encryption( paramStr, KeyStr, &EncryptStr         ))
	if( ( Status=UP_NETWORK) && RET_NOERR == WORKINGCODESYS_HTTPPost  ( UpLoadUrl,EncryptStr, &repsonse, &length ))
	{
			Status=0;    
			cJSON *root, *tmp_obj;
			root	= cJSON_Parse( repsonse );  
			if( tmp_obj = cJSON_GetObjectItem( root, "success" ))   
			if( 0 == strcmp( tmp_obj->valuestring, "fail"	))
			if( tmp_obj = cJSON_GetObjectItem( root, "cwxx" )) 
			{
				StrToInt( tmp_obj->valuestring, &Status );	
			}
			SetCtrlAttribute  ( panel, TABPANEL_7_WRULBUTTON,   ATTR_VISIBLE, 0 ); 
			SetCtrlAttribute  ( panel, TABPANEL_7_NEXTBUTTON,   ATTR_VISIBLE, 1 );
			
			cJSON_Delete( root    ); 
	}
	
	if( EncryptStr ) { free(EncryptStr); EncryptStr= NULL; } 
	if( repsonse   ) { free(repsonse);   repsonse  = NULL; }  
	
	return Status;
}

/**************************************************************************************************************************/ 
 int CVICALLBACK InfoUploadCtrCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		//////////////////////////////////////////////////////////////////////////////////////////// 
		case EVENT_COMMIT:
		{
			if( control == TABPANEL_7_WRULBUTTON )
			{
				uploadEvent = UL_UPLOAD;
				SYSTEM_CreateThread ( UploadThreadID, Thread_WorkResultUpLoad, &uploadEvent);
			}
			
			if( control == TABPANEL_7_NEXTBUTTON )
			{
				int currIndex,tabPanel; 

				GetActiveTabPage          ( WRUPPanelHandle, WRUL_PANEL_TAB, &currIndex );      
				SetCtrlAttribute          ( WRUPPanelHandle, WRUL_PANEL_TAB, ATTR_CTRL_INDEX, ++currIndex );
				GetPanelHandleFromTabPage ( WRUPPanelHandle, WRUL_PANEL_TAB, currIndex, &tabPanel );
				SetTableCellRangeVals     ( tabPanel, TABPANEL_TABLE, MakeRect(1,1,numRows,numCols-1), tableInfo, VAL_ROW_MAJOR ); 
				
				for( int i=0; i<FileNumber; i++ )
				{
					int top = TotalNumOfInfo-NumOfInfo[i]+1; 
					Rect selRange =MakeRect ( top, numCols, NumOfInfo[i], 1);    
					SetTableCellRangeVals   ( tabPanel, TABPANEL_TABLE, selRange, SelArray[i], VAL_ROW_MAJOR ); 
					SetTableCellRangeAttribute ( tabPanel, TABPANEL_TABLE, selRange,  ATTR_TEXT_BGCOLOR, VAL_RED );
					
					for( int j=0; j<NumOfInfo[i]; j++ )
					{
						if( 0 == strcmp( SelArray[i][j], "成功"))     SetTableCellAttribute ( tabPanel, TABPANEL_TABLE, MakePoint(numCols,top+j), ATTR_TEXT_BGCOLOR, VAL_GREEN  ); 
						if( 0 == strcmp( SelArray[i][j], "用户取消")) SetTableCellAttribute ( tabPanel, TABPANEL_TABLE, MakePoint(numCols,top+j), ATTR_TEXT_BGCOLOR, VAL_YELLOW );
					}
				}
				SetTableCellRangeAttribute   ( tabPanel, TABPANEL_TABLE, VAL_TABLE_ENTIRE_RANGE, ATTR_TEXT_POINT_SIZE, 16);
				SetTableCellRangeAttribute   ( tabPanel, TABPANEL_TABLE, VAL_TABLE_ENTIRE_RANGE, ATTR_CELL_JUSTIFY, VAL_CENTER_CENTER_JUSTIFIED );
			}
		}
		break;

	}
	return 0;
}
/**************************************************************************************************************************/ 
int CVICALLBACK UploadDoneCtrCallBack (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		//////////////////////////////////////////////////////////////////////////////////////////// 
		case EVENT_COMMIT:
		{
			CallPanelCallback ( WRUPPanelHandle, EVENT_CLOSE, 0, 0, 0 );    
		}
		break; 
	}
	
	return 0; 
}
