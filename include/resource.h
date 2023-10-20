#ifndef RESOURCE_H
#define RESOURCE_H



#include "pvi_interface.h"
#include <commctrl.h>

#define VER_PRODUCTVERSION          1,4,0,0
#define VER_PRODUCTVERSION_STR      "1.4.0"

// Used by main.rc
//
#define DLG_SETTINGS                    100
#define DLG_ABOUT                       101
#define DLG_WRITE_DATE_AND_TIME         102
#define DLG_WRITE_NUMBERS               103
#define DLG_LOGGER_CONFIG               104
#define DLG_WRITE_STRINGS               105
#define DLG_EDIT_CPU                    106
#define DLG_EDIT_TASK                   107
#define IDR_DEFAULT_INI1                115

#define IDC_STATIC_PVIVERSION           1002
#define IDS_ZLIB2                       1006
#define IDC_STATIC_TASK                 1007
#define IDC_STATIC_CPU                  1008
#define IDC_STATIC_VERSION              1009
#define IDC_STATIC_VARTYPE              1011
#define IDC_EDIT_YEAR                   1013
#define IDC_EDIT_MONTH                  1014
#define IDC_EDIT_DAY                    1015
#define IDC_EDIT_HOUR                   1016
#define IDC_EDIT_MINUTE                 1017
#define IDC_EDIT_MILLISECONDS           1018
#define IDC_EDIT1                       1022
#define IDC_STATIC_MODULENAME           1023
#define IDC_LIST                        1026
#define IDC_CHECK_CHANGED               1027
#define IDC_STATUS_MESSAGE              1028
#define IDR_BUTTON_WARMSTART            1030
#define IDR_BUTTON_COLDSTART            1031
#define IDR_BUTTON_STOP                 1032
#define IDR_BUTTON_START                1033
#define IDR_BUTTON_DIAGNOSIS            1034
#define IDR_BUTTON_RESUME               1035
#define IDR_BUTTON_DEVICE1				1036
#define IDR_BUTTON_DEVICE2				1037
#define IDR_STATIC_CPU_STATUS           1040
#define IDR_STATIC_TASK_STATUS          1041
#define IDR_EDIT_NUMBER_OF_CYCLES       1042

#define IDR_MNU_MAIN                    2001
#define IDR_LVPVIOBJECTS                4001
#define IDR_LVTASKLIST                  4003
#define IDM_ABOUT                       6001
#define IDM_STATUSPVIOBJEKTE            6002
#define IDM_MNU_REFRESH                 6003
#define IDM_SAVE_WATCH_FILE             6004
#define IDM_LOAD_WATCH_FILE             6005
#define IDM_CHANGE_VIEW_DECIMAL         6101
#define IDM_CHANGE_VIEW_HEX             6102
#define IDM_CHANGE_VIEW_BINARY          6103
#define IDM_CHANGE_VIEW_CHAR            6104
#define IDM_CHANGE_VIEW_OEMCHAR         6105
#define IDM_SEND_ICMP                   6106
#define IDM_DELETE_OBJECT_FROM_LIST		6107
#define IDM_LOGGER_CONFIG               6201
#define IDM_LOGGER_START                6202
#define IDR_ICO_MAIN                    8001
#define IDR_ICO_PVI                     8002
#define IDR_ICO_TASK                    8003
#define IDR_ICO_STRUCT                  8004
#define IDR_ICO_CPU                     8005
#define IDR_ICO_VARIABLE                8006
#define IDR_ICO_GLOBAL                  8007
#define IDR_ICO_ARRAY                   8008
#define IDR_ICO_VARIABLE_U32            8009
#define IDR_ICO_VARIABLE_I32            8010
#define IDR_ICO_VARIABLE_U16            8011
#define IDR_ICO_VARIABLE_I16            8012
#define IDR_ICO_VARIABLE_U8             8013
#define IDR_ICO_VARIABLE_I8             8014
#define IDR_ICO_VARIABLE_BOOL           8015
#define IDR_ICO_VARIABLE_STRING         8016
#define IDR_ICO_VARIABLE_REAL           8017
#define IDR_ICO_VARIABLE_DATE_AND_TIME  8018
#define IDR_ICO_VARIABLE_TIME           8019
#define IDR_ICO_DEVICE                  8020
#define IDR_ICO_PLOT                    8021
#define IDR_ICO_VARIABLE_WSTRING        8022
#define IDR_ICO_VARIABLE_LREAL          8023
#define IDR_STATIC_NAME                 8201
#define IDR_EDIT_VALUE                  8202
#define IDR_EDIT_FILENAME               8203
#define IDR_BUTTON_FILESELECT           8204
#define IDR_EDIT_MAXSIZE                8205
#define IDR_EDIT_TIMESTAMP_CPU          8206
#define IDR_EDIT_CYCLETIME              8207
#define IDR_EDIT_OBJECT_COUNT           8208
#define IDC_CHECK_ZIP                   8209
#define IDR_SETTINGS                    8210
#define IDR_EDIT_IPADDRESS              8211
#define IDR_EDIT_SUBNETMASK             8212
#define IDR_BUTTON_CHANGEIP             8213
#define IDR_CHECK_DHCP_CLIENT           8214
#define IDR_INA_ACTIVATED               8215
#define IDR_MAC_ADDRESS                 8216
#define IDR_INA_SETTINGS                8217

#define IDM_LOGGER_UNZIPLOGGERFILE      40002
#define IDM_OPTIONS_SETTINGS            40004
#define IDM_VIEW_SELECT_FONT            40006
#define IDM_VIEW_SETDEFAULTFONT         40008
#define IDM_QUIT                        40009
#define ID_LOGGER_OPENLOGGERFILE        40012

#define ID_Menu                         40026
#define ID_APPLY                        40031
#define IDC_STATIC                      -1


HIMAGELIST ResourcesCreateImageList(void);
int ResourcesGetImageIndex( WORD resource );
int ResourcesGetPviObjectImage( PVIOBJECT *object );

#endif // RESOURCE_H
