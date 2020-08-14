//==============================================================================
// Include files

#include <utility.h>
#include <ansi_c.h>
#include "cvidef.h"
#include <userint.h> 
#include "system.h"
#include "MYSQLite.h"

#include "sqlite3.h"

//==============================================================================
// Constants
#define MAX_DB_INFO_LENS   500
#define MAX_DB_COLUMN_ITEM 500
#define SQLITE_HAS_CODEC   1
//==============================================================================
// Types
typedef struct
{
  int  OwnID; 
  int  DBID; 
  int  ColumnCount;
  int  lasetRecordID;
  char Name[MAX_DB_INFO_LENS]; 
  //char ColumnItem[MAX_DB_COLUMN_ITEM][MAX_DB_COLUMN_ITEM];
  char ColumnList [MAX_DB_COLUMN_ITEM*MAX_DB_COLUMN_ITEM];
  char ColumnValue[MAX_DB_COLUMN_ITEM*MAX_DB_COLUMN_ITEM]; 
}
SQLTABLE;

typedef struct
{
  int  OwnID;
  char Name[MAX_DB_INFO_LENS];
  int  ErrorLedID;
  int  Encrypted;
  SQLTABLE *TableList;
    
  sqlite3 *pDb;
}
DATABASE;

//==============================================================================
// Static global variables
static DATABASE  *SQLDB;
static SQLTABLE *DBTable; 
static sqlite3 *ppDb;  

static int NumofDataBase;
static int NumofDBTable;

static int ModuleID = -1;

//TODO: 
//MUTEX INDICATOR

//==============================================================================
// Static functions


//==============================================================================
// Global variables

//==============================================================================
// Global functions
typedef enum
{
  ERR_INST_MODULE,
  ERR_NAMEPARA,
	ERR_DBOPEN,
  ERR_SQL,
	ERR_ENCRYPT,
  ERR_DBID,
  ERR_PARAMS,
  ERR_COLUMNITEM,
  ERR_CHECKTABLE,
  ERR_NOTABLE, 
  ERR_TRANS_BEGIN,
  
}
ERROR_NRS;

static RETIDX ErrorListing ( MSGTYPE Type, int numError, char *addInfoText )                                           
{                                                                                                                          
  char *MessageText = "End of List";                                                                                       
                                                                                                                         
  switch( numError )                                                                                                       
  {                                                                                                                        
    case ERR_INST_MODULE:           MessageText = "Installation failed";                                  break;
    case ERR_NAMEPARA:              MessageText = "Invalid name parameter";                               break;
		case ERR_DBOPEN:                MessageText = "DataBase Not Open Yet";                                break;    
    case ERR_DBID:                  MessageText = "Invalid DataBase ID";                                  break; 
    case ERR_PARAMS:                MessageText = "Invalid Input Param";                                  break; 
    case ERR_COLUMNITEM:            MessageText = "Invalid Column Item Numbers";                          break; 
    case ERR_CHECKTABLE:            MessageText = "Exsited DataBase Table info not same with input";      break;  
    case ERR_NOTABLE:               MessageText = "Table is not existed in database";                     break; 
    case ERR_TRANS_BEGIN:           MessageText = "The database file is locked and timeout";              break;
    case ERR_SQL:                   MessageText = "SQL Operation is failed";                              break;  
    
    default:                        numError    = -1;                                                     break;                                      
  }                                                                                                                        
	char info[200]; 
	strcpy( info, addInfoText ); 
	strcat( info, "_" );      
	strcat( info, MessageText);
	
  SYSTEM_GenerateMessage ( Type, ModuleID, numError, info );                                                                                                             
  return numError;                                                                                                         
}

//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************

static RETVAL UnInstall ( int sysCompID, int panelID, void *funcData )  
{

  ModuleID = -1;
  
  for( int i=0; i<NumofDataBase; i++ )
  { 
    DATABASE *dBase = &SQLDB[i];
    if( dBase->pDb )  { sqlite3_close( dBase->pDb ); dBase->pDb = 0;}
  } 
  
  if( DBTable) { free(DBTable); DBTable = 0; NumofDBTable  = 0;} 
  if( SQLDB)   { free(SQLDB);   SQLDB = 0;   NumofDataBase = 0;}

  if( ppDb ) ppDb = 0; 
  
  return RET_NOERR;
}

