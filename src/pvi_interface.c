/*-------------------------------------------------------------------
Module   : pvi_interface.c
 -----------------------------------------------------------------*/

//#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
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

/* globale Variable */
static char tempstring[256]; // schont den Stack :-)
static UINT_PTR timerid_cyclic_requests;
static BOOL PLCDataChangeEvents;  // PLC- Eventbetrieb

/* Liste fuer PVI- Objekte */
static PVIOBJECT pvirootobject;
static PVIOBJECT *readpointer_pviobject; // zum Auslesen der Pvi- Objektliste

/* Liste fuer Strukturnamen */
static PVISTRUCTNAMES structnamesroot = { "DATATYPE", NULL };


/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: ReadSettings
 Created  : Wed Feb  1 11:01:24 2006
 Modified : Mon Feb  6 15:35:56 2006

 Synopsys : liest INI-Datei

 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void ReadSettings(void)
{
    PVIOBJECT tempobject, *object;
    int i;
    char section[20];
    char key[20];
    char buffer[80];

    memset(&tempobject, 0, sizeof(PVIOBJECT));

    sprintf( section, "General" );
    sprintf( key, "PLCDataChangeEvents" );
    strcpy( buffer, "" );
    GetPrivateProfileString(section, key, "", buffer, sizeof(buffer), SettingsGetFileName());
    PLCDataChangeEvents = atoi(buffer);



    /* create ethernet device */
    strcpy(tempobject.descriptor, "/IF=tcpip /SA=99");
    CreateUniqueObjectName(tempstring, tempobject.descriptor);
    sprintf(tempobject.name, "@Pvi/LN/DEV%s", tempstring);
    tempobject.type = POBJ_DEVICE;
    AddPviObject(&tempobject, TRUE);



    // read devices from brwatch.ini
    for (i = 1; i <= 16; ++i)
    {
        sprintf(section, "DEVICE%u", i);
        strcpy(key, "broadcast");
        GetPrivateProfileString(section, key, "255.255.255.255", buffer, sizeof(buffer), SettingsGetFileName());
        tempobject.ex.dev.broadcast = inet_addr(buffer);

        strcpy(key, "name");
        strcpy( buffer, "" );
        GetPrivateProfileString(section, key, "", buffer, sizeof(buffer), SettingsGetFileName());

        strcpy(key, "descriptor");
        strcpy( buffer, "" );
        GetPrivateProfileString(section, key, "", buffer, sizeof(buffer), SettingsGetFileName());
        if (strlen(buffer) != 0)
        {
            strcpy(tempobject.descriptor, buffer);
            CreateUniqueObjectName(tempstring, tempobject.descriptor);
            sprintf(tempobject.name, "@Pvi/LN/DEV%s", tempstring);
            tempobject.type = POBJ_DEVICE;
            AddPviObject(&tempobject, TRUE);
        }


    }

    // read CPUs from brwatch.ini
    for (i = 1; i <= 255; ++i)
    {
        char devicename[256];

        memset(&tempobject, 0, sizeof(tempobject));
        sprintf(section, "CPU%u", i);
        strcpy(key, "descriptor");
        strcpy( buffer, "" );
        GetPrivateProfileString(section, key, "", buffer, sizeof(buffer), SettingsGetFileName());
        strcpy(tempobject.descriptor, buffer);

        strcpy(key, "device");
        strcpy( buffer, "" );
        GetPrivateProfileString(section, key, "", buffer, sizeof(buffer), SettingsGetFileName());
        if (strlen(buffer) != 0)
        {
            // Devicenamen zusammenbasteln
            CreateUniqueObjectName(tempstring, buffer);
            sprintf(devicename, "@Pvi/LN/DEV%s", tempstring);

            // Device mit dem angegebenen Namen suchen
            object = GetNextPviObject(TRUE);
            while ((object = GetNextPviObject(FALSE)) != NULL)
            {
                if (strcmp(object->name, devicename) == 0)  	// Device gefunden
                {
                    // Objektnamen erzeugen
                    CreateUniqueObjectName(tempstring, tempobject.descriptor);
                    sprintf(tempobject.name, "%s/CPU%s", devicename, tempstring);
                    tempobject.type = POBJ_CPU;
                    AddPviObject(&tempobject, TRUE);
                }
            }
        }
        else
        {
            // sprintf( tempstring, "Section %s: device ""%s"" not found !", section, buffer );
            // MessageBox( NULL, tempstring, "Loading settings...", MB_ICONERROR | MB_OK );
        }

    }


}


/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: CreateDefiniteObjectName
 Created  : Thu Mar 16 11:03:56 2006
 Modified : Thu Mar 16 11:03:56 2006

 Synopsys : erzeugt einen eindeutigen Objektname aus dem Descriptor
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void CreateUniqueObjectName( char *name, char *descriptor )
{
    while( *descriptor )
    {
        if( *descriptor == ' ' )
            ++descriptor;
        else if( *descriptor == '/' )
        {
            *name++ = '#';
            ++descriptor;
        }
        else if( *descriptor == '.' )
        {
            *name ++ = '_';
            ++descriptor;
        }
        else
        {
            *name++ = toupper(*descriptor++);
        }
    }
    *name = 0;
}



/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: AddPviObject
 Created  : Wed Feb  1 13:13:31 2006
 Modified : Thu Feb  2 10:30:23 2006

 Synopsys : Objekt zur Liste hinzufuegen
 Input    : Zeiger auf das temporäre Objekt, von welchem die Parameter
            kopiert werden
 Output   : Zeiger auf das neu eingefügte Objekt
 Errors   : NULL
 ------------------------------------------------------------------@@-@@-*/

PVIOBJECT *AddPviObject(PVIOBJECT * tempobject, BOOL create )
{

    PVIOBJECT *lastobject;
    PVIOBJECT *newobject;
    char descriptor[256];
    int result = 0;
    BOOL already_exist;   // Objekt existiert schon
    char *event_mask;

    already_exist = FALSE;

    if (tempobject == NULL) 			// Objekt gueltig ?
        return NULL;

    // das letzte Objekt in der Liste suchen
    lastobject = &pvirootobject;
    while (lastobject->next != NULL)
    {
        lastobject = lastobject->next;
        if (!strcmp(tempobject->name, lastobject->name) )  	// Objekt schon vorhanden ?
        {
            already_exist = TRUE;
            break;
        }
    }


    // Speicher fuer neues Objekt erzeugen
    if (already_exist == TRUE)
    {
        newobject = lastobject;
    }
    else
    {
        if ((newobject = (PVIOBJECT *)malloc(sizeof(PVIOBJECT))) != NULL)
        {
            memcpy( newobject, tempobject, sizeof(PVIOBJECT) ); // erst mal alles kopieren
            memset( &newobject->gui_info, 0, sizeof(newobject->gui_info) );
            newobject->gui_info.display_as_decimal = 1;
            newobject->gui_info.display_as_hex = 0;
            newobject->gui_info.display_as_binary = 0;
            newobject->gui_info.display_as_char = 0;
            newobject->watchsort = -1;
            newobject->next = NULL;	// Zeiger auf naechstes Object
            lastobject->next = newobject;	// neues Objekt in Liste einhaengen

        }
        else
        {
            AddRowToLog("AddPviObject:malloc()", -1);
            return NULL;
        }
    }

    // ggf eine Variable auch im Pvi- Manager anlegen
    if (create)
    {
        strcpy(descriptor, "CD=\"");
        strcat(descriptor, newobject->descriptor);
        strcat(descriptor, "\"");
        event_mask = "Ev=e";
        if( newobject->type == POBJ_PVAR )
        {
            if( PLCDataChangeEvents && newobject->ex.pv.dimension <= 1 && newobject->ex.pv.type != BR_STRUCT && newobject->ex.pv.type != BR_STRING)
            {
                strcat(descriptor, " RF=50 AT=rwe" );
            }
            else
            {
                strcat(descriptor, " RF=50 " );
            }
            event_mask = "Ev=ed";
        }

        result = PviCreate(&newobject->linkid, newobject->name, newobject->type, descriptor, PviCallback, SET_PVICALLBACK_DATA, 0, event_mask );

        if( result )
        {
            memset( tempstring, 0, sizeof(tempstring) );
            sprintf(tempstring, "AddPviObject:PviCreate \"%s\"", tempobject->name);
            AddRowToLog(tempstring, result);
            return NULL;
        }
        else
        {
            if( newobject->type == POBJ_PVAR )
            {
                PviReadRequest( newobject->linkid, POBJ_ACC_DATA, PviCallback, SET_PVICALLBACK_DATA, 0 );
                // read variable once
            }
        }
    }

    return newobject;

}

