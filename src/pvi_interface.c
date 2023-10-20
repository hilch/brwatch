/*-------------------------------------------------------------------
Module   : pvi_interface.c
 -----------------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <direct.h>
#include "main.h"
#include "settings.h"
#include "cpusearch.h"
#include "stringtools.h"
#include "logger.h"


#define PVILOGFILE "pvilog.txt"
#define WATCH_FILE_VERSION 	"1.0"
#define MAX(x,y) ((x)>(y)?(x):(y))
#define MAX_SUPPORTED_DEVICES 256
#define MAX_SUPPORTED_CPUS 256


static int g_ansl = 0;


/* Prototypen */
static void WINAPI PviCallback (WPARAM, LPARAM, LPVOID, DWORD, T_RESPONSE_INFO* );
static void CALLBACK PviCyclicRequests( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime );
static void AddRowToLog( char*, int );
static void DeleteLog(void);
static void CreateUniqueObjectName( char *name, char *descriptor );

static PVIOBJECT *AddPviObject(PVIOBJECT *, BOOL);
static PVIOBJECT *UnlinkPviObject( PVIOBJECT *object );
static PVIOBJECT *RemovePviObject(PVIOBJECT *);
static PVIOBJECT *PviReadPvarList( PVIOBJECT * object );
static PVIOBJECT *PviReadCPUList(PVIOBJECT *object );
static PVIOBJECT *PviReadTaskList(PVIOBJECT *object);
static PVIOBJECT *PviReadStructElements( PVIOBJECT *pvar );
static PVIOBJECT *FindPviObjectByLinkId( DWORD );
static void PviReadDataType( PVIOBJECT *object );

/* */


static char *AddPviDatatype( char * t );
static int GetPviLicenceInfo(void );

/* global variable */
static char tempstring[256]; // save stack space
static UINT_PTR timerid_cyclic_requests;
static BOOL PLCDataChangeEvents;  // PLC- event mode

/* list for PVI objects */
static PVIOBJECT pvirootobject;
static PVIOBJECT *readpointer_pviobject; // zum Auslesen der Pvi- Objektliste

/* list for structure names  */
static PVISTRUCTNAMES structnamesroot = { "DATATYPE", NULL };


/* read the INI file */

static void ReadSettings(void) {
	PVIOBJECT tempobject, *pObject;
	PVIOBJECT *deviceObject;
	int numberOfDevices = 0;
	int numberOfCpus = 0;
	char section[20];
	char key[20];
	char buffer[80];

	deviceObject = calloc( MAX_SUPPORTED_DEVICES, sizeof(PVIOBJECT));
	if( deviceObject == NULL) {
		char tempstring[256];
		snprintf( tempstring, sizeof(tempstring), "Out of memory !" );
		MessageBox( MainWindowGetHandle(), tempstring, "Reading *.ini file", MB_OK );
		return;
	}


	memset(&tempobject, 0, sizeof(PVIOBJECT));

	sprintf( section, "General" );
	sprintf( key, "PLCDataChangeEvents" );
	strcpy( buffer, "" );
	GetPrivateProfileString(section, key, "", buffer, sizeof(buffer), SettingsGetFileName());
	PLCDataChangeEvents = atoi(buffer);



	// read devices from brwatch.ini
	pObject = deviceObject;
	numberOfDevices = 0;
	for (unsigned dev = 1; dev <= MAX_SUPPORTED_DEVICES; ++dev) {
		sprintf(section, "DEVICE%u", dev);
		strcpy(key, "broadcast");
		GetPrivateProfileString(section, key, "255.255.255.255", buffer, sizeof(buffer), SettingsGetFileName());
		tempobject.ex.dev.broadcast = inet_addr(buffer);

		strcpy(key, "name");
		strcpy( buffer, "" );
		GetPrivateProfileString(section, key, "", buffer, sizeof(buffer), SettingsGetFileName());

		strcpy(key, "descriptor");
		strcpy( buffer, "" );
		GetPrivateProfileString(section, key, "", buffer, sizeof(buffer), SettingsGetFileName());
		if (strlen(buffer) != 0) {
			// create DEVICE object
			strcpy(tempobject.descriptor, buffer);
			CreateUniqueObjectName(tempstring, tempobject.descriptor);
			snprintf(tempobject.name, sizeof(tempobject.name), "@Pvi/LNBRWATCH/DEV%s", tempstring);
			tempobject.type = POBJ_DEVICE;
			tempobject.ex.dev.number = dev;
			AddPviObject(&tempobject, TRUE);
			// store device's information for later use
			memcpy( pObject, &tempobject, sizeof(PVIOBJECT));
			++pObject;
			++numberOfDevices;
		}
	}

	if( numberOfDevices == 0 ) {
		char tempstring[256];
		snprintf( tempstring, sizeof(tempstring), "No PVI devices (interfaces) configured in .ini file !" );
		MessageBox( MainWindowGetHandle(), tempstring, "Reading *.ini file", MB_OK );
		return;
	}


	// read CPUs from brwatch.ini
	numberOfCpus = 0;
	for (unsigned cpu = 1; cpu <= MAX_SUPPORTED_CPUS; ++cpu) {
		char deviceName[256];
		memset(&tempobject, 0, sizeof(tempobject));
		sprintf(section, "CPU%u", cpu);
		strcpy(key, "descriptor");
		strcpy( buffer, "" );
		GetPrivateProfileString(section, key, "", buffer, sizeof(buffer), SettingsGetFileName());
		strcpy(tempobject.descriptor, buffer);

		strcpy(key, "device");
		strcpy( buffer, "" );
		GetPrivateProfileString(section, key, "", buffer, sizeof(buffer), SettingsGetFileName());

		if( strlen(buffer) != 0 ) {
			int deviceNumber = atoi(buffer);
			/* get device's descriptor */
			pObject = deviceObject;
			for( unsigned dev = 0; dev < numberOfDevices; ++dev, ++pObject) {
				if( deviceNumber == pObject->ex.dev.number ) {
					// get device's PVI path name
					char devicename[256];
					CreateUniqueObjectName(devicename, pObject->descriptor);
					// create CPU object
					CreateUniqueObjectName(tempstring, tempobject.descriptor);
					snprintf( tempobject.name, sizeof(tempobject.name), "@Pvi/LNBRWATCH/DEV%s/CPU%s", devicename, tempstring);
					tempobject.type = POBJ_CPU;
					AddPviObject(&tempobject, TRUE);
					++numberOfCpus;
					break;
				}
			}
		}
	}
	
	if( numberOfCpus == 0 ) {
		char tempstring[256];
		snprintf( tempstring, sizeof(tempstring), "No CPUs configured in .ini file !" );
		MessageBox( MainWindowGetHandle(), tempstring, "Reading *.ini file", MB_OK );
		return;
	}

	free( deviceObject );


}


/* create unique name from descriptor */

static void CreateUniqueObjectName( char *name, char *descriptor ) {
	while( *descriptor ) {
		if( *descriptor == ' ' )
			++descriptor;
		else if( *descriptor == '/' ) {
			*name++ = '#';
			++descriptor;
		} else if( *descriptor == '.' ) {
			*name ++ = '_';
			++descriptor;
		} else {
			*name++ = toupper(*descriptor++);
		}
	}
	*name = 0;
}


/* add object to list, expects pointer to temporary object from which parameters are copied
returns pointer to new object */

