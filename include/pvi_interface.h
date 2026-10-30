
#ifndef __PVI_INTERFACE_H
#define __PVI_INTERFACE_H

#include "pvicom.h"

#include <windows.h>
#include <time.h>
#include <cstddef>
#include <memory>
#include <vector>

#ifdef __cplusplus
template <typename Callback>
inline INT PviCreate(LPDWORD linkId, LPCSTR objectName, DWORD objectType, LPCSTR objectDescription,
					Callback callback, DWORD eventMessage, LPARAM eventParameter, LPCSTR linkDescription) {
	return ::PviCreate(linkId, objectName, objectType, objectDescription,
					reinterpret_cast<LPVOID>(callback), eventMessage, eventParameter, linkDescription);
}

template <typename Callback>
inline INT PviReadRequest(DWORD linkId, DWORD accessType, Callback callback, DWORD responseMessage,
						  LPARAM responseParameter) {
	return ::PviReadRequest(linkId, accessType, reinterpret_cast<LPVOID>(callback), responseMessage,
							responseParameter);
}

template <typename Callback>
inline INT PviSetGlobEventMsg(DWORD eventType, Callback callback, DWORD eventMessage, LPARAM eventParameter) {
	return ::PviSetGlobEventMsg(eventType, reinterpret_cast<LPVOID>(callback), eventMessage, eventParameter);
}
#endif

/* Variable types */
typedef enum PVIPVARTYPE {
	BR_BOOL = 1,
	BR_USINT,
	BR_SINT,
	BR_UINT,
	BR_INT,
	BR_UDINT,
	BR_DINT,
	BR_REAL,
	BR_LREAL,
	BR_STRING,
	BR_WSTRING,
	BR_STRUCT,
	BR_TIME,
	BR_DATI,
	BR_TOD,
	BR_DATE
} PVIPVARTYPE;

/* Structure names */
typedef struct PVISTRUCTNAMES {
	char						*pname;
	struct PVISTRUCTNAMES		*next;
} PVISTRUCTNAMES;


/* Extended process variable information */
class PviObject;

typedef struct PVIPVAROBJECT{
	PVIPVARTYPE					type;				// Variable type
	const char				*pdatatype;			// Pointer to the data type name
	int							dimension;			// Number of array elements
	DWORD						length;				// Variable length
	void						*pvalue;			// Pointer to watch data
	BOOL						value_changed;		// Value has changed
	char						scope[2];			// Scope (global, local, dynamic)
	PviObject			*task;				// Pointer to the task object (or CPU object for global variables)
	PviObject			*cpu;				// Pointer to the CPU object
} PVIPVAROBJECT;


/* Extended device object information */
typedef struct PVIDEVICEOBJECT{
	unsigned					number;				// number in INI file
	unsigned long				broadcast;			// Broadcast IP
} PVIDEVICEOBJECT;


struct stEthernetCpuInfo {
    char macAddress[20+1];
    char ipAddress[16+1];
    char subnetMask[16+1];
    char gateway[16+1];
    int ipMethod;  /* 0 = fixed IP, 1 = DHCP-Client */
    int SNMP_mode; /* 0 = not activated, 1 = read only, 2 = active */
    int INA_activated;
    int INA_nodeNumber;
    int INA_portNumber;
    char targetTypeDescription[32+1];
    char arVersion[8+1];
    char arState[10+1];
};


/* Extended CPU object information */
typedef struct PVICPUOBJECT{
	char						arversion[8+1];		// Automation Runtime version
	char						cputype[32+1];		// PLC type (CPU type)
	char						status[15];			// CPU status
    struct stEthernetCpuInfo    ethernetCpuInfo;
	struct tm					rtc_time;		    
	BOOL						running;			// CPU is in 'RUN' state
} PVICPUOBJECT;

/* Extended task object information */
typedef struct PVITASKOBJECT {
	PviObject			*cpu;				
	char						status[15];			// Status
} PVITASKOBJECT;


class PviObject {
public:
	PviObject();

	PviObject *Add(BOOL create);
	PviObject *Unlink();
	PviObject *Remove();
	PviObject *Watch(BOOL watch);
	PviObject *FindChild(BOOL first);
	BOOL IsParentOf(const PviObject *child) const;
	PviObject *Expand();
	int AddToWatchFile(char *filename);

	static int Start();
	static int Stop();
	static PviObject *AddCpuByIp(const char *ipAddress, unsigned timeoutMs);
	static PviObject *GetNext(BOOL first);
	static PviObject *FindByName(const char *name);
	static PviObject *FindByLinkId(DWORD linkid);
	static int Count();
	static int LoadFromWatchFile(T_POBJ_TYPE typefilter, char *filename);

	DWORD 						linkid;
	T_POBJ_TYPE					type;
	char						name[256];
	char 						descriptor[256];
	int							error;
	int							watchsort;  // sort order in watch list
	struct stGUIINFO{
		unsigned has_childs;		// 1 = childs have already been created as separate objects
		unsigned loaded_from_file;// 1 = object was just loaded from file
		unsigned display_as_decimal; // 1 = display as decimal
		unsigned display_as_hex;
		unsigned display_as_binary;
		unsigned display_as_char;
		unsigned interpret_as_oem;
	} gui_info;
	union unEX{
		PVIDEVICEOBJECT			dev;
		PVICPUOBJECT			cpu;
		PVITASKOBJECT			task;
		PVIPVAROBJECT			pv;
	} ex;
private:
	void ReadDataType();
	PviObject *ReadCpuList();
	PviObject *ReadTaskList();
	PviObject *ReadPvarList();
	PviObject *ReadStructElements();

	static void ReadSettings();
	static void WINAPI Callback(WPARAM, LPARAM, LPVOID, DWORD, T_RESPONSE_INFO *);
	static void WINAPI GlobalEvent(WPARAM, LPARAM, LPVOID, DWORD, T_RESPONSE_INFO *);
	static void CALLBACK CyclicRequests(HWND, UINT, UINT_PTR, DWORD);
	static PviObject root;
	static std::vector<std::unique_ptr<PviObject>> objects;
	static std::size_t readIndex;
	static std::size_t childReadIndex;
};


typedef struct {
	void		(* cbobjects_valid)( void );
	void		(* cbdata_changed)( PviObject *object );
	void		(* cberror_changed)(PviObject *object, int error );
} PVIOBJECTNOTIFICATION;

extern PVIOBJECTNOTIFICATION pvi_interface_notify;

#endif
