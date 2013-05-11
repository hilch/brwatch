
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Project:  Process Visualization Interface
*  Part:     PVI Application
*  File:     PviCom.h
*  Type:     MSVC++ (32 Bit)
*------------------------------------------------------------------------------
*  Function: Include file for PVI applications
******************************************************************************/

#ifndef _INC_PVICOM
#define _INC_PVICOM

#ifndef _WINDOWS_        
#include <windows.h>
#endif              

#ifdef	__cplusplus
#define PVICM_EXPORT extern "C"
#else
#define PVICM_EXPORT extern
#endif              
	

//=============================================================================
// PVI Communication Defines
//=============================================================================

#define PVIBASE_POBJNAME	"Pvi"	// name of the PVI base object
#define MAXLEN_POBJ_NAME	1024	// max length of process object name

typedef enum {						// PVI process object types:
	POBJ_PVI,							// global
	POBJ_LINE,							// line
	POBJ_DEVICE,						// device
	POBJ_STATION,						// station (PLC)
	POBJ_CPU,							// CPU
	POBJ_MODULE,						// modul
	POBJ_TASK,							// task
	POBJ_PVAR,							// process variable
	POBJ_TYPE_CNT } T_POBJ_TYPE;		// number of object types

// Accessing Types:
#define POBJ_ACC_OBJECT			1		// read process object type
#define POBJ_ACC_VERSION		2		// read object version
#define POBJ_ACC_ERROR			3		// read last error code

#define POBJ_ACC_EVMASK			5		// read/write event mask
#define POBJ_ACC_LIST			6		// read list of object names
#define POBJ_ACC_LIST_EXTERN	7		// read list of line object names

#define POBJ_ACC_CONNECT		10		// read/write connection description
#define POBJ_ACC_DATA			11		// read/write object data
#define POBJ_ACC_STATUS			12		// read/write object state
#define POBJ_ACC_TYPE			13		// read/write object type (attributes, format)
#define POBJ_ACC_TYPE_EXTERN	14		// read external object type (attributes, format)
#define POBJ_ACC_REFRESH		15		// read/write data refresh time
#define POBJ_ACC_HYSTERESE		16		// read/write data hysterese
#define POBJ_ACC_DEFAULT		17		// read default data
#define POBJ_ACC_FUNCTION		18		// read/write data function

#define POBJ_ACC_UPLOAD			20		// upload module/file
#define POBJ_ACC_DOWNLOAD		21		// download module/file
#define POBJ_ACC_DATE_TIME		22		// read/write date and time
#define POBJ_ACC_MEM_DELETE		23		// clear memory
#define POBJ_ACC_MEM_INFO		24		// read memory informations
#define POBJ_ACC_MOD_TYPE		25		// read type of module
#define POBJ_ACC_UPLOAD_STM		26		// upload module/file (stream)
#define POBJ_ACC_DOWNLOAD_STM	27		// download module/file (stream)
#define POBJ_ACC_MOD_DATA		28		// read data from module

#define POBJ_ACC_LIST_LINE		30		// extended list of line object names
#define POBJ_ACC_LIST_DEVICE	31		// extended list of device object names
#define POBJ_ACC_LIST_STATION	32		// extended list of station object names
#define POBJ_ACC_LIST_CPU		33		// extended list of CPU object names
#define POBJ_ACC_LIST_MODULE	34		// extended list of module object names
#define POBJ_ACC_LIST_TASK		35		// extended list of task object names
#define POBJ_ACC_LIST_PVAR		36		// extended list of variable object names

#define POBJ_ACC_CPU_INFO		50		// read CPU information

#define POBJ_ACC_CANCEL			128		// cancel current request
#define POBJ_ACC_USERTAG		129		// user tag string

#define POBJ_ACC_INFO_LICENCE	200		// read PVI licence information
#define POBJ_ACC_LIST_CLIENTS	210		// read list of all PVI clients

#define POBJ_ACC_SNAPSHOT		240		// snapshot function

#define POBJ_ACC_LINEBASE		256		// base number for advanced line services

// Event Types:
#define POBJ_EVENT_ERROR		3		// error state event

#define POBJ_EVENT_CONNECT		10		// connect event
#define POBJ_EVENT_DATA			11		// data event
#define POBJ_EVENT_STATUS		12		// status event
#define POBJ_EVENT_DATAFORM		13		// data format event