PVIOBJECT *AddPviObject(PVIOBJECT * tempobject, BOOL create ) {

	PVIOBJECT *lastobject;
	PVIOBJECT *newobject;
	char descriptor[256];
	int result = 0;
	BOOL already_exist;   // object already exist
	char *event_mask;

	already_exist = FALSE;

	if (tempobject == NULL) 			// object valid ?
		return NULL;

	// search for last object in list
	lastobject = &pvirootobject;
	while (lastobject->next != NULL) {
		lastobject = lastobject->next;
		if (!strcmp(tempobject->name, lastobject->name) ) {	// does object already exist ?
			already_exist = TRUE;
			break;
		}
	}


	// allocate memory for new object
	if (already_exist == TRUE) {
		newobject = lastobject;
	} else {
		if ((newobject = (PVIOBJECT *)malloc(sizeof(PVIOBJECT))) != NULL) {
			memcpy( newobject, tempobject, sizeof(PVIOBJECT) ); // copy everything for for first
			memset( &newobject->gui_info, 0, sizeof(newobject->gui_info) );
			newobject->gui_info.display_as_decimal = 1;
			newobject->gui_info.display_as_hex = 0;
			newobject->gui_info.display_as_binary = 0;
			newobject->gui_info.display_as_char = 0;
			newobject->watchsort = -1;
			newobject->next = NULL;	// pointer to next object
			lastobject->next = newobject;	// enter new object in list

		} else {
			AddRowToLog("AddPviObject:malloc()", -1);
			return NULL;
		}
	}

	// create variable in PVI manager if necessary
	if (create) {
		strcpy(descriptor, "CD=\"");
		strcat(descriptor, newobject->descriptor);
		strcat(descriptor, "\"");
		event_mask = "Ev=e";
		if( newobject->type == POBJ_PVAR ) {
			if( PLCDataChangeEvents && newobject->ex.pv.dimension <= 1 && newobject->ex.pv.type != BR_STRUCT
			        && newobject->ex.pv.type != BR_STRING && newobject->ex.pv.type != BR_WSTRING ) {
				strcat(descriptor, " RF=50 AT=rwe" );
			} else {
				strcat(descriptor, " RF=50 " );
			}
			event_mask = "Ev=ed";
		}

		result = PviCreate(&newobject->linkid, newobject->name, newobject->type, descriptor, PviCallback, SET_PVICALLBACK_DATA, 0, event_mask );

		if( result ) {
			memset( tempstring, 0, sizeof(tempstring) );
			snprintf(tempstring, sizeof(tempstring), "AddPviObject:PviCreate \"%s\"", tempobject->name);
			AddRowToLog(tempstring, result);
			return NULL;
		} else {
			if( newobject->type == POBJ_PVAR ) {
				PviReadRequest( newobject->linkid, POBJ_ACC_DATA, PviCallback, SET_PVICALLBACK_DATA, 0 );
				// read variable once
			}
		}
	}

	return newobject;

}


/* remove link to object */

static PVIOBJECT *UnlinkPviObject( PVIOBJECT *object ) {
	PVIOBJECT *find;
	int result;

	if( object == NULL )
		return NULL;

	GetNextPviObject(TRUE);

	while( (find = GetNextPviObject(FALSE)) != NULL ) {
		if( find == object ) {
			result = PviUnlink( object->linkid );
			if( result == 0 ) {
				object->linkid =0;
				return object;
			} else {
				AddRowToLog( "UnlinkPviObject", result );
				return NULL;
			}
		}
	}
	return NULL;
}


/* remove object from list */

static PVIOBJECT *RemovePviObject(PVIOBJECT * object ) {
	PVIOBJECT *prevobject, *find;

	prevobject = &pvirootobject;

	// search for predecessor...
	while( prevobject->next != NULL ) {
		find = prevobject->next;

		//if( !strcmp( find->name, object->name ) ) {
		if( find == object ) {
			PviUnlink(object->linkid);	// unlink PVI object
			object->linkid = 0;
			// ignore error since PVI manager could have already deleted childs

			prevobject->next = object->next;	// close gap in linked list


			if( object->type == POBJ_PVAR && object->ex.pv.pvalue != NULL ) { // free memory for watch if necessary
				free(object->ex.pv.pvalue);
				object->ex.pv.pvalue = NULL;
			}
			free(object);					  	// free memory
			return(prevobject);
		}
		prevobject = prevobject->next;		  	// next object
	}

	return( prevobject );
}

/* search for object with link id, expects PVI link id, returns object or NULL if not found */

static PVIOBJECT *FindPviObjectByLinkId( DWORD linkid ) {
	PVIOBJECT *object;

	object = &pvirootobject;
	while( object->next != NULL ) {	// until the end
		object = object->next;
		if( object->linkid == linkid ) {
			return object;
		}
	}
	return NULL;	// object not found
}



/* search for object by name, expects name of object */

PVIOBJECT *FindPviObjectByName( char *name ) {
	PVIOBJECT *object;

	if( name != NULL ) {
		object = &pvirootobject;
		while( object->next != NULL ) {	// until the end
			object = object->next;
			if( strcmp( object->name, name ) == 0 ) {
				return object;
			}
		}
	}
	return NULL;	// object not found
}


/* adds object to watch list */

PVIOBJECT *WatchPviObject(PVIOBJECT * object, BOOL watch) {
	if (object != NULL) {
		switch (object->type) {

			case POBJ_PVAR:
				if (watch) {
					object = AddPviObject( object, TRUE );
					if( object != NULL ) {
						object->ex.pv.pvalue = malloc(object->ex.pv.length); // allocate memeory
						if( object->ex.pv.pvalue ) {
							if( object->ex.pv.type == BR_REAL ) {
								* ((float*)object->ex.pv.pvalue) = 0.0;  /* initialize PV value */
							} else if( object->ex.pv.type == BR_LREAL ) {
								* ((double*)object->ex.pv.pvalue) = 0.0;  /* initialize PV value */
							} else {
								memset( object->ex.pv.pvalue, 0, object->ex.pv.length); /* initialize PV value */
							}
						}
					}
					return( object );
				} else {
					UnlinkPviObject(object);
					object->watchsort = -1;
					if( object->ex.pv.pvalue != NULL ) {
						free(object->ex.pv.pvalue);
						object->ex.pv.pvalue = NULL;
					}
				}
				return (object);
				break;

			default:
				return object;
		}
	}
	return NULL;

}



/* get next object in list, returns next object or first one if 'first' is set to 1 */

PVIOBJECT* GetNextPviObject( BOOL first  ) {
	if( first )
		return( readpointer_pviobject = &pvirootobject );


	if( readpointer_pviobject != NULL ) {
		if( readpointer_pviobject->next != NULL ) {
			readpointer_pviobject = readpointer_pviobject->next;
			return( readpointer_pviobject );
		}
	}
	return NULL;
}



/* search for child object, return first child if 'first' is set to 1 */

PVIOBJECT *FindPviChildObject(PVIOBJECT * object, BOOL first ) {
	static PVIOBJECT *find=NULL;
	size_t length;

	if (object != NULL) {
		if( first )
			find = &pvirootobject;

		length =  strlen(object->name);

		while (find->next != NULL) {
			find = find->next;
			if (strncmp(object->name, find->name, length ) == 0) {
				if( find->name[length] == '/' || find->name[length] == '[' || find->name[length] == '.' )
					if( strrchr( find->name+length+1, '.' )== NULL &&
					        strrchr( find->name+length+1, '/' )== NULL )  // no grandchilds
						return (find);
			}
		}
	}

	return NULL;
}



/* determines parent / child relationship */

BOOL IsPviObjectParentOf( PVIOBJECT *parent, PVIOBJECT * child ) {
	size_t length;


	if( parent == NULL || child == NULL )
		return FALSE;

	length = strlen(parent->name);

	if( strncmp( parent->name, child->name, length ) == 0 ) {
		if( parent != child )
			return TRUE;
	}
	return FALSE;

}


/* count number of objects */

int GetNumberOfPviObjects( void ) {
	PVIOBJECT *p;
	int i=0;

	p =pvirootobject.next;

	while( p != NULL ) {
		++i;
		p = p->next;
	}
	return(i);
}


/* global PVI event */

static void WINAPI Pvi_GlobalEvent(WPARAM wParam, LPARAM lParam, LPVOID pData, DWORD DataLen, T_RESPONSE_INFO * pInfo) {
	PVIOBJECT po, *ppo;


	switch (pInfo->nType) {
		case POBJ_EVENT_PVI_CONNECT:

			break;

		case POBJ_EVENT_PVI_DISCONN:
			AddRowToLog( "POBJ_EVENT_PVI_DISCONN", -1 );
			break;

		case POBJ_EVENT_PVI_ARRANGE: {
			char section[20];
			char key[20];
			char buffer[80];
			sprintf( section, "General" );
			sprintf( key, "Ansl" );
			strcpy( buffer, "" );
			GetPrivateProfileString(section, key, "", buffer, sizeof(buffer), SettingsGetFileName());
			g_ansl = atoi(buffer);
		}

		AddRowToLog("POBJ_EVENT_PVI_ARRANGE", 0);
		ppo = &po;
		strcpy( po.name, "@Pvi/LNBRWATCH");
		if( g_ansl ) {
			strcpy( po.descriptor,"LnAnsl");
		} else {
			strcpy( po.descriptor,"LnIna2");
		}
		po.type = POBJ_LINE;
		AddPviObject( ppo, 1 );
		ReadSettings();

		if( pvi_interface_notify.cbobjects_valid != NULL ) {
			pvi_interface_notify.cbobjects_valid();
		}
		break;
	}

}