/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: UnlinkPviObject
 Created  : Mon Feb 20 13:19:43 2006
 Modified : Mon Feb 20 13:19:43 2006

 Synopsys : Entfernt den Link zum Ojbekt
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static PVIOBJECT *UnlinkPviObject( PVIOBJECT *object )
{
    PVIOBJECT *find;
    int result;

    if( object == NULL )
        return NULL;

    GetNextPviObject(TRUE);

    while( (find = GetNextPviObject(FALSE)) != NULL )
    {
        if( find == object )
        {
            result = PviUnlink( object->linkid );
            if( result == 0 )
            {
                object->linkid =0;
                return object;
            }
            else
            {
                AddRowToLog( "UnlinkPviObject", result );
                return NULL;
            }
        }
    }
    return NULL;
}

/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: RemovePviObject
 Created  : Thu Feb  2 10:11:25 2006
 Modified : Thu Feb  2 10:28:42 2006

 Synopsys : Objekt aus Liste löschen
 Input    : object: Zeiger auf das Objekt, welches gelöscht wird
 Output   : Zeiger auf das Objekt, welches der Vorgänger war/ist
 Errors   :
 ------------------------------------------------------------------@@-@@-*/
static PVIOBJECT *RemovePviObject(PVIOBJECT * object )
{
    PVIOBJECT *prevobject, *find;

    prevobject = &pvirootobject;

    // Vorgaenger suchen...
    while( prevobject->next != NULL )
    {
        find = prevobject->next;

        //if( !strcmp( find->name, object->name ) ) {
        if( find == object )
        {
            PviUnlink(object->linkid);	// Pvi- Objekt unlinken
            object->linkid = 0;
            // Fehler ignorieren wir erst mal, da der PVI evtl.
            // Child-Objekte schon geloescht haben kann
            prevobject->next = object->next;	// Luecke in verketter Liste schliessen


            if( object->type == POBJ_PVAR && object->ex.pv.pvalue != NULL )  // evtl. Speicher für Watchdaten killen
            {
                free(object->ex.pv.pvalue);
                object->ex.pv.pvalue = NULL;
            }
            free(object);					  	// Speicher freigeben
            return(prevobject);
        }
        prevobject = prevobject->next;		  	// naechstes Objekt
    }

    return( prevobject );
}

/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: FindPviObjectByLinkId
 Created  : Fri Feb  3 10:10:45 2006
 Modified : Fri Feb  3 10:10:45 2006

 Synopsys : sucht ein Objekt anhand der Link- Id
 Input    : PVI- Link- Id von PviLink()
 Output   : das Objekt
 Errors   : NULL= nicht gefunden
 ------------------------------------------------------------------@@-@@-*/

static PVIOBJECT *FindPviObjectByLinkId( DWORD linkid )
{
    PVIOBJECT *object;

    object = &pvirootobject;
    while( object->next != NULL ) 	// List bis Ende durchsuchen;
    {
        object = object->next;
        if( object->linkid == linkid )
        {
            return object;
        }
    }
    return NULL;	// Objekt nicht gefunden
}

/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: FindPviObjectByName
 Created  : Thu Feb  9 17:06:10 2006
 Modified : Thu Feb  9 17:06:10 2006

 Synopsys : sucht das Objekt mit dem angegebenen Namen
 Input    : Name des gesuchten Objektes
 Output   :
 Errors   : NULL
 ------------------------------------------------------------------@@-@@-*/

PVIOBJECT *FindPviObjectByName( char *name )
{
    PVIOBJECT *object;

    if( name != NULL )
    {
        object = &pvirootobject;
        while( object->next != NULL ) 	// Liste bis Ende durchsuchen;
        {
            object = object->next;
            if( strcmp( object->name, name ) == 0 )
            {
                return object;
            }
        }
    }
    return NULL;	// Objekt nicht gefunden
}

/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: WatchPviObject
 Created  : Mon Feb 20 13:37:04 2006
 Modified : Mon Feb 20 13:37:04 2006

 Synopsys : nimmt das Object in die Watchliste auf

 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