//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************
RETVAL SQLITE_Install ( int  SysModuleID, int panelID, void *funcData )
{
  char **DeviceNameList = funcData;
  //char ErrorLedLabel[32];                                                                                                        
  /////////////////////////////////////////////////////////////////////////////////////////////////      
  // system integration                                                                                  
  /////////////////////////////////////////////////////////////////////////////////////////////////      
  ModuleID = SysModuleID;                                                                                
                                                                                                         
  SYSTEM_SetSysModuleUninstFunc  ( SysModuleID, UnInstall, 0 );                                          
  SYSTEM_SetSysModuleErrListFunc ( SysModuleID, ErrorListing );                                          
                                                                                                         
  if( DeviceNameList!=0 && DeviceNameList[0]!=0 )
  {
    while( *DeviceNameList[NumofDataBase] )   
    {                                          
      NumofDataBase++;
    }
    if ( SQLDB = calloc     ( NumofDataBase, sizeof( DATABASE ) ))   
    for( int i=0; i < NumofDataBase; i++ )
    {
      DATABASE *dBase = &SQLDB[i]; 
      dBase->OwnID    = i+1;

      strcpy( dBase->Name, DeviceNameList[i] );
    }
    return RET_NOERR; 
  }
    
  ErrorListing ( MSG_ERROR, ERR_INST_MODULE, " " );          
  return RET_ERROR;                                           
}
//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************
static RETVAL DDNPSQLITE_GetDataBaseByName( char *DataBaseName, DATABASE **pDataBase )
{
  for( int i=0; i < NumofDataBase; i++ )
  {
    DATABASE *dBase = &SQLDB[i]; 
    if( 0 == strcmp( dBase->Name, DataBaseName ) ) 
    {
      *pDataBase = dBase;
      return RET_NOERR;
    }
  }
  
  ErrorListing ( MSG_ERROR, -1, DataBaseName );
  return RET_ERROR;
}

//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************
static RETVAL DDNPSQLITE_CheckTableInfo( int DBTableID, char TableItemList[500][500], int NumOfColumn)  //UpdateTableInfo GetLatestRecordIDAndColumnCount
{
  SQLTABLE *dTable;
  sqlite3 *ppDb;
  int error, index=0, number=0;
  char info[200]= "CheckTableInfo_";  
  
  if ((error=ERR_NAMEPARA) && NumOfColumn > 0 && *TableItemList )
  if ((error=ERR_DBID)     && DBTableID > 0 && DBTableID <= NumofDBTable && ( dTable = &DBTable[DBTableID-1] ))
  {
    DATABASE *dBase=&SQLDB[dTable->DBID - 1];
    ppDb = dBase->pDb; 
    
    sqlite3_stmt *stmt;
    const char *zTail;
    
    char sql_search[1200]     = "SELECT * FROM ";
    char sql_order[50]        = " ORDER BY id DESC LIMIT 0, 1"; //last record, desc and only one returned   
    
    strcat( sql_search,  dTable->Name ); 
    strcat( sql_search,  sql_order );
    
    error = ERR_SQL;  
    if( SQLITE_OK   == sqlite3_prepare  ( ppDb, sql_search, -1, &stmt, &zTail ))
    {
      if( SQLITE_ERROR != sqlite3_step      ( stmt ))
      { 
        dTable->lasetRecordID = sqlite3_column_int( stmt, 0 );
        dTable->ColumnCount   = sqlite3_column_count(stmt);
        
        for( int i=0; i<dTable->ColumnCount; i++ )
        { 
          //strcpy( dTable->ColumnItem[i], sqlite3_column_name( stmt, i ));
        
          char ColumnName[50];
          strcpy( ColumnName, sqlite3_column_name( stmt, i ));

          index  += sprintf( &dTable->ColumnList[index],   "%s, ", ColumnName ); 
          number += sprintf( &dTable->ColumnValue[number], "%s, ", "?"  ); 
          
          if( i>0 )
          if( 0 != strncmp( TableItemList[i-1], ColumnName, strlen(ColumnName) )) { error=ERR_CHECKTABLE; break; } 
          error = 0;
        } 
        
        sprintf( &dTable->ColumnList [index-2],  "%s", "" ); 
        sprintf( &dTable->ColumnValue[number-2], "%s", "" );
      }

      if( SQLITE_OK   == sqlite3_finalize ( stmt ))
      {
        /*
        //error = ERR_CHECKTABLE; 
        if( NumOfColumn == (dTable->ColumnCount - 1) && !(error=0))
        for( int i=1; i<dTable->ColumnCount; i++ )
        {
          if( 0 != strncmp( TableItemList[i-1], dTable->ColumnItem[i], strlen(dTable->ColumnItem[i]) )) { error=ERR_CHECKTABLE; break; }
        }
        */
        //#define SQLITE_THREADSAFE  1
        //int safe = sqlite3_threadsafe   (      );
        //int read = sqlite3_stmt_readonly( stmt );
        //int busy = sqlite3_stmt_busy    ( stmt );
        
        //char *errmsg;
        //int result = sqlite3_exec( ppDb, "BEGIN IMMEDIATE;", 0, 0, &errmsg );
        
        if(!error) return RET_NOERR;
      }
    }
    
    strcat( info, sqlite3_errmsg( ppDb ));  
  }
  
  ErrorListing ( MSG_ERROR, error, info );          
  return RET_ERROR;
  
}