/* PVI callback */

static void WINAPI PviCallback (WPARAM wParam, LPARAM lParam, LPVOID pData, DWORD DataLen, T_RESPONSE_INFO* pInfo) {
	PVIOBJECT *object;
	static char value[128];
	char *s, *d;

	switch( pInfo->nType ) {
		case POBJ_EVENT_DATA:			// data change event
			if( DataLen == 0 )			// no data
				return;
			object = FindPviObjectByLinkId(pInfo->LinkID);
			if( object != NULL ) {
				if( object->type == POBJ_PVAR ) {
					switch( object->ex.pv.type ) {
						case BR_STRING:
						case BR_WSTRING:
							if( object->ex.pv.pvalue != NULL )
								memcpy( object->ex.pv.pvalue, pData, MAX(DataLen,object->ex.pv.length) );
							break;

						case BR_USINT:
						case BR_SINT:
						case BR_BOOL:
							if( object->ex.pv.pvalue != NULL )
								memcpy( object->ex.pv.pvalue, pData, 1 );
							break;

						case BR_UINT:
						case BR_INT:
							if( object->ex.pv.pvalue != NULL )
								memcpy( object->ex.pv.pvalue, pData, 2 );
							break;

						case BR_UDINT:
						case BR_DINT:
						case BR_DATI:
						case BR_TOD:
						case BR_DATE:
						case BR_TIME:
						case BR_REAL:
							if( object->ex.pv.pvalue != NULL )
								memcpy( object->ex.pv.pvalue, pData, 4 );
							break;

						case BR_LREAL:
							if( object->ex.pv.pvalue != NULL )
								memcpy( object->ex.pv.pvalue, pData, 8 );
							break;

						default:
							break;



					}
					if( object->error ) { // reset error state
						object->error = 0;
						if( pvi_interface_notify.cberror_changed != NULL )
							pvi_interface_notify.cberror_changed( object, 0 );
					}
					if( pvi_interface_notify.cbdata_changed != NULL )
						pvi_interface_notify.cbdata_changed( object );

					if( object->ex.pv.pvalue != NULL ) {
						object->ex.pv.value_changed = 1; /* value was changed */
						LoggerDataChanged( object );
					}

				}
			}
			break;


		case POBJ_EVENT_ERROR:			// change of error state
			object = FindPviObjectByLinkId(pInfo->LinkID);
			if( object != NULL ) {
				object->error = pInfo->ErrCode;

				if( pvi_interface_notify.cberror_changed != NULL )
					pvi_interface_notify.cberror_changed( object, pInfo->ErrCode );
			}

			break;


		case POBJ_ACC_STATUS:			//change of state
			if( pInfo->ErrCode != 0 )
				return;
			object = FindPviObjectByLinkId(pInfo->LinkID);
			if( object != NULL ) {
				if( pvi_interface_notify.cbdata_changed != NULL ) {
					s = (char*) pData;
					d = value;

					*d = 0;
					while( *s ) {
						if( FindToken( &s, "ST=" )) {
							while( (*d++ = *s++) != 0 );
							*d = 0;
							break;
						}
						++s;
					}


					switch( object->type ) {
						case POBJ_CPU:
							strncpy( object->ex.cpu.status, value, sizeof(object->ex.cpu.status)-1);
							if( strcmp( value, KWSTATUS_WARMSTART ) == 0 || strcmp( value, KWSTATUS_COLDSTART ) == 0 ) {
								object->ex.cpu.running = TRUE;
							} else {
								object->ex.cpu.running = FALSE;
							}
							break;

						case POBJ_TASK:
							strncpy( object->ex.task.status, value, sizeof(object->ex.task.status)-1);
							break;

						default:
							break;
					}
					pvi_interface_notify.cbdata_changed( object );
				}

			}
			break;


		case POBJ_ACC_DATE_TIME:		// read clock
			object = FindPviObjectByLinkId(pInfo->LinkID);
			if( object != NULL ) {
				if( object->type == POBJ_CPU ) {
					if( DataLen > sizeof(struct tm) )
						DataLen = sizeof(struct tm);
					memcpy( &object->ex.cpu.rtc_time, pData, DataLen );
					// not notification since POBJ_ACC_STATUS does that already...
				}
			}
			break;
	}

}



/* is used to send cyclic requests, since not all events are supported for all access types. */
static void CALLBACK PviCyclicRequests( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime ) {
	PVIOBJECT *object;
	static int zyklus_readpv = 0;
	static int zyklus_getlicenseinfo = 0;

	if( ++zyklus_readpv > 4 ) {
		zyklus_readpv = 0;
	}

	// read PVI license information
	if( zyklus_getlicenseinfo < 10) {
		if( ++zyklus_getlicenseinfo >= 10 ) {
			if( GetPviLicenceInfo() == PVIWORK_STATE_LOCKED ) {
				MessageBox( MainWindowGetHandle(), "Sorry PVI is locked.\nProgram must shut down !", "Error Pvi License", MB_ICONERROR );
				SendMessage( MainWindowGetHandle(), WM_CLOSE, 0, 0 );
			} else {
				zyklus_getlicenseinfo = 0;
			}
		}
	}

	object = GetNextPviObject( TRUE );

	while( (object = GetNextPviObject(FALSE)) != NULL ) {
		if( object->linkid == 0 || object->error != 0 )
			continue;

		switch( object->type ) {
			case POBJ_CPU:
				PviReadRequest(object->linkid, POBJ_ACC_STATUS, PviCallback, SET_PVICALLBACK_DATA, object->linkid );
				PviReadRequest(object->linkid, POBJ_ACC_DATE_TIME, PviCallback, SET_PVICALLBACK_DATA, object->linkid );
				break;

			case POBJ_TASK:
				PviReadRequest(object->linkid, POBJ_ACC_STATUS, PviCallback, SET_PVICALLBACK_DATA, object->linkid );
				break;

			default:
				break;

		}
	}

}


static int GetPviLicenceInfo( void ) {
	DWORD LinkId;
	long result;
	T_PVI_INFO_LICENCE license;

	memset( &license, 0, sizeof(license ) );
	result = PviLink( &LinkId, "@Pvi", PVI_HMSG_NIL, 0, 0, 0 );

	if( result == 0 ) {
		result = PviRead( LinkId, POBJ_ACC_INFO_LICENCE, 0, 0, &license, sizeof(license) );
	}

	if( result == 12059 ) {
		return PVIWORK_STATE_LOCKED;
	}

	else if( result != 0 ) {
		AddRowToLog( "GetPviLicenceInfo()", result );
	}

	PviUnlink(LinkId);

	return license.PviWorkState[0];
}


/* start PVI connection */
int StartPvi( void ) {
	int result;

	memset( &pvirootobject, 0, sizeof(PVIOBJECT) );
	strcpy( pvirootobject.name, "@Pvi");
	strcpy( pvirootobject.descriptor, "");
	pvirootobject.type = POBJ_PVI;
	pvirootobject.next = NULL;



	DeleteLog(); // delete PVI log file

	// write PVI version to logfile
	strcpy( g_PVIVersionString, "" );
	PviGetVersion( g_PVIVersionString, sizeof(g_PVIVersionString) );
	AddRowToLog( g_PVIVersionString, 0 );

	//start PVI
	{
		char buffer[80];

		memset( buffer, 0, sizeof(buffer) );
		GetPrivateProfileString( "REMOTE", "parameter", "COMT=10 AS=1 PT=20", buffer, sizeof(buffer), SettingsGetFileName());
		result = PviInitialize( 0, 0, buffer, NULL );
	}

	if( result ) {
		AddRowToLog( "PviInitialize", result );
		return result;
	}




	// global events
	result = PviSetGlobEventMsg( POBJ_EVENT_PVI_ARRANGE, Pvi_GlobalEvent, SET_PVICALLBACK_DATA, 0 );

	if( result ) {
		AddRowToLog( "PviSetGlobEventMsg", result );
		return result;
	}


	// timer for cyclic requests
	timerid_cyclic_requests = SetTimer( NULL, 0, 500, (TIMERPROC) PviCyclicRequests );


	return 0; // Alles OK
}