PVIOBJECT *WatchPviObject(PVIOBJECT * object, BOOL watch)
{
    if (object != NULL)
    {
        switch (object->type)
        {

        case POBJ_PVAR:
            if (watch)
            {
                object = AddPviObject( object, TRUE );
                if( object != NULL )
                {
                    object->ex.pv.pvalue = malloc(object->ex.pv.length); // Speicher für Daten allokieren
                    if( object->ex.pv.pvalue )
                    {
                        if( object->ex.pv.type == BR_REAL )
                        {
                            * ((float*)object->ex.pv.pvalue) = 0.0;  /* PV-Wert initialisieren */
                        }
                        else if( object->ex.pv.type == BR_LREAL )
                        {
                            * ((double*)object->ex.pv.pvalue) = 0.0;  /* PV-Wert initialisieren */
                        }
                        else
                        {
                            memset( object->ex.pv.pvalue, 0, object->ex.pv.length); /* PV-Wert initialisieren */
                        }
                    }
                }
                return( object );
            }
            else
            {
                UnlinkPviObject(object);
                object->watchsort = -1;
                if( object->ex.pv.pvalue != NULL )
                {
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



/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: GetNextPviOjbect
 Created  : Thu Feb  2 10:33:25 2006
 Modified : Thu Feb  9 15:11:32 2006

 Synopsys : ergibt das nächste Objekt in der Liste
 Input    : first: 1= das erste Object

 Output   : das nächste Objekt der Liste
 Errors   : NULL
 ------------------------------------------------------------------@@-@@-*/

PVIOBJECT* GetNextPviObject( BOOL first  )
{
    if( first )
        return( readpointer_pviobject = &pvirootobject );


    if( readpointer_pviobject != NULL )
    {
        if( readpointer_pviobject->next != NULL )
        {
            readpointer_pviobject = readpointer_pviobject->next;
            return( readpointer_pviobject );
        }
    }
    return NULL;
}



/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: FindPviChildObject
 Created  : Thu Feb  9 15:12:47 2006
 Modified : Thu Feb  9 15:12:47 2006

 Synopsys : sucht nach den Kindern des angegebenen Objektes
 Input    : object:
            first: 1 = das erste Kind
 Output   : Zeiger auf das Kind
 Errors   : NULL
 ------------------------------------------------------------------@@-@@-*/
PVIOBJECT *FindPviChildObject(PVIOBJECT * object, BOOL first )
{
    static PVIOBJECT *find=NULL;
    size_t length;

    if (object != NULL)
    {
        if( first )
            find = &pvirootobject;

        length =  strlen(object->name);

        while (find->next != NULL)
        {
            find = find->next;
            if (strncmp(object->name, find->name, length ) == 0)
            {
                if( find->name[length] == '/' || find->name[length] == '[' || find->name[length] == '.' )
                    if( strrchr( find->name+length+1, '.' )== NULL &&
                            strrchr( find->name+length+1, '/' )== NULL )  // keine Enkel
                        return (find);
            }
        }
    }

    return NULL;
}

/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: IsPviObjectParentOf
 Created  : Wed Mar  8 09:17:11 2006
 Modified : Wed Mar  8 09:17:11 2006

 Synopsys : ermittelt, ob parent und child eine Eltern/-Kind Beziehung
            haben
 Input    :
 Output   : TRUE, FALSE
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

BOOL IsPviObjectParentOf( PVIOBJECT *parent, PVIOBJECT * child )
{
    size_t length;


    if( parent == NULL || child == NULL )
        return FALSE;

    length = strlen(parent->name);

    if( strncmp( parent->name, child->name, length ) == 0 )
    {
        if( parent != child )
            return TRUE;
    }
    return FALSE;

}

/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: GetNumberOfPviObjects
 Created  : Fri Feb  3 14:39:54 2006
 Modified : Thu Feb  9 17:07:01 2006

 Synopsys : Gibt die Anzahl der eingetragenen Objekte zurueck
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

int GetNumberOfPviObjects( void )
{
    PVIOBJECT *p;
    int i=0;

    p =pvirootobject.next;

    while( p != NULL )
    {
        ++i;
        p = p->next;
    }
    return(i);
}


/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: Pvi_GlobalEvent
 Created  : Wed Mar  1 11:42:28 2006
 Modified : Wed Mar  1 11:42:28 2006

 Synopsys : Globale PVI- Events
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/
static void WINAPI Pvi_GlobalEvent(WPARAM wParam, LPARAM lParam, LPVOID pData, DWORD DataLen, T_RESPONSE_INFO * pInfo)
{
    PVIOBJECT po, *ppo;


    switch (pInfo->nType)
    {
    case POBJ_EVENT_PVI_CONNECT:

        break;

    case POBJ_EVENT_PVI_DISCONN:
        AddRowToLog( "POBJ_EVENT_PVI_DISCONN", -1 );
        break;

    case POBJ_EVENT_PVI_ARRANGE:
        {
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
        strcpy( po.name, "@Pvi/LN");
        if( g_ansl )
        {
            strcpy( po.descriptor,"LnAnsl");
        }
        else
        {
            strcpy( po.descriptor,"LnIna2");
        }
        po.type = POBJ_LINE;
        AddPviObject( ppo, 1 );
        ReadSettings();

        if( pvi_interface_notify.cbobjects_valid != NULL )
        {
            pvi_interface_notify.cbobjects_valid();
        }
        break;
    }

}


/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: PviCallback
 Created  : Wed Feb  1 12:09:58 2006
 Modified : Wed Feb  1 12:09:58 2006

 Synopsys : Callback

 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void WINAPI PviCallback (WPARAM wParam, LPARAM lParam, LPVOID pData, DWORD DataLen, T_RESPONSE_INFO* pInfo)
{
    PVIOBJECT *object;
    static char value[128];
    char *s, *d;

    switch( pInfo->nType )
    {
    case POBJ_EVENT_DATA:			// Antwort auf Datenaenderung
        if( DataLen == 0 )			// keine Antwortdaten vorhanden
            return;
        object = FindPviObjectByLinkId(pInfo->LinkID);
        if( object != NULL )
        {
            if( object->type == POBJ_PVAR )
            {
                switch( object->ex.pv.type )
                {
                case BR_STRING:
//							if( (int) DataLen > object->ex.pv.length )
//								(int) DataLen = object->ex.pv.length;
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
                if( object->error )  // Fehlerstatus zurücksetzen
                {
                    object->error = 0;
                    if( pvi_interface_notify.cberror_changed != NULL )
                        pvi_interface_notify.cberror_changed( object, 0 );
                }
                if( pvi_interface_notify.cbdata_changed != NULL )
                    pvi_interface_notify.cbdata_changed( object );

                if( object->ex.pv.pvalue != NULL )
                {
                    object->ex.pv.value_changed = 1; /* Wert hat sich geändert */
                    LoggerDataChanged( object );
                }

            }
        }
        break;


    case POBJ_EVENT_ERROR:			// Aenderung des Fehlerstatus
        object = FindPviObjectByLinkId(pInfo->LinkID);
        if( object != NULL )
        {
            object->error = pInfo->ErrCode;

            if( pvi_interface_notify.cberror_changed != NULL )
                pvi_interface_notify.cberror_changed( object, pInfo->ErrCode );
        }

        break;


    case POBJ_ACC_STATUS:			// Aenderung des Status
        if( pInfo->ErrCode != 0 )
            return;
        object = FindPviObjectByLinkId(pInfo->LinkID);
        if( object != NULL )
        {
            if( pvi_interface_notify.cbdata_changed != NULL )
            {
                s = (char*) pData;
                d = value;

                *d = 0;
                while( *s )
                {
                    if( FindToken( &s, "ST=" ))
                    {
                        while( (*d++ = *s++) != 0 );
                        *d = 0;
                        break;
                    }
                    ++s;
                }


                switch( object->type )
                {
                case POBJ_CPU:
                    strncpy( object->ex.cpu.status, value, sizeof(object->ex.cpu.status)-1);
                    if( strcmp( value, KWSTATUS_WARMSTART ) == 0 || strcmp( value, KWSTATUS_COLDSTART ) == 0 )
                    {
                        object->ex.cpu.running = TRUE;
                    }
                    else
                    {
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


    case POBJ_ACC_DATE_TIME:		// Datum/Uhrzeit lesen
        object = FindPviObjectByLinkId(pInfo->LinkID);
        if( object != NULL )
        {
            if( object->type == POBJ_CPU )
            {
                if( DataLen > sizeof(struct tm) )
                    DataLen = sizeof(struct tm);
                memcpy( &object->ex.cpu.rtc_time, pData, DataLen );
                // einen Notify sparen wir uns, da dies schon POBJ_ACC_STATUS macht...
            }
        }
        break;
    }

}

/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: PviCyclicRequests
 Created  : Tue Mar 14 10:00:02 2006
 Modified : Tue Mar 14 10:00:02 2006

 Synopsys : wird benutzt, um zyklische Requests abzusetzen, da nicht für
            alle Zugriffsarten Events unterstützt werden
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void CALLBACK PviCyclicRequests( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime )
{
    PVIOBJECT *object;
    static int zyklus_readpv = 0;
    static int zyklus_getlicenseinfo = 0;

    if( ++zyklus_readpv > 4 )
    {
        zyklus_readpv = 0;
    }

    // Pvi- Lizenzinformation lesen
    if( zyklus_getlicenseinfo < 10)
    {
        if( ++zyklus_getlicenseinfo >= 10 )
        {
            if( GetPviLicenceInfo() == PVIWORK_STATE_LOCKED )
            {
                MessageBox( MainWindowGetHandle(), "Sorry PVI is locked.\nProgram must shut down !", "Error Pvi License", MB_ICONERROR );
                SendMessage( MainWindowGetHandle(), WM_CLOSE, 0, 0 );
            }
            else
            {
                zyklus_getlicenseinfo = 0;
            }
        }
    }

    object = GetNextPviObject( TRUE );

    while( (object = GetNextPviObject(FALSE)) != NULL )
    {
        if( object->linkid == 0 || object->error != 0 )
            continue;

        switch( object->type )
        {
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


static int GetPviLicenceInfo( void )
{
    DWORD LinkId;
    long result;
    T_PVI_INFO_LICENCE license;

    memset( &license, 0, sizeof(license ) );
    result = PviLink( &LinkId, "@Pvi", PVI_HMSG_NIL, 0, 0, 0 );

    if( result == 0 )
    {
        result = PviRead( LinkId, POBJ_ACC_INFO_LICENCE, 0, 0, &license, sizeof(license) );
    }

    if( result == 12059 )
    {
        return PVIWORK_STATE_LOCKED;
    }

    else if( result != 0 )
    {
        AddRowToLog( "GetPviLicenceInfo()", result );
    }

    PviUnlink(LinkId);

    return license.PviWorkState[0];
}



/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: StartPvi
 Created  : Wed Feb  1 12:09:58 2006
 Modified : Wed Feb  1 12:09:58 2006

 Synopsys : Pvi starten

 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

int StartPvi( void )
{
    int result;

    memset( &pvirootobject, 0, sizeof(PVIOBJECT) );
    strcpy( pvirootobject.name, "@Pvi");
    strcpy( pvirootobject.descriptor, "");
    pvirootobject.type = POBJ_PVI;
    pvirootobject.next = NULL;



    DeleteLog(); // Pvi- Logdatei loeschen

    // PVI- Version in Logdatei schreiben
    strcpy( g_PVIVersionString, "" );
    PviGetVersion( g_PVIVersionString, sizeof(g_PVIVersionString) );
    AddRowToLog( g_PVIVersionString, 0 );

    // Pvi starten
    {
        char buffer[80];

        memset( buffer, 0, sizeof(buffer) );
        GetPrivateProfileString( "REMOTE", "parameter", "COMT=10 AS=1 PT=20", buffer, sizeof(buffer), SettingsGetFileName());

        result = PviInitialize( 0, 0, buffer, NULL );
    }

    if( result )
    {
        AddRowToLog( "PviInitialize", result );
        return result;
    }




    // globale Ereignisse setzen
    result = PviSetGlobEventMsg( POBJ_EVENT_PVI_ARRANGE, Pvi_GlobalEvent, SET_PVICALLBACK_DATA, 0 );

    if( result )
    {
        AddRowToLog( "PviSetGlobEventMsg", result );
        return result;
    }


    // Timer für zyklische Requests
    timerid_cyclic_requests = SetTimer( NULL, 0, 500, (TIMERPROC) PviCyclicRequests );


    return 0; // Alles OK
}



/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: StopPvi
 Created  : Wed Feb  1 12:37:01 2006
 Modified : Wed Feb  1 12:37:01 2006

 Synopsys : Pvi stoppen
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

int StopPvi(void)
{

    int result;
    PVIOBJECT *prevobject;

    // Timer für zyklische Requests killen
    KillTimer( NULL, timerid_cyclic_requests );

    // alle Objekte loeschen
    prevobject = &pvirootobject;
    while (prevobject->next != NULL)
    {
        prevobject = RemovePviObject(prevobject->next);
    }


    result = PviDeinitialize();

    // alle Datentypen loeschen
    AddPviDatatype( NULL );

    AddRowToLog("PviDeinitialize", result);
    return result;
}


/* =================================================================================
   Funktionen zur Verwaltung von Datentyp
   ==================================================================================
*/


static char *AddPviDatatype( char * t )
{
    size_t length;
    PVISTRUCTNAMES *p, *kill;

    if( t == NULL )   // alle Datentypen löschen ...
    {
        p = structnamesroot.next	;
        while( p != NULL )
        {
            kill = p;
            p = kill->next;
            if( kill->pname != NULL )
                free(kill->pname);
            free(kill);
        }
        structnamesroot.next = NULL;
        return ("nil!");
    }

    length = strlen(t); // Länge des neuen Datentypnames

    // Datentypen suchen
    p = &structnamesroot;
    while( p->next != NULL )
    {
        p = p->next;
        if( p->pname != NULL )
        {
            if( !strcmp( p->pname, t ) )   // Datentyp existiert bereits
            {
                return( p->pname );
            }
        }
        else
        {
            return ("ERROR!");
        }
    }

    // Datentyp neu eintragen
    p->next = malloc( sizeof(PVISTRUCTNAMES) );
    if( p->next != NULL )
    {
        p = p->next;
        p->pname = malloc( length + 1); // Speicher für Name
        p->next = NULL;
        if( p->pname != NULL )
        {
            return( strcpy( p->pname, t ) );
        }
    }


    return ("ERROR!");
}

/* ===================================================================================
	Funktionen zum Extrahieren von Informationen über PVI- Objekte
	==================================================================================
*/


/* Ermittelt den Datentyp (aus der SPS) */
static void PviReadDataType( PVIOBJECT *object )
{
    char buffer[256];
    int result;
    char *s;

    result = PviRead( object->linkid, POBJ_ACC_TYPE_EXTERN, NULL, 0, buffer, sizeof(buffer) );
    if( result == 0 )
    {
        s = buffer;

        while ( *s )
        {
            if (*s == ' ')  	// Leerzeichen
            {
                ++s;			// Überlesen
                continue;
            }

            if( FindToken( &s, "{" ) )   // Beginn einer Strukturdefinition
            {
                break;
            }

            if (FindToken(&s, KWDESC_PVLEN "=" ))   	// Länge der Variable
            {
                object->ex.pv.length = GetIntValue(&s);
                continue;
            }

            if (FindToken(&s,  KWDESC_PVTYPE "="))  	// Typ der Variable
            {
                if (FindToken(&s, KWPVTYPE_BOOLEAN))
                {
                    object->ex.pv.type = BR_BOOL;
                    object->ex.pv.length = 1;
                    object->ex.pv.pdatatype = "BOOL";
                }
                else if (FindToken(&s, KWPVTYPE_UINT8))
                {
                    object->ex.pv.type = BR_USINT;
                    object->ex.pv.length = 1;
                    object->ex.pv.pdatatype = "USINT";
                }
                else if (FindToken(&s, KWPVTYPE_INT8))
                {
                    object->ex.pv.type = BR_SINT;
                    object->ex.pv.length = 1;
                    object->ex.pv.pdatatype = "SINT";
                }
                else if (FindToken(&s, KWPVTYPE_UINT16))
                {
                    object->ex.pv.type = BR_UINT;
                    object->ex.pv.length = 2;
                    object->ex.pv.pdatatype = "UINT";
                }
                else if (FindToken(&s, KWPVTYPE_INT16))
                {
                    object->ex.pv.type = BR_INT;
                    object->ex.pv.length = 2;
                    object->ex.pv.pdatatype = "INT";
                }
                else if (FindToken(&s, KWPVTYPE_UINT32))
                {
                    object->ex.pv.type = BR_UDINT;
                    object->ex.pv.length = 4;
                    object->ex.pv.pdatatype = "UDINT";
                }
                else if (FindToken(&s, KWPVTYPE_INT32))
                {
                    object->ex.pv.type = BR_DINT;
                    object->ex.pv.length = 4;
                    object->ex.pv.pdatatype = "DINT";
                }
                else if (FindToken(&s, KWPVTYPE_FLOAT32))
                {
                    object->ex.pv.type = BR_REAL;
                    object->ex.pv.length = 4;
                    object->ex.pv.pdatatype = "REAL";
                }
                else if (FindToken(&s, KWPVTYPE_FLOAT64))
                {
                    object->ex.pv.type = BR_LREAL;
                    object->ex.pv.length = 8;
                    object->ex.pv.pdatatype = "LREAL";
                }
                else if (FindToken(&s, KWPVTYPE_DATI))
                {
                    object->ex.pv.type = BR_DATI;
                    object->ex.pv.length = 4;
                    object->ex.pv.pdatatype = "DATE_AND_TIME";
                }
                else if (FindToken(&s, KWPVTYPE_DATE))
                {
                    object->ex.pv.type = BR_DATE;
                    object->ex.pv.length = 4;
                    object->ex.pv.pdatatype = "DATE";
                }
                else if (FindToken(&s, KWPVTYPE_TIME))
                {
                    object->ex.pv.type = BR_TIME;
                    object->ex.pv.length = 4;
                    object->ex.pv.pdatatype = "TIME";
                }
               else if (FindToken(&s, KWPVTYPE_TOD))
                {
                    object->ex.pv.type = BR_TOD;
                    object->ex.pv.length = 4;
                    object->ex.pv.pdatatype = "TOD";
                }
                else if (FindToken(&s, KWPVTYPE_STRUCTURE))
                {
                    object->ex.pv.type = BR_STRUCT;
                    if( object->ex.pv.pdatatype == NULL )
                        object->ex.pv.pdatatype = "DATATYPE";
                }
                else if (FindToken(&s, KWPVTYPE_STRING))
                {
                    object->ex.pv.type = BR_STRING;
                    object->ex.pv.pdatatype = "STRING";
                }
                else
                {
                    object->ex.pv.type = 0;	// unbekannt
                    object->ex.pv.pdatatype = "UNKNOWN";
                    while (*s != ' ' && *s != 0)
                        ++s;	// Rest überlesen
                }
                continue;
            }

            if (FindToken(&s, KWDESC_SCOPE "="))    // Gültigkeitsbereich der Variable
            {
                object->ex.pv.scope[0] = *s++;
                object->ex.pv.scope[1] = 0;
                while (*s != ' ' && *s != 0)
                    ++s;	// Rest überlesen
                continue;
            }

            if (FindToken(&s, KWDESC_PVCNT "="))  	// Anzahl der Elemente
            {
                object->ex.pv.dimension = GetIntValue(&s);
                continue;
            }
            ++s;
        } /* Ende while()*/

        if( object->ex.pv.type == BR_STRUCT )
        {
            //PviReadStructElements( object );
        }

    }
}


/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: ExpandPviObject
 Created  : Thu Feb 16 08:39:19 2006
 Modified : Thu Feb 16 08:39:19 2006

 Synopsys : legt die Kinder des Objektes als eigene Objekte an
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

PVIOBJECT *ExpandPviObject( PVIOBJECT * object )
{
    int i;
    PVIOBJECT *tempobject;

    if( object == NULL )
        return NULL;

    tempobject = malloc(sizeof(PVIOBJECT));

    if( tempobject != NULL )
    {

        if( object->gui_info.has_childs == 0 )
        {
            switch( object->type )
            {
            case POBJ_PVAR: 	/* Prozessvariable */
                if( object->ex.pv.dimension > 1 )   // Array ?
                {

                    if( object->ex.pv.type == BR_STRUCT )  // Array von Strukturen ?
                    {
                        PviReadStructElements( object ); // nur einmal Strukturdefinition holen
                    }

                    // Alle Array-elemente anlegen
                    for( i = 0; i < object->ex.pv.dimension; ++i )
                    {
                        memcpy( tempobject, object, sizeof(PVIOBJECT) );
                        tempobject->ex.pv.dimension = 1;
                        sprintf( tempobject->name, "%s[%i]", object->name, i );
                        sprintf( tempobject->descriptor, "%s[%i]", object->descriptor, i );
                        if( AddPviObject( tempobject, FALSE ) != NULL )
                        {
                            object->gui_info.has_childs = 1;  // mind. 1 "Kind" wurde angelegt
                        }
                    }

                }

                else

                    if( object->ex.pv.type == BR_STRUCT )
                    {
                        if( PviReadStructElements( object ) != NULL )
                        {
                            object->gui_info.has_childs = 1;
                        }
                    }
                break;

            case POBJ_CPU:
                if( PviReadTaskList(object) != NULL )
                {
                    object->gui_info.has_childs = 1;
                }
                object->gui_info.has_childs = 0;
                break;

            case POBJ_TASK:
                if( PviReadPvarList(object) == 0 )
                {
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





/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: PviReadCPUList
 Created  : Wed Feb 22 08:17:03 2006
 Modified : Tue Feb 28 12:48:28 2006

 Synopsys : ermittelt die erreichbaren CPUs
 Input    : Zeiger auf Device- Objekt
 Output   : Zeiger auf Device- Objekt
 Errors   : NULL
 ------------------------------------------------------------------@@-@@-*/
static PVIOBJECT *PviReadCPUList(PVIOBJECT *object )
{
    PVIOBJECT *cpuobject;
    char *s;
    char descriptor[256];
    int result;
    char *pbuffer;

    if( object == NULL )
        return NULL;

    if( object->type != POBJ_DEVICE )
        return NULL;


    pbuffer = malloc(65535);
    if( pbuffer == NULL )
    {
        AddRowToLog( "PviReadCPUList():malloc(65535)", -1 );
        return NULL;
    }


    cpuobject = malloc(sizeof(PVIOBJECT) );
    if( cpuobject == NULL )
    {
        free(pbuffer);
        AddRowToLog( "PviReadCPUList():malloc(PVIOBJECT)", -1 );
        return NULL;
    }


    s= _strupr(object->descriptor);
    while( *s )
    {
        // search for CPUs connected to a serial port
        if( FindToken( &s, "/IF=COM" ) )
        {
            memset( cpuobject, 0, sizeof(PVIOBJECT) );
            //strcpy( cpuobject->descriptor, "/RT=10000" );

            CreateUniqueObjectName( tempstring, cpuobject->descriptor );
            sprintf( cpuobject->name, "%s/CPU%s", object->name, tempstring  );

            cpuobject->type = POBJ_CPU;
            sprintf( descriptor, "CD=\"%s\"", cpuobject->descriptor );

            result = PviCreate(&cpuobject->linkid, cpuobject->name, cpuobject->type, descriptor, PviCallback, SET_PVICALLBACK_DATA, 0, "Ev=e" );
            if( result == 0 )
            {
                result = PviRead(cpuobject->linkid, POBJ_ACC_VERSION, NULL, 0, pbuffer, 65535 );
                if( result == 0 )
                {
                    if( AddPviObject( cpuobject, 1 ) == NULL )
                    {
                        object = NULL; //Fehler
                    }
                    break;
                }
            }
            switch( result )
            {
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
        if( FindToken( &s, "/IF=TCPIP" ) )
        {
            struct stEthernetCpuInfo *ethernetCpuInfo = malloc(256*sizeof(struct stEthernetCpuInfo) );
            if( ethernetCpuInfo != NULL )
            {
                int i;
                int noOfCpu = SearchEthernetCpus( ethernetCpuInfo, 256 );
                for( i = 0; i < noOfCpu; ++i, ++ethernetCpuInfo )
                {
                    memset( cpuobject, 0, sizeof(PVIOBJECT) );
                    sprintf( cpuobject->descriptor, "/DAIP=%s", ethernetCpuInfo->ipAddress );

                    CreateUniqueObjectName( tempstring, cpuobject->descriptor );
                    sprintf( cpuobject->name, "%s/CPU%s", object->name, tempstring );

                    cpuobject->type = POBJ_CPU;
                    sprintf( descriptor, "CD=\"%s\"", cpuobject->descriptor );
                    result = PviCreate(&cpuobject->linkid, cpuobject->name, cpuobject->type, descriptor, PviCallback, SET_PVICALLBACK_DATA, 0, "Ev=s" );
                    if( result == 0 )
                    {
                        AddPviObject( cpuobject, 1 );
                    }
                }
                free(ethernetCpuInfo);
            }


//            char ipaddress[20];
//            int node;  // Knotennummer
//
//            if( object->ex.dev.allow_icmp && object->ex.dev.broadcast != 0 )
//            {
//                object->ex.dev.allow_icmp = 0;
//                //if( Ping( object->ex.dev.broadcast, pbuffer, 65535 ) != 0 ){
//                if( SearchCpuViaUDP( 255, object->ex.dev.broadcast, pbuffer, 65535 ) != 0 )
//                {
//                    object = NULL;
//                    break; // Fehler
//                }
//
//                s = pbuffer;
//                d = ipaddress;
//                while( *s )
//                {
//                    *d++=*s++;
//                    if( *s == '\t' || *s == 0 )  // Ende des Eintrags
//                    {
//                        *d = 0; // String mit 0 abschliessen
//                        sscanf( ipaddress, "%3u%s", &node, ipaddress );
//                        d = ipaddress;
//                        //
//                        memset( cpuobject, 0, sizeof(PVIOBJECT) );
//                        sprintf( cpuobject->descriptor, "/DAIP=%s /DA=%u", ipaddress, node );
//
//                        CreateUniqueObjectName( tempstring, cpuobject->descriptor );
//                        sprintf( cpuobject->name, "%s/CPU%s", object->name, tempstring );
//
//                        cpuobject->type = POBJ_CPU;
//                        sprintf( descriptor, "CD=\"%s\"", cpuobject->descriptor );
//                        result = PviCreate(&cpuobject->linkid, cpuobject->name, cpuobject->type, descriptor, PviCallback, SET_PVICALLBACK_DATA, 0, "Ev=s" );
//                        if( result == 0 )
//                        {
//
//                            if( result == 0 )
//                            {
//                                AddPviObject( cpuobject, 1 );
//                                //MessageBox( NULL, "CPU gefunden!", "Hinweis", MB_OK );
//                            }
//                        }
//
//                        //
//                        if( *s == 0 )
//                            break;
//                        else
//                            ++s;
//                    }
//                }
//            }

            break;
        }

        ++s;
    }

    free(pbuffer);
    free(cpuobject);
    return object;
}


/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: PviRefreshTaskList
 Created  : Wed Feb  1 16:58:28 2006
 Modified : Wed Feb  8 10:01:46 2006

 Synopsys : Liest eine Liste der auf der Steuerung verfügbaren Tasks
 Input    : Zeiger auf CPU- Object
 Output   : Zeiger auf CPU- Objekt
 Errors   : NULL = kein CPU- Object gefunden

 ------------------------------------------------------------------@@-@@-*/

static PVIOBJECT * PviReadTaskList(PVIOBJECT *object)
{
    PVIOBJECT *tempobject, *o;
    int result = 0;
    char *pbuffer;
    char *s, *d;

    if( object == NULL )
        return NULL;

    if( object->type != POBJ_CPU )
        return NULL;

    /* Speicher allokieren */
    pbuffer = (char *)malloc(65535);

    if( pbuffer == NULL )
    {
        AddRowToLog("PviRefreshTaskList:malloc(65535)", -1 );
        return NULL;  // Fehler
    }

    tempobject = malloc(sizeof(PVIOBJECT));

    if( tempobject == NULL )
    {
        free(pbuffer); /* im Fehlerfall den Speicher wieder freigeben */
        AddRowToLog("PviRefreshTaskList:malloc(PVIOBJECT)", -1 );
        return NULL;  // Fehler
    }

    /* AR- Version holen */
    object->ex.cpu.arversion[0] = 0;
    result = PviRead(object->linkid, POBJ_ACC_VERSION, NULL, 0, object->ex.cpu.arversion, sizeof(object->ex.cpu.arversion)-1 );

    if( result != 0 )
    {
        return NULL;
    }

    /* Typ der CPU holen */
    object->ex.cpu.cputype[0] = 0;
    result = PviRead(object->linkid, POBJ_ACC_CPU_INFO, NULL, 0, pbuffer, 65535);
    if( result == 0 )
    {
        s = pbuffer;
        while( s )
        {
            if( FindToken( &s, "CT=" ) )
            {
                strcpy( object->ex.cpu.cputype, s );
                break;
            }
            ++s;
        }
    }

    /* Status der CPU holen */
    result = PviRead(object->linkid, POBJ_ACC_STATUS, NULL, 0, pbuffer, 65535 );
    if( result != 0 )
    {
        return NULL;

    }


    /* Liste der Tasks holen */
    if( object->error != 0 )
        return NULL;

    result = PviRead(object->linkid, POBJ_ACC_LIST_TASK, NULL, 0, pbuffer, 65535);
    if (result == 0)
    {
        s = pbuffer;
        while(*s)
        {
            memset(tempobject, 0, sizeof(PVIOBJECT));
            d = tempobject->descriptor;
            do
            {
                *d++ = *s++;
            }
            while ((*s != 0) && (*s != '\t'));
            ++s;					// Tab überlesen
            *d = 0;					// Name mit Null abschliessen
            strcpy( tempobject->name, object->name );
            strcat( tempobject->name, "/" );
            strcat( tempobject->name, tempobject->descriptor );
            tempobject->type = POBJ_TASK;

            if( (o=AddPviObject(tempobject, 1)) != NULL )
            {
                o->ex.task.cpu = object; // Zeiger auf CPU- Object speichern
                object->gui_info.has_childs = 1; // CPU- "Kinder" wurden angelegt
            }
        }
    }
    else
    {
        AddRowToLog("PviRefreshTaskList", result);
    }


    /* Speicher freigeben */
    free(pbuffer);
    free(tempobject);
    return object;  // Zeiger auf CPU- Object zurueckgeben
}


/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: PviReadPvarList
 Created  : Wed Feb  8 10:18:15 2006
 Modified : Wed Feb  8 10:18:15 2006

 Synopsys : liest die Liste der Variablen, die ueber das angegebene Object
            erreichbar sind
 Input    :
 Output   :
 Errors   : NULL
 ------------------------------------------------------------------@@-@@-*/

static PVIOBJECT *PviReadPvarList(PVIOBJECT * object)
{
    PVIOBJECT newobject;
    char *pbuffer, *s, *d;
    int result;

    /* Objektueberpruefung */
    if (object == NULL)
        return NULL;

    if (object->type != POBJ_CPU && object->type != POBJ_TASK)
        return NULL;


    /* Speicher allokieren */
    pbuffer = (char *)malloc(65535);

    if (pbuffer == NULL)
    {
        AddRowToLog("PviReadPvarList:malloc(65535)", -1);
        return NULL;			// Fehler
    }

    memset(pbuffer, 0, 65535 );

    /* ggf. Objekt linken */
    AddPviObject( object, 1 );
    /* Liste der Variablen holen */
    result = PviRead(object->linkid, POBJ_ACC_LIST_PVAR, NULL, 0, pbuffer, 65530);
    if (result == 0)
    {
        s = pbuffer;
        strcat(pbuffer, "\t");

        memset(&newobject, 0, sizeof(PVIOBJECT));
        while ( *s )
        {
            if (*s == ' ')  	// Leerzeichen
            {
                ++s;			// Überlesen
                continue;
            }

            if (strlen(newobject.descriptor) == 0)  	// Variablennamen holen
            {
                d = newobject.descriptor;
                while (*s && *s != ' ')
                {
                    *d++ = *s++;
                }
                *d = 0;			// mit 0 abschliessen;
                continue;
            }

            if (FindToken(&s, KWDESC_PVLEN "=" ))   	// Länge der Variable
            {
                newobject.ex.pv.length = GetIntValue(&s);
                continue;
            }

            if (FindToken(&s,  KWDESC_PVTYPE "="))  	// Typ der Variable
            {
                if (FindToken(&s, KWPVTYPE_BOOLEAN))
                {
                    newobject.ex.pv.type = BR_BOOL;
                    newobject.ex.pv.length = 1;
                    newobject.ex.pv.pdatatype = "BOOL";
                }
                else if (FindToken(&s, KWPVTYPE_UINT8))
                {
                    newobject.ex.pv.type = BR_USINT;
                    newobject.ex.pv.length = 1;
                    newobject.ex.pv.pdatatype = "USINT";
                }
                else if (FindToken(&s, KWPVTYPE_INT8))
                {
                    newobject.ex.pv.type = BR_SINT;
                    newobject.ex.pv.length = 1;
                    newobject.ex.pv.pdatatype = "SINT";
                }
                else if (FindToken(&s, KWPVTYPE_UINT16))
                {
                    newobject.ex.pv.type = BR_UINT;
                    newobject.ex.pv.length = 2;
                    newobject.ex.pv.pdatatype = "UINT";
                }
                else if (FindToken(&s, KWPVTYPE_INT16))
                {
                    newobject.ex.pv.type = BR_INT;
                    newobject.ex.pv.length = 2;
                    newobject.ex.pv.pdatatype = "INT";
                }
                else if (FindToken(&s, KWPVTYPE_UINT32))
                {
                    newobject.ex.pv.type = BR_UDINT;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "UDINT";
                }
                else if (FindToken(&s, KWPVTYPE_INT32))
                {
                    newobject.ex.pv.type = BR_DINT;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "DINT";
                }
                else if (FindToken(&s, KWPVTYPE_FLOAT32))
                {
                    newobject.ex.pv.type = BR_REAL;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "REAL";
                }
                else if (FindToken(&s, KWPVTYPE_FLOAT64))
                {
                    newobject.ex.pv.type = BR_LREAL;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "LREAL";
                }
                else if (FindToken(&s, KWPVTYPE_DATI))
                {
                    newobject.ex.pv.type = BR_DATI;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "DATE_AND_TIME";
                }
                else if (FindToken(&s, KWPVTYPE_DATE))
                {
                    newobject.ex.pv.type = BR_DATE;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "DATE";
                }
                else if (FindToken(&s, KWPVTYPE_TOD))
                {
                    newobject.ex.pv.type = BR_TOD;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "TOD";
                }
                else if (FindToken(&s, KWPVTYPE_TIME))
                {
                    newobject.ex.pv.type = BR_TIME;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "TIME";
                }
                else if (FindToken(&s, KWPVTYPE_STRUCTURE))
                {
                    newobject.ex.pv.type = BR_STRUCT;
                    if( newobject.ex.pv.pdatatype == NULL )
                        newobject.ex.pv.pdatatype = "DATATYPE";
                }
                else if (FindToken(&s, KWPVTYPE_STRING))
                {
                    newobject.ex.pv.type = BR_STRING;
                    newobject.ex.pv.pdatatype = "STRING";
                }
                else
                {
                    newobject.ex.pv.type = 0;	// unbekannt
                    newobject.ex.pv.pdatatype = "UNKNOWN";
                    while (*s != ' ' && *s != 0)
                        ++s;	// Rest überlesen
                }
                continue;
            }

            if (FindToken(&s, KWDESC_SCOPE "="))    // Gültigkeitsbereich der Variable
            {
                newobject.ex.pv.scope[0] = *s++;
                newobject.ex.pv.scope[1] = 0;
                while (*s != ' ' && *s != 0)
                    ++s;	// Rest überlesen
                continue;
            }

            if (FindToken(&s, KWDESC_PVCNT "="))  	// Anzahl der Elemente
            {
                newobject.ex.pv.dimension = GetIntValue(&s);
                continue;
            }

            if (FindToken( &s, "\t" ) || *(s+1) == 0 )  	// Ende des Eintrags oder der gesamten Liste
            {
                newobject.type = POBJ_PVAR;
                strcpy( newobject.name, object->name );
                strcat( newobject.name, "/" );
                strcat( newobject.name, newobject.descriptor );
                if( object->type == POBJ_CPU )  // globale Variable ?
                {
                    newobject.ex.pv.task = object;  // da Task nicht existiert, hier CPU- Object eintragen
                    newobject.ex.pv.cpu = object;	// CPU - Objekt speichern
                }
                else    // lokale Variable
                {
                    PVIOBJECT *cpu = object->ex.task.cpu;

                    newobject.ex.pv.task = object; // Zeiger auf Task- Objekt speichern
                    newobject.ex.pv.cpu = cpu;  // Zeiger auf CPU- Object speichern
                }
                if( AddPviObject(&newobject, 0) != NULL )
                {
                    object->gui_info.has_childs = 1; // "Kinder" wurden als eigene Objekte angelegt
                }
                memset(&newobject, 0, sizeof(PVIOBJECT));
                continue;
            }

            ++s;
        }						/* end of while (*s != 0)... */

    }
    else
    {
        AddRowToLog("PviRefreshTaskList(Variablen)", result);
    }

    //Speicher freigeben
    free(pbuffer);
    return (object);

}




/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: PviReadStructElements
 Created  : Fri Feb 17 12:38:29 2006
 Modified : Fri Feb 17 12:38:29 2006

 Synopsys : liest die Elemente von Strukturen und legt diese als eigene
            Objekte an
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static PVIOBJECT *PviReadStructElements( PVIOBJECT *object )
{
    char *pbuffer;
    int result;
    char *s, *d;
    PVIOBJECT newobject;
    PVIOBJECT *o;
    BOOL element_definition;

    //Objekt ueberpruefen
    if( object == NULL )
        return NULL;

    if( object->type != POBJ_PVAR && object->ex.pv.type != BR_STRUCT )
        return NULL;

    /* Speicher allokieren */
    pbuffer = (char *)malloc(65535);

    if( pbuffer == NULL )
    {
        AddRowToLog("PviReadStructElements:malloc(65535)", -1 );
        return NULL;  // Fehler
    }


    memcpy( &newobject, object, sizeof(PVIOBJECT) );
    strcpy( newobject.descriptor, "CD=\"" );
    strcat( newobject.descriptor, object->descriptor );
    strcat( newobject.descriptor, "\"" );

    result = PviCreate(&newobject.linkid, newobject.name, newobject.type, newobject.descriptor, PviCallback, SET_PVICALLBACK_DATA, 0, "Ev=e");
    if( result == 0 || result == 12002 )
    {
        result = PviRead(newobject.linkid, POBJ_ACC_TYPE_EXTERN, NULL, 0, pbuffer, 65535);
    }
    PviDelete( newobject.name );

    if( result == 0 )
    {
        //AddRowToLog( pbuffer, 0 );

        s = pbuffer;
        element_definition = FALSE;

        while( *s != 0 )
        {
            if( *s == ' ' )    // Leerzeichen
            {
                ++s;		// Überlesen
                continue;
            }

            if( !element_definition  )  // Strukturname des Parents
            {
                if( FindToken(&s, KWDESC_SNAME "=" ) )
                {
                    d = tempstring;
                    while( *s && *s != ' ' )
                    {
                        *d++=*s++;
                    }
                    *d = 0;
                    object->ex.pv.pdatatype = AddPviDatatype(tempstring);
                }
            }

            if( FindToken( &s, "{" ) )    // Beginn einer Elementdefinition
            {
                element_definition = TRUE;
                memset( &newobject, 0, sizeof(PVIOBJECT) );
                memset( tempstring, 0, sizeof(tempstring) );
                d = tempstring;
                while( *s && *s == ' ' ) // Leerzeichen überlesen
                    ++s;
                while( *s && *s!=' ' )
                    *d++ = *s++;
                *d = 0;
                strcpy( newobject.descriptor, object->descriptor );  // descriptor erstellen
                strcat( newobject.descriptor, tempstring );
                strcpy( newobject.name, object->name );		// name erstellen
                strcat( newobject.name, tempstring );
                newobject.type =POBJ_PVAR;
                // Anzahl der Punkte zählen, wenn > 1, dann ist Elementdefinition eine Unterstruktur
                if( CountToken( tempstring, '.' ) > 1 )
                {
                    while( !FindToken( &s, "}" ) )
                        ++s;  // alles überlesen
                    element_definition = FALSE;
                }
                continue;
            }

            else

                if( element_definition == FALSE )
                {
                    ++s;
                    continue;
                }



            if (FindToken(&s, KWDESC_PVLEN "=" ))   	// Länge der Variable
            {
                newobject.ex.pv.length = GetIntValue(&s);
                continue;
            }

            if (FindToken(&s, KWDESC_PVCNT "=" ))   	// Anzahl der Elemente der Variable
            {
                newobject.ex.pv.dimension = GetIntValue(&s);
                continue;
            }



            if (FindToken(&s,  KWDESC_PVTYPE "="))  	// Typ der Variable
            {
                if (FindToken(&s, KWPVTYPE_BOOLEAN))
                {
                    newobject.ex.pv.type = BR_BOOL;
                    newobject.ex.pv.length = 1;
                    newobject.ex.pv.pdatatype = "BOOL";
                }
                else if (FindToken(&s, KWPVTYPE_UINT8))
                {
                    newobject.ex.pv.type = BR_USINT;
                    newobject.ex.pv.length = 1;
                    newobject.ex.pv.pdatatype = "USINT";
                }
                else if (FindToken(&s, KWPVTYPE_INT8))
                {
                    newobject.ex.pv.type = BR_SINT;
                    newobject.ex.pv.length = 1;
                    newobject.ex.pv.pdatatype = "SINT";
                }
                else if (FindToken(&s, KWPVTYPE_UINT16))
                {
                    newobject.ex.pv.type = BR_UINT;
                    newobject.ex.pv.length = 2;
                    newobject.ex.pv.pdatatype = "UINT";
                }
                else if (FindToken(&s, KWPVTYPE_INT16))
                {
                    newobject.ex.pv.type = BR_INT;
                    newobject.ex.pv.length = 2;
                    newobject.ex.pv.pdatatype = "INT";
                }
                else if (FindToken(&s, KWPVTYPE_UINT32))
                {
                    newobject.ex.pv.type = BR_UDINT;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "UDINT";
                }
                else if (FindToken(&s, KWPVTYPE_INT32))
                {
                    newobject.ex.pv.type = BR_DINT;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "DINT";
                }
                else if (FindToken(&s, KWPVTYPE_FLOAT32))
                {
                    newobject.ex.pv.type = BR_REAL;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "REAL";
                }
                else if (FindToken(&s, KWPVTYPE_FLOAT64))
                {
                    newobject.ex.pv.type = BR_LREAL;
                    newobject.ex.pv.length = 8;
                    newobject.ex.pv.pdatatype = "LREAL";
                }
                else if (FindToken(&s, KWPVTYPE_DATI))
                {
                    newobject.ex.pv.type = BR_DATI;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "DATE_AND_TIME";
                }
               else if (FindToken(&s, KWPVTYPE_DATE))
                {
                    newobject.ex.pv.type = BR_DATI;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "DATE";
                }
               else if (FindToken(&s, KWPVTYPE_TOD))
                {
                    newobject.ex.pv.type = BR_TOD;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "TOD";
                }
                else if (FindToken(&s, KWPVTYPE_TIME))
                {
                    newobject.ex.pv.type = BR_TIME;
                    newobject.ex.pv.length = 4;
                    newobject.ex.pv.pdatatype = "TIME";
                }
                else if (FindToken(&s, KWPVTYPE_STRUCTURE))
                {
                    newobject.ex.pv.type = BR_STRUCT;
                    if( newobject.ex.pv.pdatatype == NULL )
                        newobject.ex.pv.pdatatype = "DATATYPE(STRUCT)";
                }
                else if (FindToken(&s, KWPVTYPE_STRING))
                {
                    newobject.ex.pv.type = BR_STRING;
                    newobject.ex.pv.pdatatype = "STRING";
                }
                else
                {
                    newobject.ex.pv.type = 0;	// unbekannt
                    newobject.ex.pv.pdatatype = "UNKNOWN";
                    while (*s != ' ' && *s != 0)
                        ++s;	// Rest überlesen
                }
                continue;
            }

//#ifdef RAUS
            if( element_definition  && !strcmp(newobject.ex.pv.pdatatype, "DATATYPE") )  // Strukturname des Elementes
            {
                if( FindToken(&s, KWDESC_SNAME "=" ) )
                {
                    d = tempstring;
                    while( *s && *s != ' ' )
                    {
                        *d++=*s++;
                    }
                    *d = 0;
                    newobject.ex.pv.pdatatype = AddPviDatatype(tempstring);
                }
            }
//#endif
            if (FindToken(&s, KWDESC_PVCNT "="))  	// Anzahl der Elemente
            {
                newobject.ex.pv.dimension = GetIntValue(&s);
                continue;
            }

            if (FindToken( &s, "}" ) || *(s+1) == 0 )  	// Ende des Eintrags oder der gesamten Liste
            {
                PVIOBJECT newelement;
                int i;
//				int j;
                element_definition = FALSE;

                for( i = 0; i < object->ex.pv.dimension; ++i )
                {
#ifdef RAUS
                    for( j = 0; j < newobject.ex.pv.dimension; ++j )
                    {
#endif
                        memcpy( &newelement, &newobject, sizeof(PVIOBJECT) );
#ifdef RAUS
                        newelement.ex.pv.dimension = 1;  // Dimension wird nicht geerbt
#endif

                        if( object->ex.pv.dimension > 1 )  // Elternobject ist ein Array
                        {
                            strcpy( tempstring, newobject.descriptor+strlen(object->descriptor)+1 ); // Name des Elementes
                            sprintf( newelement.name, "%s[%i].%s", object->name, i, tempstring );
                            sprintf( newelement.descriptor, "%s[%i].%s", object->descriptor, i, tempstring );
                        }

#ifdef RAUS
                        if( newobject.ex.pv.dimension > 1 )   // Kindobject ist ein Array
                        {
                            sprintf( tempstring, "[%i]", j );
                            strcat( newelement.name, tempstring );
                            strcat( newelement.descriptor, tempstring );
                        }
#endif

                        strncpy( newelement.ex.pv.scope, object->ex.pv.scope, 1 ); // Gültigkeitsbereich wird geerbt
                        newelement.ex.pv.task = object->ex.pv.task;  // Task wird geerbt
                        newelement.ex.pv.cpu = object->ex.pv.cpu;  // Task wird geerbt
                        if( (o = AddPviObject( &newelement, FALSE )) != NULL )
                        {
                            //object->gui_info.has_childs = 1;  // mind. 1 "Kind" wurde für das Objekt angelegt
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


    if( result )
    {
        AddRowToLog( "PviReadStructElements:PviCreate/PviRead", result );
        object = NULL;
    }

    free(pbuffer);
    return object;
}






/* ======================================================================================
   Funktionen fuer Logdatei
   ====================================================================================== */

/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: AddRowToLog
 Created  : Tue Jan 31 13:04:31 2006
 Modified : Tue Jan 31 13:04:31 2006

 Synopsys : schreibt in Logdatei
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void AddRowToLog(char *row, int errcode)
{
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

    if (f != NULL)
    {
        if (errcode != 0)
        {
            fprintf(f, "%s - %s : %s --> Error:%i\n", datum, zeit, row, errcode);
        }
        else
        {
            fprintf(f, "%s - %s : %s --> OK! \n", datum, zeit, row);
        }
        fclose(f);
    }

    if (errcode != 0)
    {
        char *errtext;
        pmessage = (char *)malloc(512);

        if (pmessage != NULL)
        {
            switch( errcode )
            {
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


static void DeleteLog(void)
{
    char filename[MAX_PATH];

    strcpy( filename, GetApplicationPath() );
    strcat( filename, "\\" PVILOGFILE );
    remove( filename );
}

/* =====================================================================================
	Handling der Watch- Dateien
	====================================================================================
*/



/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: AddToPviWatchFile
 Created  : Thu Mar  9 09:21:59 2006
 Modified : Thu Mar  9 09:21:59 2006

 Synopsys : Schreibt ein Object in die Watch- Datei
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/
int AddToPviWatchFile(PVIOBJECT * object, char *filename)
{
    PVIOBJECT *parent;
    char *section;
    int entries;
//	int result;
    int i;



    // Versionsinformation speichern
    WritePrivateProfileString( "File", "Application", "BRWATCH.EXE", filename ); // Type
    WritePrivateProfileString( "File", "Version", WATCH_FILE_VERSION, filename ); // Version

    parent = GetNextPviObject(TRUE);

    // Eltern suchen und ggfs. speichern
    while (parent != NULL)
    {
        if ( IsPviObjectParentOf(parent, object) && strrchr(parent->name, '.') == NULL )  	// Eltern suchen
        {
            switch (parent->type)
            {
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
            if (section != NULL)
            {
                char keyname[80];
                BOOL found = FALSE;
                tempstring[0] = 0;
                GetPrivateProfileString( section, section, "0", tempstring, sizeof(tempstring), filename );
                entries = atoi(tempstring);
                // Nachschauen, ob Eintrag schon vorhanden ist
                for( i = 1; i <= entries; ++i )
                {
                    sprintf( keyname, "name%u", i );
                    GetPrivateProfileString( section, keyname, "", tempstring, sizeof(tempstring), filename );
                    if( strcmp( parent->name, tempstring ) == 0 )
                    {
                        found = TRUE;
                    }
                }

                if( !found )   // Eintrag noch nicht vorhanden
                {
                    // Anzahl der Einträge erhöhen
                    ++entries;
                    sprintf( tempstring, "%u", entries );
                    WritePrivateProfileString( section, section, tempstring, filename );
                    // Eltern-Objekt speichern
                    sprintf( keyname, "name%u", entries );
                    WritePrivateProfileString( section, keyname, parent->name, filename ); // Objektname
                    sprintf( keyname, "desc%u", entries );
                    WritePrivateProfileString( section, keyname, parent->descriptor, filename ); // Objektdescriptor
                    sprintf( keyname, "sort%u", entries );
                    WritePrivateProfileString( section, keyname, "-1", filename ); // Objektdescriptor
                }
            }
        }
        parent = GetNextPviObject(FALSE);
    }

    // Objekt und Sortierkritierium speichern
    switch (object->type)
    {
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
    if( section != NULL )
    {
        char keyname[80];
        tempstring[0] = 0;
        // Anzahl der Einträge erhöhen
        GetPrivateProfileString( section, section, "0", tempstring, sizeof(tempstring), filename );
        entries = atoi(tempstring);
        ++entries;
        sprintf( tempstring, "%u", entries );
        WritePrivateProfileString( section, section, tempstring, filename );
        // Objekt speichern
        sprintf( keyname, "name%u", entries );
        WritePrivateProfileString( section, keyname, object->name, filename ); // Objektname
        sprintf( keyname, "desc%u", entries );
        WritePrivateProfileString( section, keyname, object->descriptor, filename ); // Objekt-Descriptor
        sprintf( keyname, "sort%u", entries );
        sprintf( tempstring, "%u", object->watchsort );
        WritePrivateProfileString( section, keyname, tempstring, filename ); // Sortierkriterium
    }



    return 0;

}


/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: LoadPviObjectsFromWatchFile
 Created  : Thu Mar  9 09:22:26 2006
 Modified : Thu Mar  9 09:22:26 2006

 Synopsys : lädt alle Objekte des angegebenen Typs aus der Watchdatei
 Input    :
 Output   : Anzahl der eingelesenen Objekte oder - 1 für Fehler
 Errors   :
 ------------------------------------------------------------------@@-@@-*/
int LoadPviObjectsFromWatchFile(T_POBJ_TYPE typefilter, char *filename)
{
    char *section="";
    PVIOBJECT tempobject, *object, *parent;
    int entries, i, sort;
    int n = 0;  // Anzahl der Objekte, die im Watch eingefügt werden sollen

    switch( typefilter )
    {
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

    // Anzahl der Objekte der Sektion einlesen
    tempstring[0] = 0;
    GetPrivateProfileString(section, section, "0", tempstring, sizeof(tempstring), filename);
    entries = atoi(tempstring);
    if (entries == 0)
    {
        sprintf(tempstring, "No entries found in section [%s] !", section);
        MessageBox(NULL, tempstring, "Error Loading Watch File", MB_OK | MB_ICONERROR);
        return -1;
    }
    for (i = 1; i <= entries; ++i)
    {
        char keyname[80];

        memset(&tempobject, 0, sizeof(PVIOBJECT));
        // Name einlesen
        sprintf(keyname, "name%u", i);
        GetPrivateProfileString(section, keyname, "", tempobject.name, sizeof(tempobject.name), filename);
        if (strlen(tempobject.name) == 0)
        {
            sprintf(tempstring, "section [%s]\n%s=\ninvalid or not found !", section, keyname);
            MessageBox(NULL, tempstring, "Error Loading Watch File", MB_OK | MB_ICONERROR);
            return -1;
        }
        // Descriptor einlesen
        sprintf(keyname, "desc%u", i);
        GetPrivateProfileString(section, keyname, "", tempobject.descriptor, sizeof(tempobject.descriptor), filename);
        if (strlen(tempobject.descriptor) == 0)
        {
            sprintf(tempstring, "section [%s]\n%s=\ninvalid or not found !", section, keyname);
            MessageBox(NULL, tempstring, "Error Loading Watch File", MB_OK | MB_ICONERROR);
            return -1;
        }
        // Sortierkriterium einlesen
        sprintf(keyname, "sort%u", i);
        GetPrivateProfileString(section, keyname, "-1", tempstring, sizeof(tempstring), filename);
        sort = atoi(tempstring);

        // Objekt aufnehmen
        tempobject.type = typefilter;
        object = AddPviObject(&tempobject, 1);

        if( object != NULL )
        {
            object->gui_info.loaded_from_file = 1;
            object->watchsort = sort;
            if( object->watchsort >= 0 )
                ++n;
            PviReadDataType( object );

        }


        // bei PVs die dazugehörige CPU und den Task suchen...
        if( object->type == POBJ_PVAR )
        {

            parent = GetNextPviObject(TRUE);
            while (parent != NULL)
            {
                if ( IsPviObjectParentOf(parent, object) && parent->type == POBJ_CPU )  	// Eltern suchen
                {
                    object->ex.pv.cpu = parent;
                }
                parent = GetNextPviObject(FALSE);
            }

            parent = GetNextPviObject(TRUE);
            while (parent != NULL)
            {
                if ( IsPviObjectParentOf(parent, object) && parent->type == POBJ_TASK )  	// Eltern suchen
                {
                    object->ex.pv.task = parent;
                }
                parent = GetNextPviObject(FALSE);
            }


        }


        // bei Tasks die dazugehörige CPU suchen
        if( object->type == POBJ_TASK )
        {

            parent = GetNextPviObject(TRUE);
            while (parent != NULL)
            {
                if ( IsPviObjectParentOf(parent, object) && parent->type == POBJ_CPU )  	// Eltern suchen
                {
                    object->ex.task.cpu = parent;
                }
                parent = GetNextPviObject(FALSE);
            }
        }


    }
    return n;
}






// static void DeleteDirectory( char *name ){
// _DIR * dir;
// struct _dirent *entry;

// dir = _opendir(name);

// if( dir != NULL ){
// while( (entry = _readdir(dir)) != NULL ){
// remove( entry->d_name );
// }
// _closedir(dir);
// _rmdir( name );
// }


// }





