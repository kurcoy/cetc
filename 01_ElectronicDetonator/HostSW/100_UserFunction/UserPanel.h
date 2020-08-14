/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  BST_PANEL                        1       /* callback function: BlastingPanelCallBack */
#define  BST_PANEL_TIMER                  2       /* control type: timer, callback function: BlastingTimerCB */

#define  MAIN_PANEL                       2       /* callback function: MainPanelCallBack */
#define  MAIN_PANEL_TEXTMSG_4             2       /* control type: textMsg, callback function: (none) */
#define  MAIN_PANEL_UL_BUTTON             3       /* control type: command, callback function: InfoUploadCallBack */
#define  MAIN_PANEL_EX_BUTTON             4       /* control type: command, callback function: BlastingCallBack */
#define  MAIN_PANEL_WCDL_BUTTON           5       /* control type: command, callback function: InfoDownloadCallBack */
#define  MAIN_PANEL_TEXTMSG               6       /* control type: textMsg, callback function: (none) */
#define  MAIN_PANEL_PICTURE_6             7       /* control type: picture, callback function: (none) */
#define  MAIN_PANEL_PICTURE_5             8       /* control type: picture, callback function: (none) */

#define  TEST_PANEL                       3       /* callback function: MainPanelCallBack */
#define  TEST_PANEL_TEXTBOX_3             2       /* control type: textBox, callback function: (none) */
#define  TEST_PANEL_TEXTBOX_2             3       /* control type: textBox, callback function: (none) */
#define  TEST_PANEL_TEXTBOX               4       /* control type: textBox, callback function: (none) */
#define  TEST_PANEL_COMMANDBUTTON_4       5       /* control type: command, callback function: (none) */
#define  TEST_PANEL_COMMANDBUTTON_3       6       /* control type: command, callback function: DESDECODECallBack */
#define  TEST_PANEL_TEXTMSG_5             7       /* control type: textMsg, callback function: (none) */
#define  TEST_PANEL_TEXTMSG               8       /* control type: textMsg, callback function: (none) */
#define  TEST_PANEL_TEXTMSG_6             9       /* control type: textMsg, callback function: (none) */

#define  USER_PANEL                       4       /* callback function: UserPanelCallBack */
#define  USER_PANEL_TEXTMSG               2       /* control type: textMsg, callback function: (none) */
#define  USER_PANEL_PSW_STRING            3       /* control type: string, callback function: UserLoginCallBack */
#define  USER_PANEL_TEXTMSG_3             4       /* control type: textMsg, callback function: (none) */
#define  USER_PANEL_NAME_STRING           5       /* control type: string, callback function: UserLoginCallBack */
#define  USER_PANEL_TEXTMSG_2             6       /* control type: textMsg, callback function: (none) */
#define  USER_PANEL_TEXTMSG_4             7       /* control type: textMsg, callback function: (none) */

#define  WCDL_PANEL                       5       /* callback function: WCDownloadPanelCallBack */
#define  WCDL_PANEL_TEXTMSG               2       /* control type: textMsg, callback function: (none) */
#define  WCDL_PANEL_TAB                   3       /* control type: tab, callback function: WCDLTabCtrlCallBack */
#define  WCDL_PANEL_STEP4                 4       /* control type: textButton, callback function: (none) */
#define  WCDL_PANEL_STEP3                 5       /* control type: textButton, callback function: (none) */
#define  WCDL_PANEL_STEP2                 6       /* control type: textButton, callback function: (none) */
#define  WCDL_PANEL_STEP1                 7       /* control type: textButton, callback function: (none) */
#define  WCDL_PANEL_DECORATION            8       /* control type: deco, callback function: (none) */
#define  WCDL_PANEL_PICTURE_6             9       /* control type: picture, callback function: (none) */
#define  WCDL_PANEL_PICTURE_5             10      /* control type: picture, callback function: (none) */
#define  WCDL_PANEL_PICTURE_4             11      /* control type: picture, callback function: (none) */

#define  WRUL_PANEL                       6       /* callback function: InfoPanelCallBack */
#define  WRUL_PANEL_TEXTMSG               2       /* control type: textMsg, callback function: (none) */
#define  WRUL_PANEL_TAB                   3       /* control type: tab, callback function: InfoTabCtrlCallBack */

     /* tab page panel controls */
#define  TABPANEL_TABLE                   2       /* control type: table, callback function: (none) */
#define  TABPANEL_DONEBUTTON              3       /* control type: command, callback function: UploadDoneCtrCallBack */
#define  TABPANEL_TEXTMSG                 4       /* control type: textMsg, callback function: (none) */
#define  TABPANEL_DECORATION              5       /* control type: deco, callback function: (none) */

     /* tab page panel controls */
#define  TABPANEL_0_ONLINEBTN             2       /* control type: radioButton, callback function: SelectionCtrCallBack */
#define  TABPANEL_0_RING                  3       /* control type: ring, callback function: (none) */
#define  TABPANEL_0_OFFLINEBTN            4       /* control type: radioButton, callback function: SelectionCtrCallBack */
#define  TABPANEL_0_NEXTBUTTON            5       /* control type: command, callback function: ComPortGetCtrCallBack */
#define  TABPANEL_0_DECORATION_4          6       /* control type: deco, callback function: (none) */
#define  TABPANEL_0_LOCFILEBTN            7       /* control type: radioButton, callback function: SelectionCtrCallBack */
#define  TABPANEL_0_TEXTMSG_2             8       /* control type: textMsg, callback function: (none) */
#define  TABPANEL_0_TEXTMSG               9       /* control type: textMsg, callback function: (none) */
#define  TABPANEL_0_DECORATION_2          10      /* control type: deco, callback function: (none) */

     /* tab page panel controls */