/* stop PVI */
int StopPvi(void) {

	int result;
	PVIOBJECT *prevobject;

	// kill timer for cyclic requests
	KillTimer( NULL, timerid_cyclic_requests );

	// delete all objects
	prevobject = &pvirootobject;
	while (prevobject->next != NULL) {
		prevobject = RemovePviObject(prevobject->next);
	}


	result = PviDeinitialize();

	// delete all data types
	AddPviDatatype( NULL );

	AddRowToLog("PviDeinitialize", result);
	return result;
}


/* =================================================================================
   functions to work with data types
   ==================================================================================
*/


static char *AddPviDatatype( char * t ) {
	size_t length;
	PVISTRUCTNAMES *p, *kill;

	if( t == NULL ) { // delete all data types
		p = structnamesroot.next	;
		while( p != NULL ) {
			kill = p;
			p = kill->next;
			if( kill->pname != NULL )
				free(kill->pname);
			free(kill);
		}
		structnamesroot.next = NULL;
		return ("nil!");
	}

	length = strlen(t); // length of data type's name

	// search for data type
	p = &structnamesroot;
	while( p->next != NULL ) {
		p = p->next;
		if( p->pname != NULL ) {
			if( !strcmp( p->pname, t ) ) { // already exists
				return( p->pname );
			}
		} else {
			return ("ERROR!");
		}
	}

	// insert data type
	p->next = malloc( sizeof(PVISTRUCTNAMES) );
	if( p->next != NULL ) {
		p = p->next;
		p->pname = malloc( length + 1); // space for name
		p->next = NULL;
		if( p->pname != NULL ) {
			return( strcpy( p->pname, t ) );
		}
	}


	return ("ERROR!");
}

/* ===================================================================================
	functions to extract object information
	==================================================================================
*/


/* gets the data type of object from CPU */
static void PviReadDataType( PVIOBJECT *object ) {
	char buffer[256];
	int result;
	char *s;

	result = PviRead( object->linkid, POBJ_ACC_TYPE_EXTERN, NULL, 0, buffer, sizeof(buffer) );
	if( result == 0 ) {
		s = buffer;

		while ( *s ) {
			if (*s == ' ') {	
				++s;			// ignore spaces
				continue;
			}

			if( FindToken( &s, "{" ) ) { // start of struct definition
				break;
			}

			if (FindToken(&s, KWDESC_PVLEN "=" )) { 	// variable's byte length
				object->ex.pv.length = GetIntValue(&s);
				continue;
			}

			if (FindToken(&s,  KWDESC_PVTYPE "=")) {	// variable type
				if (FindToken(&s, KWPVTYPE_BOOLEAN)) {
					object->ex.pv.type = BR_BOOL;
					object->ex.pv.length = 1;
					object->ex.pv.pdatatype = "BOOL";
				} else if (FindToken(&s, KWPVTYPE_UINT8)) {
					object->ex.pv.type = BR_USINT;
					object->ex.pv.length = 1;
					object->ex.pv.pdatatype = "USINT";
				} else if (FindToken(&s, KWPVTYPE_INT8)) {
					object->ex.pv.type = BR_SINT;
					object->ex.pv.length = 1;
					object->ex.pv.pdatatype = "SINT";
				} else if (FindToken(&s, KWPVTYPE_UINT16)) {
					object->ex.pv.type = BR_UINT;
					object->ex.pv.length = 2;
					object->ex.pv.pdatatype = "UINT";
				} else if (FindToken(&s, KWPVTYPE_INT16)) {
					object->ex.pv.type = BR_INT;
					object->ex.pv.length = 2;
					object->ex.pv.pdatatype = "INT";
				} else if (FindToken(&s, KWPVTYPE_UINT32)) {
					object->ex.pv.type = BR_UDINT;
					object->ex.pv.length = 4;
					object->ex.pv.pdatatype = "UDINT";
				} else if (FindToken(&s, KWPVTYPE_INT32)) {
					object->ex.pv.type = BR_DINT;
					object->ex.pv.length = 4;
					object->ex.pv.pdatatype = "DINT";
				} else if (FindToken(&s, KWPVTYPE_FLOAT32)) {
					object->ex.pv.type = BR_REAL;
					object->ex.pv.length = 4;
					object->ex.pv.pdatatype = "REAL";
				} else if (FindToken(&s, KWPVTYPE_FLOAT64)) {
					object->ex.pv.type = BR_LREAL;
					object->ex.pv.length = 8;
					object->ex.pv.pdatatype = "LREAL";
				} else if (FindToken(&s, KWPVTYPE_DATI)) {
					object->ex.pv.type = BR_DATI;
					object->ex.pv.length = 4;
					object->ex.pv.pdatatype = "DATE_AND_TIME";
				} else if (FindToken(&s, KWPVTYPE_DATE)) {
					object->ex.pv.type = BR_DATE;
					object->ex.pv.length = 4;
					object->ex.pv.pdatatype = "DATE";
				} else if (FindToken(&s, KWPVTYPE_TIME)) {
					object->ex.pv.type = BR_TIME;
					object->ex.pv.length = 4;
					object->ex.pv.pdatatype = "TIME";
				} else if (FindToken(&s, KWPVTYPE_TOD)) {
					object->ex.pv.type = BR_TOD;
					object->ex.pv.length = 4;
					object->ex.pv.pdatatype = "TOD";
				} else if (FindToken(&s, KWPVTYPE_STRUCTURE)) {
					object->ex.pv.type = BR_STRUCT;
					if( object->ex.pv.pdatatype == NULL )
						object->ex.pv.pdatatype = "DATATYPE";
				} else if (FindToken(&s, KWPVTYPE_STRING)) {
					object->ex.pv.type = BR_STRING;
					object->ex.pv.pdatatype = "STRING";
				} else if (FindToken(&s, KWPVTYPE_WSTRING)) {
					object->ex.pv.type = BR_WSTRING;
					object->ex.pv.pdatatype = "WSTRING";
				} else {
					object->ex.pv.type = 0;	
					object->ex.pv.pdatatype = "UNKNOWN";
					while (*s != ' ' && *s != 0)
						++s;	// ignore rest
				}
				continue;
			}

			if (FindToken(&s, KWDESC_SCOPE "=")) {  // scope of variable
				object->ex.pv.scope[0] = *s++;
				object->ex.pv.scope[1] = 0;
				while (*s != ' ' && *s != 0)
					++s;	// ignore rest
				continue;
			}

			if (FindToken(&s, KWDESC_PVCNT "=")) {	// number of elements
				object->ex.pv.dimension = GetIntValue(&s);
				continue;
			}
			++s;
		} /* end while()*/

		if( object->ex.pv.type == BR_STRUCT ) {
			//PviReadStructElements( object );
		}

	}
}


/* creates the children of the object as seperate objects */
PVIOBJECT *ExpandPviObject( PVIOBJECT * object ) {
	int i;
	PVIOBJECT *tempobject;

	if( object == NULL )
		return NULL;

	tempobject = malloc(sizeof(PVIOBJECT));

	if( tempobject != NULL ) {

		if( object->gui_info.has_childs == 0 ) {
			switch( object->type ) {
				case POBJ_PVAR: 	/* process variable */
					if( object->ex.pv.dimension > 1 ) { // array ?

						if( object->ex.pv.type == BR_STRUCT ) { // struct array ?
							PviReadStructElements( object ); // get struct definition only once
						}

						// create all array elements
						for( i = 0; i < object->ex.pv.dimension; ++i ) {
							memcpy( tempobject, object, sizeof(PVIOBJECT) );
							tempobject->ex.pv.dimension = 1;
							snprintf( tempobject->name, sizeof(tempobject->name), "%s[%i]", object->name, i );
							snprintf( tempobject->descriptor, sizeof(tempobject->descriptor), "%s[%i]", object->descriptor, i );
							if( AddPviObject( tempobject, FALSE ) != NULL ) {
								object->gui_info.has_childs = 1;  // at least one child was found
							}
						}

					}

					else

						if( object->ex.pv.type == BR_STRUCT ) {
							if( PviReadStructElements( object ) != NULL ) {
								object->gui_info.has_childs = 1;
							}
						}
					break;

				case POBJ_CPU:
					if( PviReadTaskList(object) != NULL ) {
						object->gui_info.has_childs = 1;
					}
					object->gui_info.has_childs = 0;
					break;

				case POBJ_TASK:
					if( PviReadPvarList(object) == 0 ) {
						object->gui_info.has_childs = 1;
					}
					break;

				case POBJ_DEVICE:
					PviReadCPUList(object);
					object->gui_info.has_childs = 0;
					break;

				default:
					break;
			}
		}

		free(tempobject);
	}
	return NULL;
}