//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************
RETID SQLITE_OpenDataBase( char *DataBaseDirectory, char *DataBaseName, char *TableName, char **TableItemList )
{
  int error;
  int NumOfItem=0;   
  char dbName[500];   
  char info[200]= "OpenDataBase_"; 
  
  DATABASE *dBase;
  sqlite3 *ppDb; 
  
  if( (error=ERR_NAMEPARA) && NULL != *DataBaseName && NULL != *TableName && NULL != TableItemList)
  if( RET_NOERR == DDNPSQLITE_GetDataBaseByName( DataBaseName, &dBase ))
  {
    sqlite3_stmt *stmt;
    const char *zTail; 

    strcpy( dbName, DataBaseDirectory );
		strcat( dbName, "\\" ); 
    strcat( dbName, DataBaseName ); 
    strcat( dbName, ".db");
    
    error = ERR_SQL; 
 
		if( dBase->pDb ) { ppDb = dBase->pDb; error=0;}
		else if( SQLITE_OK == sqlite3_open  ( dbName, &ppDb ))
		{
			error=0;	
		}
		
		if( 0 == error )
    {
      //int a = sqlite3_key( ppDb, "cetcoil", 7 );
			//int b = sqlite3_rekey( ppDb, NULL, 0 );
			char ColumnItem[MAX_DB_COLUMN_ITEM][MAX_DB_COLUMN_ITEM];
      char sql_selectTable[100]  = "SELECT COUNT(*) FROM ";
      char sql_createTable[500] = "CREATE TABLE ";
      char tableItem[500];
      int  index=0;  

      index = sprintf( tableItem, "%s", "ID INTEGER, ");
    
      while( (*TableItemList[NumOfItem]) )   
      {                                          
        strcpy( ColumnItem[NumOfItem],TableItemList[NumOfItem]);  
        index += sprintf( &tableItem[index], "%s", TableItemList[NumOfItem]);
        index += sprintf( &tableItem[index], "%s", " TEXT" );
        index += sprintf( &tableItem[index], "%s", ", "    );  
        NumOfItem++;
      }
      
      //index += sprintf( &tableItem[index-1], "%s", " PRIMARY KEY" ); 
      strcpy( &tableItem[index-2], " PRIMARY KEY"); 
      if((error=ERR_COLUMNITEM) && NumOfItem <= MAX_DB_COLUMN_ITEM )
      { 
        strcat( sql_selectTable,  TableName );
        strcat( sql_createTable, TableName );
        strcat( sql_createTable, " ( " );
        strcat( sql_createTable, tableItem );
        strcat( sql_createTable, " );");
      
        //"SELECT COUNT(*) FROM package"
        //"CREATE TABLE package ( TimeStamp TEXT, TTNR TEXT, CustomerSN TEXT, ProcessSN TEXT PRIMARY KEY);"
        error = ERR_SQL;
        int status = sqlite3_prepare ( ppDb, sql_selectTable, -1, &stmt, &zTail );
        if( SQLITE_OK   == sqlite3_finalize ( stmt )) 
        if( !(error=0) && status != SQLITE_OK )
        if( (error=ERR_SQL) && SQLITE_OK == sqlite3_prepare   ( ppDb, sql_createTable, -1, &stmt, &zTail )) 
        if( SQLITE_DONE == sqlite3_step     ( stmt ))
        if( SQLITE_OK   == sqlite3_finalize ( stmt ))
        {  
          error=0; 
        }
      
        if(!error)
        {
          NumofDBTable++; 
          SQLTABLE *dTable; 

          error=-1;
          if( DBTable = realloc ( DBTable, NumofDBTable*sizeof(SQLTABLE) ))  
          if( memset( (dTable=&DBTable[NumofDBTable-1]), 0, sizeof(SQLTABLE) ))
          { 
            dBase->pDb        = ppDb;
            dTable->OwnID     = NumofDBTable;
            dTable->DBID      = dBase->OwnID;
            strcpy( dTable->Name, TableName );
          
            if( RET_NOERR == DDNPSQLITE_CheckTableInfo( NumofDBTable, ColumnItem, NumOfItem ))//GET COLUMN ITEM, THEN CMP WITH INPUT IN HERE  
            {
              return dTable->OwnID;   
            }
          }
        }
      }
      
    }
  }

  if( error == ERR_SQL )  strcat( info, sqlite3_errmsg( ppDb )); 
 
  ErrorListing ( MSG_ERROR, error, info );          
  return -1;                                           
}
//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************
RETVAL  SQLITE_Encrypt( char *DataBaseName, char Key[50] )
{
  int error;
  char info[200]= "Encrypt_"; 
  char dbName[500];  
  DATABASE *dBase;
  sqlite3  *ppDb;  
 
  if( (error=ERR_NAMEPARA) && NULL != *DataBaseName && NULL != Key )
  if( RET_NOERR == DDNPSQLITE_GetDataBaseByName( DataBaseName, &dBase ))
	{
		strcpy( dbName, DataBaseName ); 
    strcat( dbName, ".db");
		char oldKey[20], newKey[20]; 
		
		if( -1 == dBase->Encrypted )
		{
			strcpy( oldKey, "");
			strcpy( newKey, Key  ); 
		}else
		{
			strcpy( oldKey, Key); 
			strcpy( newKey, Key); 	
		}
		if( error=ERR_SQL)  
		if( SQLITE_OK == sqlite3_open  ( dbName, &ppDb )) 	
		if( NULL != ppDb )
	  if( SQLITE_OK == sqlite3_key  ( ppDb, oldKey, strlen(oldKey) ))		 
		if( error=ERR_ENCRYPT)   
		if( SQLITE_OK == sqlite3_rekey( ppDb, newKey, strlen(newKey) ))      	
		{
			dBase->pDb       = ppDb;
			dBase->Encrypted = 1;
			return RET_NOERR;
		}
	}
	if( error == ERR_ENCRYPT )	 return RET_NOERR;
	if( error == ERR_SQL  )  
	if( ppDb )				strcat( info, sqlite3_errmsg( ppDb )); 
 
  ErrorListing ( MSG_ERROR, error, info );          
  return RET_ERROR;   
}
//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************
RETVAL SQLITE_Decrypt      ( char *DataBaseName, char Key[50] )
{
  int error;
  char info[200]= "Decrypt_"; 
  char dbName[500];
  DATABASE *dBase;
  sqlite3 *ppDb; 
	
  if( (error=ERR_NAMEPARA) && NULL != *DataBaseName && NULL != Key )
  if( RET_NOERR == DDNPSQLITE_GetDataBaseByName( DataBaseName, &dBase ))
	{
		strcpy( dbName, DataBaseName ); 
    strcat( dbName, ".db");
		char oldKey[20], newKey[20];
		
		if( -1 == dBase->Encrypted )
		{
			strcpy( oldKey, NULL); 
			strcpy( newKey, Key ); 
		}else
		{
			strcpy( oldKey, Key); 
			strcpy( newKey, Key); 	
		}
		
		if( SQLITE_OK == sqlite3_open  ( dbName, &ppDb )) 	
		if( (error=ERR_SQL) && NULL != ppDb ) 	
	  if( SQLITE_OK == sqlite3_key  ( ppDb, Key, strlen(Key) ))
		if( SQLITE_OK == sqlite3_rekey( ppDb, NULL, 0 ))
		{
			dBase->pDb = ppDb; 
			dBase->Encrypted = -1; 
			return RET_NOERR;
		}
	}
	if( error == ERR_SQL )
	if( ppDb )				strcat( info, sqlite3_errmsg( ppDb )); 
 
  ErrorListing ( MSG_ERROR, error, info );          
  return RET_ERROR;   
}
//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************
RETVAL SQLITE_InsertIntoDB   ( int DBTableID, char **DataItemList, int NumOfData, double TimeOut ) 
{
  SQLTABLE *dTable;
  sqlite3 *ppDb;
  int error;
  char info[200]= "InsertIntoDB_"; 
  
  if( (error=ERR_PARAMS) && *DataItemList && NumOfData >0 )
  if ((error=ERR_DBID)   && DBTableID > 0 && DBTableID <= NumofDBTable && ( dTable = &DBTable[DBTableID-1] ))
  {
    DATABASE *dBase=&SQLDB[dTable->DBID - 1];
    
    error = 0;
    ppDb = dBase->pDb; 
    sqlite3_stmt *stmt;
    const char   *zTail;
    int          index=0;
    int          NumOfItem=0;
    //char         Values[2000][100]; 
    char         *errmsg; 
    char sql_insert[1200] = "INSERT OR REPLACE INTO ";
     		
		char **Values = (char**)calloc( NumOfData*(dTable->ColumnCount -1), sizeof(char*) ); 
		for( int i=0; i<NumOfData*(dTable->ColumnCount -1); i++ )
    {
			Values[i] = (char*) calloc( MAX_DB_INFO_LENS, sizeof(char) );
		}
    ///////////////////////////////////
    for( int i=0; i<NumOfData; i++ )
    {
/*    
/////////////////////////////////////////////////////////////////////// 
      int j=0;   
      char *pItem = strtok( DataItemList[i], ", " );
      while( NULL != pItem && strcmp(pItem, "\n"))
      {
        strcpy(Values[NumOfItem+j++], pItem );
        pItem = strtok(NULL,", " );
        
      }
///////////////////////////////////////////////////////////////////////
*/
///////////////////////////////////////////////////////////////////////
      if(error) break;
			int j=0;
      char *p = DataItemList[i];
      char *b = p;
      int  d=0;
      while( *p != 0 && *p != 10 && d<100)            // decimal 10 = "\n"
      { 
        while( *p == ' ' || *p == ',' ) *p++; //remove the space and delimiter

        d=0; b=p; 
        while( *p != ',' && *p != 0 &&  d<100)         //extract the data
        {
          d++;
          *p++;
        }
        
        if( *b != ',' && *b != 0 && *b != 10 ) strncpy(Values[NumOfItem+j++], b, d );
      }
      
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
      
///////////////////////////////////////////////////////////////////////   
      if( j == dTable->ColumnCount -1)
      { 
        NumOfItem += j;  
        continue;
      }
      error = ERR_COLUMNITEM;  
    }
    ///////////////////////////////////
    
    if( !error )
    {
      //char sql_insert[100] = "INSERT INTO package (number, nick) VALUES (?, ?, ?, ?)"; 
      strcat( sql_insert,  dTable->Name );
      strcat( sql_insert,  " ( " );
      strcat( sql_insert,  dTable->ColumnList ); 
      strcat( sql_insert,  " ) " ); 
      strcat( sql_insert,  " VALUES( " );
      strcat( sql_insert, dTable->ColumnValue );
      strcat( sql_insert, " );");
      
      error=ERR_TRANS_BEGIN; 
  
      double t = Timer();
      if( SQLITE_OK   == sqlite3_prepare  ( ppDb, sql_insert, -1, &stmt, &zTail ))    
      while( Timer() < t + TimeOut && RET_NOERR == SYSTEM_SleepTime( 1 ) )
      {
        if( SQLITE_OK == sqlite3_exec( ppDb, "BEGIN IMMEDIATE;", 0, 0, &errmsg ))
        {
          error=0; break;  
        } 
      }

      if(!error) 
      {
        index = dTable->lasetRecordID;;   
        for( int i=0; i<NumOfData; i++ ) 
        {
          char text[50]; 
          sprintf( text, "%d", ++index );
          if( (error = ERR_SQL) && SQLITE_OK == sqlite3_bind_text(stmt,1,text,-1,SQLITE_STATIC))
          {
            error=0;
            for( int j=0; j<dTable->ColumnCount-1; j++ )
            {
              if( SQLITE_OK != sqlite3_bind_text(stmt,j+2,Values[i*(dTable->ColumnCount-1)+j],-1,SQLITE_STATIC )) 
              { 
                error = ERR_SQL;break; 
              }
            }
          }
          
          if( !error ) 
          if( SQLITE_DONE == sqlite3_step     ( stmt ))
          if( SQLITE_OK   == sqlite3_reset    ( stmt )) 
          {
            error=0;
            continue;
          } 
          error = ERR_SQL; 
        }
        
        if( !error )
        if( (error = ERR_SQL)&& SQLITE_OK == sqlite3_finalize ( stmt )) 
        if( SQLITE_OK   == sqlite3_exec     ( ppDb, "COMMIT;", 0, 0, &errmsg ))   
        {
          dTable->lasetRecordID += NumOfData;
          //memset(Values, 0x00 ,sizeof(char) * (2000*100));
          //return RET_NOERR;
					error=0; 
        }
      }
    }

	 if(Values)
	 {
		 for( int i=0; i<NumOfData*(dTable->ColumnCount -1); i++ )
		 {
			 if(Values[i]) { free(Values[i]), Values[i] = NULL; }
		 }
		 free(Values); Values =NULL;
	 }
		
	}
	
	if( !error ) return RET_NOERR; 
	
	strcat( info, sqlite3_errmsg( ppDb ));
	sqlite3_exec ( ppDb, "ROLLBACK;", 0, 0, 0 );

 	ErrorListing ( MSG_ERROR, error, info );          
  return RET_ERROR; 

}