#define  TABPANEL_1_TABLE                 2       /* control type: table, callback function: (none) */
#define  TABPANEL_1_CLNBUTTON             3       /* control type: command, callback function: UIDGetCtrCallBack */
#define  TABPANEL_1_NEXTBUTTON            4       /* control type: command, callback function: UIDGetCtrCallBack */
#define  TABPANEL_1_UIDBUTTON             5       /* control type: command, callback function: UIDGetCtrCallBack */
#define  TABPANEL_1_TEXTMSG_2             6       /* control type: textMsg, callback function: (none) */
#define  TABPANEL_1_DECORATION_3          7       /* control type: deco, callback function: (none) */
#define  TABPANEL_1_TEXTMSG               8       /* control type: textMsg, callback function: (none) */
#define  TABPANEL_1_DECORATION_2          9       /* control type: deco, callback function: (none) */

     /* tab page panel controls */
#define  TABPANEL_2_TEXTMSG_2             2       /* control type: textMsg, callback function: (none) */
#define  TABPANEL_2_LOCATIONBTN           3       /* control type: command, callback function: LocationGetCtrCallBack */
#define  TABPANEL_2_INFOBUTTON            4       /* control type: command, callback function: InfoGetCtrCallBack */
#define  TABPANEL_2_STRING_5              5       /* control type: string, callback function: (none) */
#define  TABPANEL_2_DECORATION_5          6       /* control type: deco, callback function: (none) */
#define  TABPANEL_2_STRING_6              7       /* control type: string, callback function: (none) */
#define  TABPANEL_2_STRING_3              8       /* control type: string, callback function: (none) */
#define  TABPANEL_2_STRING_4              9       /* control type: string, callback function: (none) */
#define  TABPANEL_2_STRING_7              10      /* control type: string, callback function: (none) */
#define  TABPANEL_2_STRING_2              11      /* control type: string, callback function: (none) */
#define  TABPANEL_2_STRING                12      /* control type: string, callback function: (none) */
#define  TABPANEL_2_NUMERIC               13      /* control type: numeric, callback function: (none) */

     /* tab page panel controls */
#define  TABPANEL_3_TEXTMSG_2             2       /* control type: textMsg, callback function: (none) */
#define  TABPANEL_3_DONEBUTTON            3       /* control type: command, callback function: DownloadDoneCtrCallBack */
#define  TABPANEL_3_WLBUTTON              4       /* control type: command, callback function: WCDownloadCtrCallBack */
#define  TABPANEL_3_PROGRESSBAR           5       /* control type: scale, callback function: (none) */
#define  TABPANEL_3_DECORATION_5          6       /* control type: deco, callback function: (none) */
#define  TABPANEL_3_TEXTBOX               7       /* control type: textBox, callback function: (none) */

     /* tab page panel controls */
#define  TABPANEL_5_TABLE                 2       /* control type: table, callback function: (none) */
#define  TABPANEL_5_CLNBUTTON             3       /* control type: command, callback function: InfoFillCtrCallBack */
#define  TABPANEL_5_NEXTBUTTON            4       /* control type: command, callback function: InfoFillCtrCallBack */
#define  TABPANEL_5_INFOBUTTON            5       /* control type: command, callback function: InfoFillCtrCallBack */
#define  TABPANEL_5_TEXTMSG_2             6       /* control type: textMsg, callback function: (none) */
#define  TABPANEL_5_TEXTMSG               7       /* control type: textMsg, callback function: (none) */
#define  TABPANEL_5_DECORATION            8       /* control type: deco, callback function: (none) */
#define  TABPANEL_5_DECORATION_2          9       /* control type: deco, callback function: (none) */

     /* tab page panel controls */
#define  TABPANEL_7_TEXTMSG_2             2       /* control type: textMsg, callback function: (none) */
#define  TABPANEL_7_NEXTBUTTON            3       /* control type: command, callback function: InfoUploadCtrCallBack */
#define  TABPANEL_7_WRULBUTTON            4       /* control type: command, callback function: InfoUploadCtrCallBack */
#define  TABPANEL_7_PROGRESSBAR           5       /* control type: scale, callback function: (none) */
#define  TABPANEL_7_TEXTBOX               6       /* control type: textBox, callback function: (none) */
#define  TABPANEL_7_DECORATION            7       /* control type: deco, callback function: (none) */

     /* tab page panel controls */
#define  TABPANEL_L_TABLE                 2       /* control type: table, callback function: TableCtrCallBack */
#define  TABPANEL_L_FILBUTTON             3       /* control type: command, callback function: FileBrowserCtrCallBack */
#define  TABPANEL_L_NEXTBUTTON            4       /* control type: command, callback function: FileBrowserCtrCallBack */
#define  TABPANEL_L_TEXTMSG_2             5       /* control type: textMsg, callback function: (none) */
#define  TABPANEL_L_DECORATION_5          6       /* control type: deco, callback function: (none) */


     /* Control Arrays: */

#define  CTRLARRAY                        1
#define  CTRLARRAY_2                      2
#define  CTRLARRAY_3                      3
#define  CTRLARRAY_4                      4

     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK BlastingCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK BlastingPanelCallBack(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK BlastingTimerCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ComPortGetCtrCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DESDECODECallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DownloadDoneCtrCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FileBrowserCtrCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK InfoDownloadCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK InfoFillCtrCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK InfoGetCtrCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK InfoPanelCallBack(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK InfoTabCtrlCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK InfoUploadCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK InfoUploadCtrCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LocationGetCtrCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK MainPanelCallBack(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SelectionCtrCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TableCtrCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UIDGetCtrCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UploadDoneCtrCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UserLoginCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UserPanelCallBack(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK WCDLTabCtrlCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK WCDownloadCtrCallBack(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK WCDownloadPanelCallBack(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