/* searches for reachable CPUs
	input: pointer to device object
	result : pointer to device object or NULL in case of error

*/

static PVIOBJECT *PviReadCPUList(PVIOBJECT *deviceObject ) {
	PVIOBJECT *cpuObject;
	char *s;
	char descriptor[256];
	int result;
	char *pbuffer;

	if( deviceObject == NULL )
		return NULL;

	if( deviceObject->type != POBJ_DEVICE )
		return NULL;


	pbuffer = malloc(65535);
	if( pbuffer == NULL ) {
		AddRowToLog( "PviReadCPUList():malloc(65535)", -1 );
		return NULL;
	}


	cpuObject = calloc( 1, sizeof(PVIOBJECT) );
	if( cpuObject == NULL ) {
		free(pbuffer);
		AddRowToLog( "PviReadCPUList():malloc(PVIOBJECT)", -1 );
		return NULL;
	}


	s= _strupr(deviceObject->descriptor);
	while( *s ) {
		// search for CPUs connected to a serial port
		if( FindToken( &s, "/IF=COM" ) ) {

			CreateUniqueObjectName( tempstring, cpuObject->descriptor );
			snprintf( cpuObject->name, sizeof(cpuObject->name), "%s/CPU%s", deviceObject->name, tempstring  );

			cpuObject->type = POBJ_CPU;
			snprintf( descriptor, sizeof(descriptor), "CD=\"%s\"", cpuObject->descriptor );

			result = PviCreate(&cpuObject->linkid, cpuObject->name, cpuObject->type, descriptor, PviCallback, SET_PVICALLBACK_DATA, 0, "Ev=e" );
			if( result == 0 ) {
				result = PviRead(cpuObject->linkid, POBJ_ACC_VERSION, NULL, 0, pbuffer, 65535 );
				if( result == 0 ) {
					if( AddPviObject( cpuObject, 1 ) == NULL ) {
						deviceObject = NULL; //Fehler
					}
					break;
				}
			}
			switch( result ) {
				case 4808:
					MessageBox( NULL, "No CPU found !", "Error", MB_OK | MB_ICONEXCLAMATION );
					break;

				case 13076:
					MessageBox( NULL, "Device does not exist !", "Error", MB_OK | MB_ICONEXCLAMATION );
					break;


				default:
					AddRowToLog( "PviReadCPUList()", result );
					break;
			}
			break;
		}

		// search for CPUs connected via ethernet
		if( FindToken( &s, "/IF=TCPIP" ) ) {
			struct stEthernetCpuInfo *buffer = calloc(256, sizeof(struct stEthernetCpuInfo) );
			struct stEthernetCpuInfo *ethernetCpuInfo = buffer;

			if( ethernetCpuInfo != NULL ) {

				int i;
				int noOfCpu = SearchEthernetCpus( ethernetCpuInfo, 256 );
				for( i = 0; i < noOfCpu; ++i, ++ethernetCpuInfo ) {
					memset( cpuObject, 0, sizeof(PVIOBJECT) );
					memcpy( &cpuObject->ex.cpu.ethernetCpuInfo, (void*) ethernetCpuInfo, sizeof(struct stEthernetCpuInfo) );
					sprintf( cpuObject->descriptor, "/DAIP=%s", ethernetCpuInfo->ipAddress );
					strcpy( cpuObject->ex.cpu.arversion, ethernetCpuInfo->arVersion );
					strcpy( cpuObject->ex.cpu.cputype, ethernetCpuInfo->targetTypeDescription );
					if( strlen(cpuObject->ex.cpu.ethernetCpuInfo.macAddress) ) {
						// the MAC is the better unique name
						snprintf( cpuObject->name, sizeof(cpuObject->name), "%s/CPU%s", deviceObject->name, cpuObject->ex.cpu.ethernetCpuInfo.macAddress );
					} else {
						CreateUniqueObjectName( tempstring, cpuObject->descriptor );
						snprintf( cpuObject->name, sizeof(cpuObject->name), "%s/CPU%s", deviceObject->name, tempstring );
					}
					cpuObject->type = POBJ_CPU;
					snprintf( descriptor, sizeof(descriptor), "CD=\"%s\"", cpuObject->descriptor );
					result = PviCreate(&cpuObject->linkid, cpuObject->name, cpuObject->type, descriptor, PviCallback, SET_PVICALLBACK_DATA, 0, "Ev=s" );
					if( result == 0 ) {
						AddPviObject( cpuObject, 1 );
					}
				}
				free(buffer);
			}
			break;
		}

		++s;
	}

	free(pbuffer);
	free(cpuObject);
	return deviceObject;
}


/* Reads a list of tasks available on the CPU */
static PVIOBJECT * PviReadTaskList(PVIOBJECT *object) {
	PVIOBJECT *tempobject, *o;
	int result = 0;
	char *pbuffer;
	char *s, *d;

	if( object == NULL )
		return NULL;

	if( object->type != POBJ_CPU )
		return NULL;

	pbuffer = (char *)malloc(65535);

	if( pbuffer == NULL ) {
		AddRowToLog("PviRefreshTaskList:malloc(65535)", -1 );
		return NULL;  // error
	}

	tempobject = malloc(sizeof(PVIOBJECT));

	if( tempobject == NULL ) { // error
		free(pbuffer); 
		AddRowToLog("PviRefreshTaskList:malloc(PVIOBJECT)", -1 );
		return NULL;  
	}

	/* get version of Automation Runtime  */
	object->ex.cpu.arversion[0] = 0;
	result = PviRead(object->linkid, POBJ_ACC_VERSION, NULL, 0, object->ex.cpu.arversion, sizeof(object->ex.cpu.arversion)-1 );

	if( result != 0 ) {
		return NULL;
	}

	/* get CPU type */
	object->ex.cpu.cputype[0] = 0;
	result = PviRead(object->linkid, POBJ_ACC_CPU_INFO, NULL, 0, pbuffer, 65535);
	if( result == 0 ) {
		s = pbuffer;
		while( s ) {
			if( FindToken( &s, "CT=" ) ) {
				strcpy( object->ex.cpu.cputype, s );
				break;
			}
			++s;
		}
	}

	/* get CPU status */
	result = PviRead(object->linkid, POBJ_ACC_STATUS, NULL, 0, pbuffer, 65535 );
	if( result != 0 ) {
		return NULL;

	}


	/* read list of tasks */
	if( object->error != 0 )
		return NULL;

	result = PviRead(object->linkid, POBJ_ACC_LIST_TASK, NULL, 0, pbuffer, 65535);
	if (result == 0) {
		s = pbuffer;
		while(*s) {
			memset(tempobject, 0, sizeof(PVIOBJECT));
			d = tempobject->descriptor;
			do {
				*d++ = *s++;
			} while ((*s != 0) && (*s != '\t'));
			++s;					// ignore tabs
			*d = 0;					// terminate with zero
			strcpy( tempobject->name, object->name );
			strcat( tempobject->name, "/" );
			strcat( tempobject->name, tempobject->descriptor );
			tempobject->type = POBJ_TASK;

			if( (o=AddPviObject(tempobject, 1)) != NULL ) {
				o->ex.task.cpu = object; // store pointer to CPU object
				object->gui_info.has_childs = 1; // children were found
			}
		}
	} else {
		AddRowToLog("PviRefreshTaskList", result);
	}


	free(pbuffer);
	free(tempobject);
	return object;  // return pointer to CPU object
}