//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************
/*
RETVAL SQLITE_InsertIntoDB   ( int DBTableID, char **DataItemList, double TimeOut ) 
{
  DONPTABLE *dTable;
  sqlite3 *ppDb;
  int error;
  char info[200]= "InsertIntoDB_"; 
  
  if( (error=ERR_PARAMS) && *DataItemList )
  if ((error=ERR_DBID)   && DBTableID > 0 && DBTableID <= NumofDBTable && ( dTable = &DBTable[DBTableID-1] ))
  {
    DATABASE *dBase=&DonPDB[dTable->DBID - 1];
    
    ppDb = dBase->pDb; 
    sqlite3_stmt *stmt;
    const char   *zTail;
    int          index=0;
    int          NumOfItem=0; 
    char         Values[500]; 
    char         *errmsg; 
    
    ///////////////////////////////////
    index = sprintf( Values, "%d", dTable->lasetRecordID+1 );
    while( *DataItemList[NumOfItem] )   
    {                                          
      index += sprintf( &Values[index], "%s", ", " );
      index += sprintf( &Values[index], "%s", "'" );
      index += sprintf( &Values[index], "%s", DataItemList[NumOfItem]);
      index += sprintf( &Values[index], "%s", "'" ); 
      NumOfItem++;
    }
  
    error = ERR_COLUMNITEM;
    if( NumOfItem == dTable->ColumnCount -1)
    { 
    ///////////////////////////////////
    
      error=ERR_TRANS_BEGIN; 
    
      double t = Timer();  
      while( Timer() < t + TimeOut && RET_NOERR == SYSTEM_SleepTime( 1 ) )
      {
        if( SQLITE_OK == sqlite3_exec( ppDb, "BEGIN IMMEDIATE;", 0, 0, &errmsg ))
        {
          error=0; break;  
        } 
      }
    
      if(!error) 
      {
        char sql_insert[1200] = "INSERT OR REPLACE INTO "; 
  

          strcat( sql_insert,  dTable->Name );
          strcat( sql_insert,  " VALUES( " );
          strcat( sql_insert, Values );
          strcat( sql_insert, " );"); 
    
          //"INSERT INTO package VALUES( 4, '17:30', '360KM', '123', '456' );"
          error = ERR_SQL;

          if( SQLITE_OK   == sqlite3_prepare  ( ppDb, sql_insert, -1, &stmt, &zTail ))
          if( SQLITE_DONE == sqlite3_step     ( stmt ))
          if( SQLITE_OK   == sqlite3_finalize ( stmt ))
          if( SQLITE_OK   == sqlite3_exec     ( ppDb, "COMMIT;", 0, 0, &errmsg ))   
          {
            dTable->lasetRecordID += 1;
            return RET_NOERR;
          }
        }
 
      strcat( info, sqlite3_errmsg( ppDb ));
      sqlite3_exec ( ppDb, "ROLLBACK;", 0, 0, 0 );
    }
  }
  
  SQLITE_ProcessError( -1, ERR_BLNK, error, info );          
  return RET_ERROR; 
} */
//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************
/*
RETVAL SQLITE_InsertIntoDB   ( int DBTableID, char **DataItemList, double TimeOut ) 
{
  DONPTABLE *dTable;
  sqlite3 *ppDb;
  int error;
  
  if( (error=ERR_PARAMS) && *DataItemList )
  if ((error=ERR_DBID)   && DBTableID > 0 && DBTableID <= NumofDBTable && ( dTable = &DBTable[DBTableID-1] ))
  {
    DATABASE *dBase=&DonPDB[dTable->DBID - 1];
    
    ppDb = dBase->pDb; 
    sqlite3_stmt *stmt;
    const char   *zTail;
    int          index=0;
    int          NumOfItem=0; 
    char         Values[500]; 
    char         *errmsg; 
    
    error=ERR_TRANS_BEGIN; 
    
    double t = Timer();  
    while( Timer() < t + TimeOut && RET_NOERR == SYSTEM_SleepTime( 1 ) )
    {
      if( SQLITE_OK == sqlite3_exec( ppDb, "BEGIN IMMEDIATE;", 0, 0, &errmsg ))
      {
        error=0; break;  
      } 
    }
    
    if(!error) 
    {
      char sql_insert[1200] = "INSERT OR REPLACE INTO "; 
    
      index = sprintf( Values, "%d", dTable->lasetRecordID+1 );
      while( *DataItemList[NumOfItem] )   
      {                                          
        index += sprintf( &Values[index], "%s", ", " );
        index += sprintf( &Values[index], "%s", "'" );
        index += sprintf( &Values[index], "%s", DataItemList[NumOfItem]);
        index += sprintf( &Values[index], "%s", "'" ); 
        NumOfItem++;
      }
    
      error = ERR_COLUMNITEM;
      if( NumOfItem == dTable->ColumnCount -1)
      {

        strcat( sql_insert,  dTable->Name );
        strcat( sql_insert,  " VALUES( " );
        strcat( sql_insert, Values );
        strcat( sql_insert, " );"); 
    
        //"INSERT INTO package VALUES( 4, '17:30', '360KM', '123', '456' );"
        error = ERR_SQL;
                                                                            
        //char *errmsg;
        //int result = sqlite3_exec( ppDb, "BEGIN IMMEDIATE;", 0, 0, &errmsg ); 
      
        if( SQLITE_OK   == sqlite3_prepare  ( ppDb, sql_insert, -1, &stmt, &zTail ))
        if( SQLITE_DONE == sqlite3_step     ( stmt ))
        if( SQLITE_OK   == sqlite3_finalize ( stmt ))
        if( SQLITE_OK   == sqlite3_exec     ( ppDb, "COMMIT;", 0, 0, &errmsg ))   
        {
          dTable->lasetRecordID += 1;
          return RET_NOERR;
        }
      }
      
    }
  }
  
  char info[200]= "InsertIntoDB_";   
  if( error == ERR_SQL         )  strcat( info, sqlite3_errmsg( ppDb ));
  if( error == ERR_TRANS_BEGIN )  sqlite3_exec ( ppDb, "ROLLBACK;", 0, 0, 0 );
  
  SQLITE_ProcessError( -1, ERR_BLNK, error, info );          
  return RET_ERROR; 
  
}
*/
//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************
RETVAL SQLITE_ReplaceIntoDB   ( int DBTableID, char **DataItemList ) 
{
  SQLTABLE *dTable;
  sqlite3 *ppDb;
  int error;
  
  if( (error=ERR_PARAMS) && *DataItemList )
  if ((error=ERR_DBID)   && DBTableID > 0 && DBTableID <= NumofDBTable && ( dTable = &DBTable[DBTableID-1] ))
  {
    DATABASE *dBase=&SQLDB[dTable->DBID - 1];
    
    ppDb = dBase->pDb; 
    sqlite3_stmt *stmt;
    const char   *zTail; 
    int          index=0;
    int          NumOfItem=0; 
    char         Values[500]; 

    char sql_insert[1200] = "REPLACE INTO "; 
    
    index = sprintf( Values, "%d", dTable->lasetRecordID+1 );
    while( *DataItemList[NumOfItem] )   
    {                                          
      index += sprintf( &Values[index], "%s", ", " );
      index += sprintf( &Values[index], "%s", "'" );
      index += sprintf( &Values[index], "%s", DataItemList[NumOfItem]);
      index += sprintf( &Values[index], "%s", "'" ); 
      NumOfItem++;
    }
    
    if( NumOfItem == dTable->ColumnCount -1)
    {

      strcat( sql_insert,  dTable->Name );
      strcat( sql_insert,  " VALUES( " );
      strcat( sql_insert, Values );
      strcat( sql_insert, " );"); 
    
      //"INSERT INTO package VALUES( 4, '17:30', '360KM', '123', '456' );"
      error = ERR_SQL;
      if( SQLITE_OK   == sqlite3_prepare  ( ppDb, sql_insert, -1, &stmt, &zTail ))
      if( SQLITE_DONE == sqlite3_step     ( stmt ))
      if( SQLITE_OK   == sqlite3_finalize ( stmt ))
      {
        dTable->lasetRecordID += 1;
        return RET_NOERR;
      }
    }
  }
  
  char info[200]= "InsertIntoDB_";   
  if( error == ERR_SQL )  strcat( info, sqlite3_errmsg( ppDb )); 
  
  ErrorListing ( MSG_ERROR, error, info );          
  return RET_ERROR; 
  
}