#define POBJ_EVENT_PROCEEDING	128		// progress in % (0..100)
#define POBJ_EVENT_USERTAG		129		// user tag event

#define POBJ_EVENT_PVI_CONNECT	240		// connection to PVI established
#define POBJ_EVENT_PVI_DISCONN	241		// connection to PVI lost
#define POBJ_EVENT_PVI_ARRANGE	242		// arrange PVI objects

#define POBJ_EVENT_LINEBASE		256		// base number for advanced line events


// structure for PROCEEDING events:
typedef struct
{
	DWORD	nAccess;					// access type of the active request
	DWORD	Percent;					// progress of the active request (0%..100%)
	char	Info[32];					// optional text
} T_PROCEEDING_INFO;

// structure for PVI license information:
#define PVI_LCNAME_LEN			64		// max length for license name
typedef struct
{
	BYTE	PviWorkState[2];			// working state
	BYTE	_PviWorkState[2];			// inverted working state
	DWORD	Res1;						// reserved
	char	LcName[PVI_LCNAME_LEN+1];	// B&R license name
} T_PVI_INFO_LICENCE;

// PVI license information in structure element PviWorkState[0]:
#define PVIWORK_STATE_NULL		0		// undefined working state
#define PVIWORK_STATE_TRIAL		1		// working state: trial
#define PVIWORK_STATE_RUNTIME	2		// working state: runtime
#define PVIWORK_STATE_DEVELOPER	3		// historical - do not use
#define PVIWORK_STATE_LOCKED	4		// working state: locked

// PVI license information in structure element PviWorkState[1]:
#define PVIWORK_BURPC			(1<<0)	// bit indicates B&R IPC
#define PVIWORK_BURLC			(1<<1)	// bit indicates B&R license
#define PVIWORK_KEYRT			(1<<2)	// bit indicates PVI dongle (runtime)
#define PVIWORK_KEYDV			(1<<3)	// historical - do not use


// keywords for object description:
#define KWDESC_CONNECT			"CD"		// connection description
#define KWDESC_ALIGNMENT		"AL"		// alignment for structures (1-16)
#define KWDESC_PVTYPE			"VT"		// data type of variable
#define KWDESC_PVLEN			"VL"		// length of variable element (bytes)
#define KWDESC_PVCNT			"VN"		// number of elements
#define KWDESC_PVELEM			"VE"		// line internal element information
#define KWDESC_PVOFFS			"VO"		// line offset information for element
#define KWDESC_PVADDR			"VA"		// variable addressing
#define KWDESC_ATTRIBUTE		"AT"		// attribute
#define KWDESC_REFRESH			"RF"		// refresh time (ms)
#define KWDESC_HYSTERESE		"HY"		// data hysterese
#define KWDESC_FUNCTION			"FS"		// data function
#define KWDESC_DEFAULT			"DV"		// default data
#define KWDESC_CASTMODE			"CM"		// cast mode
#define KWDESC_EVMASK			"EV"		// event mask
#define KWDESC_LINKTYPE			"LT"		// link type
#define KWDESC_USERTAG			"UT"		// user tag string

// keywords for object service:
#define KWDESC_OBJTYPE			"OT"		// object type
#define KWDESC_SCOPE			"SC"		// valid range
#define KWDESC_SNAME			"SN"		// structure name
#define KWDESC_LOADTYPE			"LD"		// load state, memory type
#define KWDESC_INSTMODE			"IM"		// installation mode (download)
#define KWDESC_MODNAME			"MN"		// module name
#define KWDESC_STATUS			"ST"		// status
#define KWDESC_FORCESTATE		"FC"		// force state (0/1)
#define KWDESC_IOTYPE			"IO"		// IO type (attribut: Read/Write)
#define KWDESC_MODTYPE			"MT"		// module type
#define KWDESC_MODLEN			"ML"		// module length (bytes)
#define KWDESC_MODOFFSET		"MO"		// module offset (bytes)
#define KWDESC_MEMLEN			"SL"		// memory size (bytes)
#define KWDESC_MEMFREELEN		"SF"		// free memory size (bytes)
#define KWDESC_MEMBLOCKLEN		"SB"		// max free memory block
#define KWDESC_DATALEN			"DL"		// data length (bytes)
#define KWDESC_DATACNT			"DN"		// number of data entries
#define KWDESC_CPUNAME			"CN"		// logical CPU name
#define KWDESC_CPUTYPE			"CT"		// CPU type
#define KWDESC_AWSTYPE			"AW"		// AWS type
#define KWDESC_UNRESLKN			"UL"		// unresolved links
#define KWDESC_IDENTIFY			"ID"		// identifier
#define KWDESC_VERSION			"VI"		// version / revision