/* reads the list of variables that are accessible via the specified object */ 
static PVIOBJECT *PviReadPvarList(PVIOBJECT * object) {
	PVIOBJECT newobject;
	char *pbuffer, *s, *d;
	int result;

	if (object == NULL)
		return NULL;

	if (object->type != POBJ_CPU && object->type != POBJ_TASK)
		return NULL;


	pbuffer = (char *)malloc(65535);

	if (pbuffer == NULL) {
		AddRowToLog("PviReadPvarList:malloc(65535)", -1);
		return NULL;			// error
	}

	memset(pbuffer, 0, 65535 );

	/* link object */
	AddPviObject( object, 1 );
	/* read list of variables */
	result = PviRead(object->linkid, POBJ_ACC_LIST_PVAR, NULL, 0, pbuffer, 65530);
	if (result == 0) {
		s = pbuffer;
		strcat(pbuffer, "\t");

		memset(&newobject, 0, sizeof(PVIOBJECT));
		while ( *s ) {
			if (*s == ' ') {	
				++s;			// ignore space character
				continue;
			}

			if (strlen(newobject.descriptor) == 0) {	// get variable name
				d = newobject.descriptor;
				while (*s && *s != ' ') {
					*d++ = *s++;
				}
				*d = 0;			// terminate with zero;
				continue;
			}

			if (FindToken(&s, KWDESC_PVLEN "=" )) { 	// variable's byte length
				newobject.ex.pv.length = GetIntValue(&s);
				continue;
			}

			if (FindToken(&s,  KWDESC_PVTYPE "=")) {	// variable type
				if (FindToken(&s, KWPVTYPE_BOOLEAN)) {
					newobject.ex.pv.type = BR_BOOL;
					newobject.ex.pv.length = 1;
					newobject.ex.pv.pdatatype = "BOOL";
				} else if (FindToken(&s, KWPVTYPE_UINT8)) {
					newobject.ex.pv.type = BR_USINT;
					newobject.ex.pv.length = 1;
					newobject.ex.pv.pdatatype = "USINT";
				} else if (FindToken(&s, KWPVTYPE_INT8)) {
					newobject.ex.pv.type = BR_SINT;
					newobject.ex.pv.length = 1;
					newobject.ex.pv.pdatatype = "SINT";
				} else if (FindToken(&s, KWPVTYPE_UINT16)) {
					newobject.ex.pv.type = BR_UINT;
					newobject.ex.pv.length = 2;
					newobject.ex.pv.pdatatype = "UINT";
				} else if (FindToken(&s, KWPVTYPE_INT16)) {
					newobject.ex.pv.type = BR_INT;
					newobject.ex.pv.length = 2;
					newobject.ex.pv.pdatatype = "INT";
				} else if (FindToken(&s, KWPVTYPE_UINT32)) {
					newobject.ex.pv.type = BR_UDINT;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "UDINT";
				} else if (FindToken(&s, KWPVTYPE_INT32)) {
					newobject.ex.pv.type = BR_DINT;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "DINT";
				} else if (FindToken(&s, KWPVTYPE_FLOAT32)) {
					newobject.ex.pv.type = BR_REAL;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "REAL";
				} else if (FindToken(&s, KWPVTYPE_FLOAT64)) {
					newobject.ex.pv.type = BR_LREAL;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "LREAL";
				} else if (FindToken(&s, KWPVTYPE_DATI)) {
					newobject.ex.pv.type = BR_DATI;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "DATE_AND_TIME";
				} else if (FindToken(&s, KWPVTYPE_DATE)) {
					newobject.ex.pv.type = BR_DATE;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "DATE";
				} else if (FindToken(&s, KWPVTYPE_TOD)) {
					newobject.ex.pv.type = BR_TOD;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "TOD";
				} else if (FindToken(&s, KWPVTYPE_TIME)) {
					newobject.ex.pv.type = BR_TIME;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "TIME";
				} else if (FindToken(&s, KWPVTYPE_STRUCTURE)) {
					newobject.ex.pv.type = BR_STRUCT;
					if( newobject.ex.pv.pdatatype == NULL )
						newobject.ex.pv.pdatatype = "DATATYPE";
				} else if (FindToken(&s, KWPVTYPE_STRING)) {
					newobject.ex.pv.type = BR_STRING;
					newobject.ex.pv.pdatatype = "STRING";
				} else if (FindToken(&s, KWPVTYPE_WSTRING)) {
					newobject.ex.pv.type = BR_WSTRING;
					newobject.ex.pv.pdatatype = "WSTRING";
				} else {
					newobject.ex.pv.type = 0;	// unknown
					newobject.ex.pv.pdatatype = "UNKNOWN";
					while (*s != ' ' && *s != 0)
						++s;	// ignore rest
				}
				continue;
			}

			if (FindToken(&s, KWDESC_SCOPE "=")) {  // variable scope
				newobject.ex.pv.scope[0] = *s++;
				newobject.ex.pv.scope[1] = 0;
				while (*s != ' ' && *s != 0)
					++s;	// ignore rest
				continue;
			}

			if (FindToken(&s, KWDESC_PVCNT "=")) {	// number of elements
				newobject.ex.pv.dimension = GetIntValue(&s);
				continue;
			}

			if (FindToken( &s, "\t" ) || *(s+1) == 0 ) {	// end of entry or of complete list
				newobject.type = POBJ_PVAR;
				strcpy( newobject.name, object->name );
				strcat( newobject.name, "/" );
				strcat( newobject.name, newobject.descriptor );
				if( object->type == POBJ_CPU ) { // global variable ?
					newobject.ex.pv.task = object;  // since task does not exist we store CPU object instead
					newobject.ex.pv.cpu = object;	// store CPU object
				} else { // local variable
					PVIOBJECT *cpu = object->ex.task.cpu;

					newobject.ex.pv.task = object; // store pointer to task object
					newobject.ex.pv.cpu = cpu;  // store pointer to CPU object
				}
				if( AddPviObject(&newobject, 0) != NULL ) {
					object->gui_info.has_childs = 1; // children were created as separate objects
				}
				memset(&newobject, 0, sizeof(PVIOBJECT));
				continue;
			}

			++s;
		}						/* end of while (*s != 0)... */

	} else {
		AddRowToLog("PviRefreshTaskList(Variablen)", result);
	}

	//Speicher freigeben
	free(pbuffer);
	return (object);

}