//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************
RETID SQLITE_SearchRequest   ( char *DataBaseName, char* TableName, char *InputColumnName, char *InputColumnValue, char *RequestColumnName, char ***RequestData ) 
{
  DATABASE *dBase;

  int error;
  char info[200]= "SearchRequest_"; 
    
  //if( (error=ERR_PARAMS) && InputColumnID > 0 && RequestColumnID >0 && *InputColumnValue && *RequestData )
  //  if( (error=ERR_PARAMS) && InputColumnID < dTable->ColumnCount && RequestColumnID < dTable->ColumnCount )
  if( (error=ERR_NAMEPARA) && NULL != DataBaseName && NULL != *TableName && NULL != *InputColumnName && NULL != *InputColumnValue ) 
  if( NULL != *RequestColumnName && NULL != RequestData )
    
  if( RET_NOERR == DDNPSQLITE_GetDataBaseByName( DataBaseName, &dBase ))  
  {
    sqlite3 *ppDb;
    sqlite3_stmt *stmt; 
  
    const char   *zTail; 
    char *pzErrmsg;
    char **pazResult;
    int pnRow, pnColumn;
    
    char sql_select[200] = "SELECT * FROM ";
    char sql_condition[100]; 
    
    ppDb = dBase->pDb;  
    
    char sql_table[1200]      = "SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' and name = '";   
    strcat( sql_table, TableName );
    strcat( sql_table, "'" ); 
    
    if( (error=ERR_SQL) && SQLITE_OK     == sqlite3_prepare ( ppDb, sql_table, -1, &stmt, &zTail )) 
    if(                    SQLITE_ERROR != sqlite3_step ( stmt    ))  
    if( (error=ERR_NOTABLE) && sqlite3_column_double ( stmt, 0 ))
    if( (error=ERR_SQL)     && SQLITE_OK == sqlite3_finalize ( stmt ))
    {   
      sprintf( sql_select, " SELECT %s FROM %s ", RequestColumnName, TableName    );
      sprintf( sql_condition, "WHERE %s = '%s'", InputColumnName, InputColumnValue );
      
      strcat ( sql_select, sql_condition );
    
      if( SQLITE_OK == sqlite3_get_table ( ppDb, sql_select, &pazResult, &pnRow, &pnColumn, &pzErrmsg ))
      {
        int index = pnColumn, length; 
        if( pnRow>0 )
				{
					char **result = (char**) calloc( pnRow, sizeof(char*) );
					*RequestData = result;

	        for( int i=0; i<pnRow; i++ ) //pnRow for data quantity
	        {
	          length=0;
						result[i] = calloc( MAX_DB_INFO_LENS, sizeof(char) ); 
	          for( int j=0; j<pnColumn; j++ )	 //pnColumn for data item
	          {
							if( j>0) length += sprintf( &result[i][length], "%s", "," );
							length += sprintf( &result[i][length], "%s", pazResult[index] ); 	 //pazResult[j], 
	            index++;
	          }
	        }
				}
        sqlite3_free_table( pazResult); 
        return pnRow;
      }
    }
  
   strcat( info, sqlite3_errmsg( ppDb ));
  }

  ErrorListing ( MSG_ERROR, error, info );          
  return -1;   
    
}
//*****************************************************************************************************************************************  
//*****************************************************************************************************************************************