// keywords for memory service:
#define KWLOADTYPE_SYS_RAM		"SysRam"	// system RAM
#define KWLOADTYPE_RAM			"Ram"		// user RAM
#define KWLOADTYPE_SYS_ROM		"SysRom"	// system flash (ROM)
#define KWLOADTYPE_ROM			"Rom"		// user flash (ROM)
#define KWLOADTYPE_MEMCARD		"MemCard"	// memory card
#define KWLOADTYPE_FIX_RAM		"FixRam"	// fix RAM
#define	KWLOADTYPE_DRAM			"DRam"		// DRAM
#define	KWLOADTYPE_PER_MEM		"PerMem"	// permanent memory
#define KWLOADTYPE_DELETE		"Delete"	// delete module
#define KWLOADTYPE_CLEAR		"Clear"		// delete content

// keywords for installation mode (download service):
#define KWINSTMODE_OVERLOAD		"Overload"	// standard (default)
#define KWINSTMODE_COPY			"Copy"		// multiple cycles
#define KWINSTMODE_ONECYCLE		"OneCycle"	// one cycle

// keywords for status information:
#define KWSTATUS_WARMSTART		"WarmStart"
#define KWSTATUS_COLDSTART		"ColdStart"
#define KWSTATUS_START			"Start"
#define KWSTATUS_STOP			"Stop"
#define KWSTATUS_RESUME			"Resume"
#define KWSTATUS_CYCLE			"Cycle"
#define KWSTATUS_CYCLE_ARG		"Cycle(%u)"
#define KWSTATUS_RESET			"Reset"
#define KWSTATUS_RECONF			"Reconfiguration"
#define KWSTATUS_NMI			"NMI"
#define KWSTATUS_DIAGNOSE		"Diagnose"
#define KWSTATUS_ERROR			"Error"
#define KWSTATUS_EXISTS			"Exists"
#define KWSTATUS_LOADING		"Loading"
#define KWSTATUS_INCOMPLETE		"Incomplete"
#define KWSTATUS_COMPLETE		"Complete"
#define KWSTATUS_READY			"Ready"
#define KWSTATUS_INUSE			"InUse"
#define KWSTATUS_NONEXISTING	"NonExisting"
#define KWSTATUS_UNRUNNABLE		"Unrunnable"
#define KWSTATUS_IDLE			"Idle"
#define KWSTATUS_RUNNING		"Running"
#define KWSTATUS_STOPPED		"Stopped"
#define KWSTATUS_STARTING		"Starting"
#define KWSTATUS_STOPPING		"Stopping"
#define KWSTATUS_RESUMING		"Resuming"
#define KWSTATUS_RESETING		"Reseting"
#define KWSTATUS_CONSTANT		"Const"
#define KWSTATUS_VARIABLE		"Var"

// keywords for object type:
#define KWOBJTYPE_PVI			"Pvi"		// PVI base object
#define KWOBJTYPE_LINE			"Line"		// line object
#define KWOBJTYPE_DEVICE		"Device"	// device object
#define KWOBJTYPE_STATION		"Station"	// station object
#define KWOBJTYPE_CPU			"Cpu"		// CPU object
#define KWOBJTYPE_MODULE		"Module"	// module object
#define KWOBJTYPE_TASK			"Task"		// task object
#define KWOBJTYPE_PVAR			"Pvar"		// variable object


// keywords for variable type:
#define KWPVTYPE_INT8			"i8"		// 1 byte signed integer
#define KWPVTYPE_INT16			"i16"		// 2 byte signed integer
#define KWPVTYPE_INT32			"i32"		// 4 byte signed integer
#define KWPVTYPE_INT64			"i64"		// 8 byte signed integer
#define KWPVTYPE_UINT8			"u8"		// 1 byte unsigned integer
#define KWPVTYPE_UINT16			"u16"		// 2 byte unsigned integer
#define KWPVTYPE_UINT32			"u32"		// 4 byte unsigned integer
#define KWPVTYPE_UINT64			"u64"		// 8 byte unsigned integer
#define KWPVTYPE_FLOAT32		"f32"		// 4 byte float
#define KWPVTYPE_FLOAT64		"f64"		// 8 byte float (double)
#define KWPVTYPE_BOOLEAN		"boolean"	// bit (size = 1 byte)
#define KWPVTYPE_STRING			"string"	// string (array of bytes)
#define KWPVTYPE_STRUCTURE		"struct"	// structure
#define KWPVTYPE_DATA			"data"		// generic type
#define KWPVTYPE_TIME			"time"		// time
#define KWPVTYPE_DATI			"dt"		// date / time

