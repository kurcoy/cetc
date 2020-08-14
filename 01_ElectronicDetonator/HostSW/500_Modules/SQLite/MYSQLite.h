//==============================================================================

#ifndef __MYSQLITE_H__
#define __MYSQLITE_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files
#include "SystemService.h"

//==============================================================================
// Constants
 
//==============================================================================
 
//==============================================================================
// External variables

//==============================================================================
// Global functions

RETVAL SQLITE_Install      ( int  SysModuleID, int panelID,    void *funcData ); 
RETID  SQLITE_OpenDataBase ( char *DataBaseDirectory, char *DataBaseName, char *TableName, char **TableItemList );  

RETVAL SQLITE_Encrypt      ( char *DataBaseName, char Key[50] );  
RETVAL SQLITE_Decrypt      ( char *DataBaseName, char Key[50] ); 
/***************************************************************************************
char *TableItemList[] ={ "TimeStamp", "TTNR", "Result", "ProcessSN  ,"" };
char *DataItemList[500];
int   NumOFData = 5;
      DataItemList[0] = "2019-07-23_11:12:56,3603F63800,Pass,719114115";   
      DataItemList[1] = "2019-07-23_11:12:57,3603F63800,Pass,719114116";    
      DataItemList[2] = "2019-07-23_11:12:58,3603F63800,Pass,719114117";   
      DataItemList[3] = "2019-07-23_11:12:59,3603F63800,Pass,719114118";   
      DataItemList[4] = "2019-07-23_11:13:00,3603F63800,Pass,719114119"; 

int DBTableID; 

if( (DBTableID = SQLITE_OpenDataBase ( "//900_ResultFiles", "sqlite.db", "FinalTest", TableItemList ))>0 )
if( RET_NOERR == SQLITE_InsertIntoDB ( DBTableID, DataItemList, NumOfData, 5 ))
{
  char *RequestTable = "FinalTest"; // can be "*"
  char **RequestResult; 
	int num;
  if( num = SQLITE_SearchRequest ( DBaseName, RequestTable, "ProcessSN", ProcessSN, "Result", &RequestResult )) 
  {
    for( int i=0; i<num; i++ )
		 if( RequestResult[i] ) {free(RequestResult[i]); RequestResult{i} =NULL;}	
		return RET_NOERR;
  }
}
***************************************************************************************/  

RETVAL SQLITE_InsertIntoDB ( int DBTableID, char **DataItemList, int NumOfData, double TimeOut );
RETID  SQLITE_SearchRequest( char *DataBaseName, char *TableName, char *InputColumnName, char *InputColumnValue, char *RequestColumnName, char ***RequestData );

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __MYSQLITE_H__ */