/* read struct elements and use them as separate objects */
static PVIOBJECT *PviReadStructElements( PVIOBJECT *object ) {
	char *pbuffer;
	int result;
	char *s, *d;
	PVIOBJECT newobject;
	PVIOBJECT *o;
	BOOL element_definition;

	if( object == NULL )
		return NULL;

	if( object->type != POBJ_PVAR && object->ex.pv.type != BR_STRUCT )
		return NULL;

	pbuffer = (char *)malloc(65535);

	if( pbuffer == NULL ) {
		AddRowToLog("PviReadStructElements:malloc(65535)", -1 );
		return NULL;  // error
	}


	memcpy( &newobject, object, sizeof(PVIOBJECT) );
	strcpy( newobject.descriptor, "CD=\"" );
	strcat( newobject.descriptor, object->descriptor );
	strcat( newobject.descriptor, "\"" );

	result = PviCreate(&newobject.linkid, newobject.name, newobject.type, newobject.descriptor, PviCallback, SET_PVICALLBACK_DATA, 0, "Ev=e");
	if( result == 0 || result == 12002 ) {
		result = PviRead(newobject.linkid, POBJ_ACC_TYPE_EXTERN, NULL, 0, pbuffer, 65535);
	}
	PviDelete( newobject.name );

	if( result == 0 ) {
		//AddRowToLog( pbuffer, 0 );

		s = pbuffer;
		element_definition = FALSE;

		while( *s != 0 ) {
			if( *s == ' ' ) {  
				++s;		//ignore space character
				continue;
			}

			if( !element_definition  ) { // Strukturname des Parents
				if( FindToken(&s, KWDESC_SNAME "=" ) ) {
					d = tempstring;
					while( *s && *s != ' ' ) {
						*d++=*s++;
					}
					*d = 0;
					object->ex.pv.pdatatype = AddPviDatatype(tempstring);
				}
			}

			if( FindToken( &s, "{" ) ) {  // start of element definition
				element_definition = TRUE;
				memset( &newobject, 0, sizeof(PVIOBJECT) );
				memset( tempstring, 0, sizeof(tempstring) );
				d = tempstring;
				while( *s && *s == ' ' ) // ignore space character
					++s;
				while( *s && *s!=' ' )
					*d++ = *s++;
				*d = 0;
				strcpy( newobject.descriptor, object->descriptor );  // create descriptor
				strcat( newobject.descriptor, tempstring );
				strcpy( newobject.name, object->name );		// create name
				strcat( newobject.name, tempstring );
				newobject.type =POBJ_PVAR;
				// count number of points. If > then element is a child
				if( CountToken( tempstring, '.' ) > 1 ) {
					while( !FindToken( &s, "}" ) )
						++s;  // ignore everything
					element_definition = FALSE;
				}
				continue;
			}

			else

				if( element_definition == FALSE ) {
					++s;
					continue;
				}



			if (FindToken(&s, KWDESC_PVLEN "=" )) { 	// variable byte length
				newobject.ex.pv.length = GetIntValue(&s);
				continue;
			}

			if (FindToken(&s, KWDESC_PVCNT "=" )) { 	// number of elements
				newobject.ex.pv.dimension = GetIntValue(&s);
				continue;
			}



			if (FindToken(&s,  KWDESC_PVTYPE "=")) {	// variable type
				if (FindToken(&s, KWPVTYPE_BOOLEAN)) {
					newobject.ex.pv.type = BR_BOOL;
					newobject.ex.pv.length = 1;
					newobject.ex.pv.pdatatype = "BOOL";
				} else if (FindToken(&s, KWPVTYPE_UINT8)) {
					newobject.ex.pv.type = BR_USINT;
					newobject.ex.pv.length = 1;
					newobject.ex.pv.pdatatype = "USINT";
				} else if (FindToken(&s, KWPVTYPE_INT8)) {
					newobject.ex.pv.type = BR_SINT;
					newobject.ex.pv.length = 1;
					newobject.ex.pv.pdatatype = "SINT";
				} else if (FindToken(&s, KWPVTYPE_UINT16)) {
					newobject.ex.pv.type = BR_UINT;
					newobject.ex.pv.length = 2;
					newobject.ex.pv.pdatatype = "UINT";
				} else if (FindToken(&s, KWPVTYPE_INT16)) {
					newobject.ex.pv.type = BR_INT;
					newobject.ex.pv.length = 2;
					newobject.ex.pv.pdatatype = "INT";
				} else if (FindToken(&s, KWPVTYPE_UINT32)) {
					newobject.ex.pv.type = BR_UDINT;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "UDINT";
				} else if (FindToken(&s, KWPVTYPE_INT32)) {
					newobject.ex.pv.type = BR_DINT;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "DINT";
				} else if (FindToken(&s, KWPVTYPE_FLOAT32)) {
					newobject.ex.pv.type = BR_REAL;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "REAL";
				} else if (FindToken(&s, KWPVTYPE_FLOAT64)) {
					newobject.ex.pv.type = BR_LREAL;
					newobject.ex.pv.length = 8;
					newobject.ex.pv.pdatatype = "LREAL";
				} else if (FindToken(&s, KWPVTYPE_DATI)) {
					newobject.ex.pv.type = BR_DATI;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "DATE_AND_TIME";
				} else if (FindToken(&s, KWPVTYPE_DATE)) {
					newobject.ex.pv.type = BR_DATI;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "DATE";
				} else if (FindToken(&s, KWPVTYPE_TOD)) {
					newobject.ex.pv.type = BR_TOD;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "TOD";
				} else if (FindToken(&s, KWPVTYPE_TIME)) {
					newobject.ex.pv.type = BR_TIME;
					newobject.ex.pv.length = 4;
					newobject.ex.pv.pdatatype = "TIME";
				} else if (FindToken(&s, KWPVTYPE_STRUCTURE)) {
					newobject.ex.pv.type = BR_STRUCT;
					if( newobject.ex.pv.pdatatype == NULL )
						newobject.ex.pv.pdatatype = "DATATYPE(STRUCT)";
				} else if (FindToken(&s, KWPVTYPE_STRING)) {
					newobject.ex.pv.type = BR_STRING;
					newobject.ex.pv.pdatatype = "STRING";
				} else if (FindToken(&s, KWPVTYPE_WSTRING)) {
					newobject.ex.pv.type = BR_WSTRING;
					newobject.ex.pv.pdatatype = "WSTRING";
				} else {
					newobject.ex.pv.type = 0;	// unbekannt
					newobject.ex.pv.pdatatype = "UNKNOWN";
					while (*s != ' ' && *s != 0)
						++s;	// Rest ÃƒÆ’Ã†â€™Ãƒâ€ Ã¢â‚¬â„¢ÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â¼berlesen
				}
				continue;
			}

			if( element_definition  && !strcmp(newobject.ex.pv.pdatatype, "DATATYPE") ) { // Strukturname des Elementes
				if( FindToken(&s, KWDESC_SNAME "=" ) ) {
					d = tempstring;
					while( *s && *s != ' ' ) {
						*d++=*s++;
					}
					*d = 0;
					newobject.ex.pv.pdatatype = AddPviDatatype(tempstring);
				}
			}
			if (FindToken(&s, KWDESC_PVCNT "=")) {	// Anzahl der Elemente
				newobject.ex.pv.dimension = GetIntValue(&s);
				continue;
			}

			if (FindToken( &s, "}" ) || *(s+1) == 0 ) {	// Ende des Eintrags oder der gesamten Liste
				PVIOBJECT newelement;
				int i;
//				int j;
				element_definition = FALSE;

				for( i = 0; i < object->ex.pv.dimension; ++i ) {
#ifdef RAUS
					for( j = 0; j < newobject.ex.pv.dimension; ++j ) {
#endif
						memcpy( &newelement, &newobject, sizeof(PVIOBJECT) );
#ifdef RAUS
						newelement.ex.pv.dimension = 1;  // Dimension wird nicht geerbt
#endif

						if( object->ex.pv.dimension > 1 ) { // Elternobject ist ein Array
							strcpy( tempstring, newobject.descriptor+strlen(object->descriptor)+1 ); // Name des Elementes
							snprintf( newelement.name, sizeof(newelement.name), "%s[%i].%s", object->name, i, tempstring );
							snprintf( newelement.descriptor, sizeof(newelement.descriptor), "%s[%i].%s", object->descriptor, i, tempstring );
						}

#ifdef RAUS
						if( newobject.ex.pv.dimension > 1 ) { // Kindobject ist ein Array
							snprintf( tempstring, sizeof(tempstring), "[%i]", j );
							strcat( newelement.name, tempstring );
							strcat( newelement.descriptor, tempstring );
						}
#endif

						strncpy( newelement.ex.pv.scope, object->ex.pv.scope, 1 ); // GÃƒÆ’Ã†â€™Ãƒâ€ Ã¢â‚¬â„¢ÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â¼ltigkeitsbereich wird geerbt
						newelement.ex.pv.task = object->ex.pv.task;  // Task wird geerbt
						newelement.ex.pv.cpu = object->ex.pv.cpu;  // Task wird geerbt
						if( (o = AddPviObject( &newelement, FALSE )) != NULL ) {
							//object->gui_info.has_childs = 1;  // mind. 1 "Kind" wurde fÃƒÆ’Ã†â€™Ãƒâ€ Ã¢â‚¬â„¢ÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â¼r das Objekt angelegt
							//
							//if( o->ex.pv.type == BR_STRUCT ){
							//	o->gui_info.has_childs = 1; // Strukturen haben mind. 1 Kind
							//}

							//sprintf( tempstring, "%s[%i]", object->name, i );
							//if( (o = FindPviObjectByName( tempstring )) != NULL ){
							//	o->gui_info.has_childs = 1;  // Arrayelemente haben mind. 1 "Kind"
							//}

						}
#ifdef RAUS
					} // for( j = 0...
#endif
				} // for (i = 0...
				continue;
			}
			++s;
		}
	}


	if( result ) {
		AddRowToLog( "PviReadStructElements:PviCreate/PviRead", result );
		object = NULL;
	}

	free(pbuffer);
	return object;
}






/* ======================================================================================
   log file handling
   ====================================================================================== */