// keywords for link type:
#define KWLINKTYPE_PRCDATA		"prc"		// process data
#define KWLINKTYPE_RAWDATA		"raw"		// raw data

// flags for event mask:
#define KWEVENTMASK_ERROR		'e'			// error event
#define KWEVENTMASK_DATA		'd'			// data event
#define KWEVENTMASK_DATAFORM	'f'			// format event
#define KWEVENTMASK_CONNECT		'c'			// connection event
#define KWEVENTMASK_STATUS		's'			// status event
#define KWEVENTMASK_PROCEEDING	'p'			// proceeding event
#define KWEVENTMASK_LINE		'l'			// line based event
#define KWEVENTMASK_USERTAG		'u'			// user tag event

// flags for variable attributes:
#define KWPVATTR_READ			'r'			// read
#define KWPVATTR_WRITE			'w'			// write
#define KWPVATTR_EVENT			'e'			// event driven by line
#define KWPVATTR_DIRECT			'd'			// every event will be passed
#define KWPVATTR_WRECHO			'h'			// write echo

// flags for variable scope:
#define KWOBJSCOPE_GLOBAL		'g'			// global		
#define KWOBJSCOPE_LOCAL		'l'			// local
#define KWOBJSCOPE_DYNAMIC		'd'			// dynamic

// syntactical characters:
#define PVICHR_USEPATH			'@'		// process object path name
#define PVICHR_PATH				'/'		// object path separator
#define PVICHR_ASSIGN			'='		// assignment
#define PVICHR_PARAM			'"'		// quoted parameter string
#define PVICHR_NESTING			'.'		// structure nesting
#define PVICHR_SUB_ON			'{'		// begin sub-entry
#define PVICHR_SUB_OFF			'}'		// end sub-entry
#define PVICHR_SEPAR			','		// value seperator
#define PVICHR_SEPAR_GROUP		';'		// group seperator
#define PVICHR_OBJECT			'\t'	// list entry seperator

// keywords for initialize string (function PviInitialize):
#define KWINIT_PRC_TIMEOUT		"PT"	// process timeout
#define KWINIT_MSG_LIMIT		"LM"	// message limiting
#define KWINIT_IPADDR			"IP"	// IP address for TCP/IP
#define KWINIT_IPPORT			"PN"	// port for TCP/IP
#define KWINIT_AUTOSTART		"AS"	// auto start PVI manager

// main keywords for connection description:
#define KWCD_LINE				"/LN"	// line name
#define KWCD_INTERFACE			"/IF"	// device name
#define KWCD_CONNECTION			"/CN"	// connection/station name/number
#define KWCD_RT_OBJECT			"/RO"	// runtime object name


// request/response/event mode:
enum { POBJ_MODE_NULL,				// undefiened
	   POBJ_MODE_EVENT,				// event
	   POBJ_MODE_READ,				// read
	   POBJ_MODE_WRITE,				// write
	   POBJ_MODE_CREATE,			// create object
	   POBJ_MODE_DELETE,			// delete object
	   POBJ_MODE_LINK,				// link object
	   POBJ_MODE_CHGLINK,			// change link object
	   POBJ_MODE_UNLINK };			// unlink object

// response and event message information structure:
typedef struct
{
	DWORD		LinkID;				// link object identifier
	DWORD		nMode;				// request/response/event mode
	DWORD		nType;				// type of access or event
	DWORD		ErrCode;			// != 0 -> error state
	DWORD		Status;				// response/event status
}	T_RESPONSE_INFO;

// response/event status flags (bits):
#define RESP_STATUS_CAST_OVERFLOW	(1<<0)	// data cast overflow
#define RESP_STATUS_CAST_UNDERFLOW	(1<<1)	// data cast underflow
#define RESP_STATUS_OVERFLOW		(1<<2)	// data function overflow
#define RESP_STATUS_UNDERFLOW		(1<<3)	// data function underflow
#define RESP_STATUS_POBJ			(1<<4)	// in process object
#define RESP_STATUS_LINK			(1<<5)	// in link object
#define RESP_STATUS_INHERIT_ERR		(1<<8)	// inherit error
#define RESP_STATUS_OVERWRITE		(1<<9)	// overwritten event
#define RESP_STATUS_NO_OVERWRITE	(1<<10)	// overwrite function locked

