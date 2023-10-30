
#ifndef __PVI_INTERFACE_H
#define __PVI_INTERFACE_H

#include "pvicom.h"

#include <windows.h>
#include <time.h>

/* Variablentypen */
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

/* Strukturnamen */
typedef struct PVISTRUCTNAMES {
	char						*pname;
	struct PVISTRUCTNAMES		*next;
} PVISTRUCTNAMES;


/* erweiterte Info Prozessvariable */
typedef struct PVIPVAROBJECT{
	PVIPVARTYPE					type;				// Variablentyp
	char						*pdatatype;			// Zeiger auf Datentypnamen
	int							dimension;			// dimension (bei Arrays)
	DWORD						length;				// Variablenlänge
	void						*pvalue;			// Zeiger auf Watchdaten
	BOOL						value_changed;		// Wert wurde geändert
	char						scope[2];			// Gültigkeitsbereich (global, lokal, dynamisch)
	struct PVIOBJECT			*task;				// Zeiger auf das Task-Objekt (oder auf das CPU -Objekt bei globalen Variablen)
	struct PVIOBJECT			*cpu;				// Zeiger auf das CPU- Objekt
} PVIPVAROBJECT;


/* erweiterte Info Geräteobjekt */
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


/* erweiterte Info CPU - objekt */
typedef struct PVICPUOBJECT{
	char						arversion[8+1];		// Version Automation Runtime
	char						cputype[32+1];		// SPS- Typ
	char						status[15];			//
    struct stEthernetCpuInfo    ethernetCpuInfo;
	struct tm					rtc_time;		    // Uhrzeit und Datum
	BOOL						running;			// im "RUN"
} PVICPUOBJECT;

/* erweiterte Info TASK- object */
typedef struct PVITASKOBJECT {
	struct PVIOBJECT			*cpu;				// Zeiger auf das CPU- Object
	char						status[15];			// Status
} PVITASKOBJECT;


/* allgem. Informationen zum PVI- Objekt */
typedef struct PVIOBJECT {
	DWORD 						linkid;
	T_POBJ_TYPE					type;
	char						name[256];  // dies sollte reichen :-)
	char 						descriptor[256];
	int							error;
	int							watchsort;  // Sortierung in Watchliste
	struct PVIOBJECT			*next; // Zeiger auf naechstes Objekt
	struct stGUIINFO{
		unsigned has_childs;		// 1 = Kinder wurden bereits als eigene Objekte angelegt
		unsigned loaded_from_file;// 1 = Objekt wurde gerade aus Datei geladen
		unsigned display_as_decimal; // 1 = Decimalanzeige
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
} PVIOBJECT;


typedef struct {
	void		(* cbobjects_valid)( void );
	void		(* cbdata_changed)( PVIOBJECT *object );
	void		(* cberror_changed)(PVIOBJECT *object, int error );
} PVIOBJECTNOTIFICATION;

PVIOBJECTNOTIFICATION pvi_interface_notify;

/* Prototypen */
int StartPvi( void );
int StopPvi( void );

PVIOBJECT *GetNextPviObject(BOOL);
PVIOBJECT *FindPviObjectByName( char *name );
PVIOBJECT *FindPviChildObject(PVIOBJECT * object, BOOL first );
PVIOBJECT *ExpandPviObject( PVIOBJECT * object );
PVIOBJECT *WatchPviObject( PVIOBJECT * object , BOOL watch );


int GetNumberOfPviObjects( void );
int AddToPviWatchFile( PVIOBJECT *object, char *filename );
int LoadPviObjectsFromWatchFile(T_POBJ_TYPE typefilter, char *filename);


#endif