/* writes into log file */
static void AddRowToLog(char *row, int errcode) {
	FILE *f;
	char zeit[10];
	char datum[10];
	char *pmessage;
	char filename[MAX_PATH];

	strcpy( filename, GetApplicationPath() );
	strcat( filename, "\\" PVILOGFILE );

	_strtime(zeit);
	_strdate(datum);

	f = fopen(filename, "a+");

	if (f != NULL) {
		if (errcode != 0) {
			fprintf(f, "%s - %s : %s --> Error:%i\n", datum, zeit, row, errcode);
		} else {
			fprintf(f, "%s - %s : %s --> OK! \n", datum, zeit, row);
		}
		fclose(f);
	}

	if (errcode != 0) {
		char *errtext;
		pmessage = (char *)malloc(512);

		if (pmessage != NULL) {
			switch( errcode ) {
				case 12050:
					errtext = "PVI-Manager not startet";
					break;
				case 12060:
					errtext = "process timeout";
					break;
				default:
					errtext = "please refer to PVI documentation";
			}
			sprintf(pmessage, "Error in PVI-Interface\nError: %u\n\"%s\"\n(%s)", errcode, errtext, row  );
			MessageBox(NULL, pmessage, "Error", MB_OK | MB_ICONERROR);
		}
		free(pmessage);
	}

}


static void DeleteLog(void) {
	char filename[MAX_PATH];

	strcpy( filename, GetApplicationPath() );
	strcat( filename, "\\" PVILOGFILE );
	remove( filename );
}

/* =====================================================================================
	watch file handling
	====================================================================================
*/


/* writes object to watch file */
int AddToPviWatchFile(PVIOBJECT * object, char *filename) {
	PVIOBJECT *parent;
	char *section;
	int entries;
//	int result;
	int i;



	// store file version info
	WritePrivateProfileString( "File", "Application", "BRWATCH.EXE", filename ); // type
	WritePrivateProfileString( "File", "Version", WATCH_FILE_VERSION, filename ); // version

	parent = GetNextPviObject(TRUE);

	// Eltern suchen und ggfs. speichern
	while (parent != NULL) {
		if ( IsPviObjectParentOf(parent, object) && strrchr(parent->name, '.') == NULL ) {	// find parents
			switch (parent->type) {
				case POBJ_DEVICE:
					section = KWOBJTYPE_DEVICE;
					break;
				case POBJ_CPU:
					section = KWOBJTYPE_CPU;
					break;
				case POBJ_TASK:
					section = KWOBJTYPE_TASK;
					break;
				case POBJ_PVAR:
					section = KWOBJTYPE_PVAR;
					break;
				default:
					section = NULL;
					break;
			}
			if (section != NULL) {
				char keyname[80];
				BOOL found = FALSE;
				tempstring[0] = 0;
				GetPrivateProfileString( section, section, "0", tempstring, sizeof(tempstring), filename );
				entries = atoi(tempstring);
				// does entry exist ?
				for( i = 1; i <= entries; ++i ) {
					sprintf( keyname, "name%u", i );
					GetPrivateProfileString( section, keyname, "", tempstring, sizeof(tempstring), filename );
					if( strcmp( parent->name, tempstring ) == 0 ) {
						found = TRUE;
					}
				}

				if( !found ) { // not found
					++entries;
					sprintf( tempstring, "%u", entries );
					WritePrivateProfileString( section, section, tempstring, filename );
					// Eltern-Objekt speichern
					sprintf( keyname, "name%u", entries );
					WritePrivateProfileString( section, keyname, parent->name, filename ); // name
					sprintf( keyname, "desc%u", entries );
					WritePrivateProfileString( section, keyname, parent->descriptor, filename ); // descriptor
					sprintf( keyname, "sort%u", entries );
					WritePrivateProfileString( section, keyname, "-1", filename ); // sort criteria
				}
			}
		}
		parent = GetNextPviObject(FALSE);
	}

	// Objekt und Sortierkritierium speichern
	switch (object->type) {
		case POBJ_CPU:
			section = KWOBJTYPE_CPU;
			break;
		case POBJ_TASK:
			section = KWOBJTYPE_TASK;
			break;
		case POBJ_PVAR:
			section = KWOBJTYPE_PVAR;
			break;
		default:
			section = NULL;
			break;
	}
	if( section != NULL ) {
		char keyname[80];
		tempstring[0] = 0;
		// number of entries
		GetPrivateProfileString( section, section, "0", tempstring, sizeof(tempstring), filename );
		entries = atoi(tempstring);
		++entries;
		sprintf( tempstring, "%u", entries );
		WritePrivateProfileString( section, section, tempstring, filename );
		// Objekt speichern
		sprintf( keyname, "name%u", entries );
		WritePrivateProfileString( section, keyname, object->name, filename ); // name
		sprintf( keyname, "desc%u", entries );
		WritePrivateProfileString( section, keyname, object->descriptor, filename ); // descriptor
		sprintf( keyname, "sort%u", entries );
		sprintf( tempstring, "%u", object->watchsort );
		WritePrivateProfileString( section, keyname, tempstring, filename ); // sort criteria
	}



	return 0;

}

 
/* load objects of filtered type from watch file 
and returns number of objects read */ 
int LoadPviObjectsFromWatchFile(T_POBJ_TYPE typefilter, char *filename) {
	char *section="";
	PVIOBJECT tempobject, *object, *parent;
	int entries, i, sort;
	int n = 0;  // number of objects to be inserted

	switch( typefilter ) {
		case POBJ_DEVICE:
			section = KWOBJTYPE_DEVICE;
			break;
		case POBJ_CPU:
			section = KWOBJTYPE_CPU;
			break;
		case POBJ_TASK:
			section = KWOBJTYPE_TASK;
			break;
		case POBJ_PVAR:
			section = KWOBJTYPE_PVAR;
			break;

		default:
			break;
	}

	// read number of objects in this section
	tempstring[0] = 0;
	GetPrivateProfileString(section, section, "0", tempstring, sizeof(tempstring), filename);
	entries = atoi(tempstring);
	if (entries == 0) {
		sprintf(tempstring, "No entries found in section [%s] !", section);
		MessageBox(NULL, tempstring, "Error Loading Watch File", MB_OK | MB_ICONERROR);
		return -1;
	}
	for (i = 1; i <= entries; ++i) {
		char keyname[80];

		memset(&tempobject, 0, sizeof(PVIOBJECT));
		// read name
		sprintf(keyname, "name%u", i);
		GetPrivateProfileString(section, keyname, "", tempobject.name, sizeof(tempobject.name), filename);
		if (strlen(tempobject.name) == 0) {
			sprintf(tempstring, "section [%s]\n%s=\ninvalid or not found !", section, keyname);
			MessageBox(NULL, tempstring, "Error Loading Watch File", MB_OK | MB_ICONERROR);
			return -1;
		}
		// read descriptor
		sprintf(keyname, "desc%u", i);
		GetPrivateProfileString(section, keyname, "", tempobject.descriptor, sizeof(tempobject.descriptor), filename);
		if (strlen(tempobject.descriptor) == 0) {
			sprintf(tempstring, "section [%s]\n%s=\ninvalid or not found !", section, keyname);
			MessageBox(NULL, tempstring, "Error Loading Watch File", MB_OK | MB_ICONERROR);
			return -1;
		}
		// read sort criteria
		sprintf(keyname, "sort%u", i);
		GetPrivateProfileString(section, keyname, "-1", tempstring, sizeof(tempstring), filename);
		sort = atoi(tempstring);

		// add object
		tempobject.type = typefilter;
		object = AddPviObject(&tempobject, 1);

		if( object != NULL ) {
			object->gui_info.loaded_from_file = 1;
			object->watchsort = sort;
			if( object->watchsort >= 0 )
				++n;
			PviReadDataType( object );

		}


		// find the corresponding task and CPU
		if( object->type == POBJ_PVAR ) {

			parent = GetNextPviObject(TRUE);
			while (parent != NULL) {
				if ( IsPviObjectParentOf(parent, object) && parent->type == POBJ_CPU ) {	// find parents
					object->ex.pv.cpu = parent;
				}
				parent = GetNextPviObject(FALSE);
			}

			parent = GetNextPviObject(TRUE);
			while (parent != NULL) {
				if ( IsPviObjectParentOf(parent, object) && parent->type == POBJ_TASK ) {	// find parents
					object->ex.pv.task = parent;
				}
				parent = GetNextPviObject(FALSE);
			}


		}


		// find the corresponding CPU
		if( object->type == POBJ_TASK ) {

			parent = GetNextPviObject(TRUE);
			while (parent != NULL) {
				if ( IsPviObjectParentOf(parent, object) && parent->type == POBJ_CPU ) {	// find parents
					object->ex.task.cpu = parent;
				}
				parent = GetNextPviObject(FALSE);
			}
		}


	}
	return n;
}