#define PVI_HMSG_NIL	(void*)1


//=============================================================================
// PVI Communication Functions
//=============================================================================

// simple callback function:
#define SET_PVICALLBACK			0xffffffff
typedef void (WINAPI *PVICALLBACK)(WPARAM,LPARAM);

// advanced callback function:
#define SET_PVICALLBACK_DATA	0xfffffffe
typedef void (WINAPI *PVICALLBACK_DATA)(WPARAM,LPARAM,LPVOID,DWORD,
										T_RESPONSE_INFO*);
// asynchronous callback function:
#define SET_PVICALLBACK_ASYNC	0xfffffffd
typedef void (WINAPI *PVICALLBACK_ASYNC)(WPARAM,LPARAM);


//-----------------------------------------------------------------------------
// initalisation/deinitalisation
//-----------------------------------------------------------------------------

PVICM_EXPORT INT WINAPI PviInitialize (LONG		ComTimeout,
									   LONG		RetryTimeMessage,
									   LPCSTR	pInitParam,
									   LPVOID	pRes2);

PVICM_EXPORT INT WINAPI PviXInitialize (DWORD*	hPviCom,
										LONG	ComTimeout,
									    LONG	RetryTimeMessage,
									    LPCSTR	pInitParam,
									    LPVOID	pRes2);

PVICM_EXPORT INT WINAPI PviDeinitialize (void);

PVICM_EXPORT INT WINAPI PviXDeinitialize (DWORD	hPviCom);

PVICM_EXPORT INT WINAPI PviSetGlobEventMsg (DWORD	nGlobEvent,
											LPVOID	hEventMsg,
											DWORD	EventMsgNo,
											LPARAM	EventParam);

PVICM_EXPORT INT WINAPI PviXSetGlobEventMsg (DWORD	hPviCom,
											 DWORD	nGlobEvent,
											 LPVOID	hEventMsg,
											 DWORD	EventMsgNo,
											 LPARAM	EventParam);


//-----------------------------------------------------------------------------
// create/delete objects
//-----------------------------------------------------------------------------

PVICM_EXPORT INT WINAPI PviCreateRequest (LPCSTR	pPObjName,
										  DWORD		nPObjType,
										  LPCSTR	pPObjDesc,
										  LPVOID	hEventMsg,
										  DWORD		EventMsgNo,
										  LPARAM	EventParam,
										  LPCSTR	pLinkDesc,
										  LPVOID	hRespMsg,
										  DWORD		RespMsgNo,
										  LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviXCreateRequest (DWORD	hPviCom,
										   LPCSTR	pPObjName,
										   DWORD	nPObjType,
										   LPCSTR	pPObjDesc,
										   LPVOID	hEventMsg,
										   DWORD	EventMsgNo,
										   LPARAM	EventParam,
										   LPCSTR	pLinkDesc,
										   LPVOID	hRespMsg,
										   DWORD	RespMsgNo,
										   LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviCreateResponse (WPARAM	wParam,
										   LPDWORD	pLinkID);

PVICM_EXPORT INT WINAPI PviXCreateResponse (DWORD	hPviCom,
											WPARAM	wParam,
										    LPDWORD	pLinkID);

PVICM_EXPORT INT WINAPI PviCreate (LPDWORD	pLinkID,
								   LPCSTR	pPObjName,
								   DWORD	nPObjType,
								   LPCSTR	pPObjDesc,
								   LPVOID	hEventMsg,
								   DWORD	EventMsgNo,
								   LPARAM	EventParam,
								   LPCSTR	pLinkDesc);

PVICM_EXPORT INT WINAPI PviXCreate (DWORD	hPviCom,
									LPDWORD	pLinkID,
								    LPCSTR	pPObjName,
								    DWORD	nPObjType,
								    LPCSTR	pPObjDesc,
								    LPVOID	hEventMsg,
								    DWORD	EventMsgNo,
								    LPARAM	EventParam,
								    LPCSTR	pLinkDesc);

PVICM_EXPORT INT WINAPI PviDeleteRequest (LPCSTR	pPObjName,
										  LPVOID	hRespMsg,
										  DWORD		RespMsgNo,
										  LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviXDeleteRequest (DWORD	hPviCom,
										   LPCSTR	pPObjName,
										   LPVOID	hRespMsg,
										   DWORD	RespMsgNo,
										   LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviDeleteResponse (WPARAM	wParam);

PVICM_EXPORT INT WINAPI PviXDeleteResponse (DWORD	hPviCom,
											WPARAM	wParam);

PVICM_EXPORT INT WINAPI PviDelete (LPCSTR	pPObjName);

PVICM_EXPORT INT WINAPI PviXDelete (DWORD	hPviCom,
								    LPCSTR	pPObjName);


//-----------------------------------------------------------------------------
// link/unlink objects
//-----------------------------------------------------------------------------

PVICM_EXPORT INT WINAPI PviLinkRequest (LPCSTR	pPObjName,
										LPVOID	hEventMsg,
										DWORD	EventMsgNo,
										LPARAM	EventParam,
										LPCSTR	pLinkDesc,
										LPVOID	hRespMsg,
										DWORD	RespMsgNo,
										LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviXLinkRequest (DWORD	hPviCom,
										 LPCSTR	pPObjName,
										 LPVOID	hEventMsg,
										 DWORD	EventMsgNo,
										 LPARAM	EventParam,
										 LPCSTR	pLinkDesc,
										 LPVOID	hRespMsg,
										 DWORD	RespMsgNo,
										 LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviLinkResponse (WPARAM		wParam,
										 LPDWORD	pLinkID);

PVICM_EXPORT INT WINAPI PviXLinkResponse (DWORD		hPviCom,
										  WPARAM	wParam,
										  LPDWORD	pLinkID);

PVICM_EXPORT INT WINAPI PviLink (LPDWORD	pLinkID,
								 LPCSTR		pPObjName,
								 LPVOID		hEventMsg,
								 DWORD		EventMsgNo,
								 LPARAM		EventParam,
								 LPCSTR		pLinkDesc);

PVICM_EXPORT INT WINAPI PviXLink (DWORD		hPviCom,
								  LPDWORD	pLinkID,
								  LPCSTR	pPObjName,
								  LPVOID	hEventMsg,
								  DWORD		EventMsgNo,
								  LPARAM	EventParam,
								  LPCSTR	pLinkDesc);

PVICM_EXPORT INT WINAPI PviChgLinkRequest (DWORD	LinkID,
										   DWORD	EventMsgNo,
										   LPARAM	EventParam,
										   LPVOID	hRespMsg,
										   DWORD	RespMsgNo,
										   LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviXChgLinkRequest (DWORD	hPviCom,
											DWORD	LinkID,
											DWORD	EventMsgNo,
											LPARAM	EventParam,
											LPVOID	hRespMsg,
											DWORD	RespMsgNo,
											LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviChgLinkResponse (WPARAM	wParam);

PVICM_EXPORT INT WINAPI PviXChgLinkResponse (DWORD	hPviCom,
											 WPARAM	wParam);

PVICM_EXPORT INT WINAPI PviChgLink (DWORD	LinkID,
									DWORD	EventMsgNo,
									LPARAM	EventParam);

PVICM_EXPORT INT WINAPI PviXChgLink (DWORD	hPviCom,
								     DWORD	LinkID,
									 DWORD	EventMsgNo,
									 LPARAM	EventParam);

PVICM_EXPORT INT WINAPI PviUnlinkRequest (DWORD		LinkID,
										  LPVOID	hRespMsg,
										  DWORD		RespMsgNo,
										  LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviXUnlinkRequest (DWORD	hPviCom,
										   DWORD	LinkID,
										   LPVOID	hRespMsg,
										   DWORD	RespMsgNo,
										   LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviUnlinkResponse (WPARAM	wParam);

PVICM_EXPORT INT WINAPI PviXUnlinkResponse (DWORD	hPviCom,
											WPARAM	wParam);

PVICM_EXPORT INT WINAPI PviUnlink (DWORD	LinkID);

PVICM_EXPORT INT WINAPI PviXUnlink (DWORD	hPviCom,
								    DWORD	LinkID);

PVICM_EXPORT INT WINAPI PviUnlinkAll (LPVOID	hMsg);

PVICM_EXPORT INT WINAPI PviXUnlinkAll (DWORD	hPviCom,
									   LPVOID	hMsg);


//-----------------------------------------------------------------------------
// read/write data or properties of objects
//-----------------------------------------------------------------------------

PVICM_EXPORT INT WINAPI PviReadArgumentRequest (DWORD	LinkID,
											    DWORD	nAccess,
											    LPVOID	pArgData,
												LONG	ArgDataLen,
												LPVOID	hRespMsg,
												DWORD	RespMsgNo,
												LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviXReadArgumentRequest (DWORD	hPviCom,
												 DWORD	LinkID,
												 DWORD	nAccess,
												 LPVOID	pArgData,
												 LONG	ArgDataLen,
												 LPVOID	hRespMsg,
												 DWORD	RespMsgNo,
												 LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviReadRequest (DWORD	LinkID,
									    DWORD	nAccess,
									    LPVOID	hRespMsg,
									    DWORD	RespMsgNo,
									    LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviXReadRequest (DWORD	hPviCom,
										 DWORD	LinkID,
										 DWORD	nAccess,
										 LPVOID	hRespMsg,
										 DWORD	RespMsgNo,
										 LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviReadResponse (WPARAM	wParam,
										 LPVOID	pData,
										 LONG	DataLen); 

PVICM_EXPORT INT WINAPI PviXReadResponse (DWORD		hPviCom,
										  WPARAM	wParam,
										  LPVOID	pData,
										  LONG		DataLen); 

PVICM_EXPORT INT WINAPI PviRead (DWORD	LinkID,
								 DWORD	nAccess,
								 LPVOID	pArgData,
								 LONG	ArgDataLen,
								 LPVOID	pData,
								 LONG	DataLen);

PVICM_EXPORT INT WINAPI PviXRead (DWORD		hPviCom,
								  DWORD		LinkID,
								  DWORD		nAccess,
								  LPVOID	pArgData,
								  LONG		ArgDataLen,
								  LPVOID	pData,
								  LONG		DataLen);

PVICM_EXPORT INT WINAPI PviWriteRequest (DWORD	LinkID,
										 DWORD	nAccess,
										 LPVOID	pData,
										 LONG	DataLen,
										 LPVOID	hRespMsg,
										 DWORD	RespMsgNo,
										 LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviXWriteRequest (DWORD		hPviCom,
										  DWORD		LinkID,
										  DWORD		nAccess,
										  LPVOID	pData,
										  LONG		DataLen,
										  LPVOID	hRespMsg,
										  DWORD		RespMsgNo,
										  LPARAM	RespParam);

PVICM_EXPORT INT WINAPI PviWriteResponse (WPARAM	wParam); 

PVICM_EXPORT INT WINAPI PviXWriteResponse (DWORD	hPviCom,
										   WPARAM	wParam); 

PVICM_EXPORT INT WINAPI PviWriteResultResponse (WPARAM	wParam,
												LPVOID	pData,
												LONG	DataLen); 

PVICM_EXPORT INT WINAPI PviXWriteResultResponse (DWORD	hPviCom,
												 WPARAM	wParam,
												 LPVOID	pData,
												 LONG	DataLen); 

PVICM_EXPORT INT WINAPI PviWrite (DWORD		LinkID,
								  DWORD		nAccess,
								  LPVOID	pData,
								  LONG		DataLen,
								  LPVOID	pRstData,
								  LONG		RstDataLen);

PVICM_EXPORT INT WINAPI PviXWrite (DWORD	hPviCom,
								   DWORD	LinkID,
								   DWORD	nAccess,
								   LPVOID	pData,
								   LONG		DataLen,
								   LPVOID	pRstData,
								   LONG		RstDataLen);

PVICM_EXPORT INT WINAPI PviGetResponseInfo (WPARAM				wParam,
											LPDWORD				pParam,
											LPDWORD				pDataLen,
											T_RESPONSE_INFO*	pInfo,
											DWORD				InfoLen);

PVICM_EXPORT INT WINAPI PviXGetResponseInfo (DWORD				hPviCom,	
											 WPARAM				wParam,
											 LPDWORD			pParam,
											 LPDWORD			pDataLen,
											 T_RESPONSE_INFO*	pInfo,
											 DWORD				InfoLen);


//-----------------------------------------------------------------------------
// read version
//-----------------------------------------------------------------------------

PVICM_EXPORT VOID WINAPI PviGetVersion (LPSTR	pVersBf,
										DWORD	VersBfLen);

#endif

/*=============================== End of file ===============================*/
