/*-------------------------------------------------------------------
Module   : pvi_interface.cpp
 -----------------------------------------------------------------*/

/*
PVI object API

PviObject represents a PVI object. Per-object operations are member functions,
including Expand(), Watch(), FindChild(), and AddToWatchFile(). Connection
and list operations use static members such as PviObject::Start(),
PviObject::FindByName(), and PviObject::GetNext().

The public data fields remain available to the Win32 UI and logger.
Add() registers a copy in a std::vector<std::unique_ptr<PviObject>>, using
std::make_unique rather than explicit new/delete. Object addresses remain
stable when the vector grows or other entries are removed. Remove() releases
that instance, and PviObject::Stop() releases the registry. Returned pointers
are borrowed and remain valid until their object is removed or PVI is stopped.
GetNext(TRUE) resets enumeration and returns the root sentinel; subsequent
GetNext(FALSE) calls enumerate the registered objects in insertion order.

String callbacks reject oversized or misaligned payloads with an error and
keep accepted strings terminated within their declared storage. The watch list
formats long strings and escaped control characters without fixed-size buffers.
Only scalar variables can be watched directly. Watch(TRUE) also enforces this
for drag-and-drop: expand arrays and structures in the tree, then select
individual elements or members.

Run the object regression tests with:
mingw32-make.exe -f Makefile SHELL=cmd test
They mock PVI calls and do not connect to a controller; the matching PVI runtime
is still required to load the application libraries.
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <new>
#include <utility>
#include <time.h>
#include <direct.h>
#include "main.h"
#include "settings.h"
#include "cpusearch.h"
#include "stringtools.h"
#include "logger.h"

#define PVILOGFILE "pvilog.txt"
#define WATCH_FILE_VERSION "1.0"
#define MAX_SUPPORTED_DEVICES 1
#define MAX_SUPPORTED_CPUS 256

/* Prototypen */
static void AddRowToLog(const char *, int);
static void DeleteLog(void);
static void CreateUniqueObjectName(char *name, char *descriptor);


/* */

static const char *AddPviDatatype(const char *t);
static int GetPviLicenceInfo(void);

/* global variable */
static char tempstring[256]; // save stack space
static UINT_PTR timerid_cyclic_requests;
static BOOL PLCDataChangeEvents; // PLC- event mode

/* registry for PVI objects */
PviObject PviObject::root;
std::vector<std::unique_ptr<PviObject>> PviObject::objects;
std::size_t PviObject::readIndex = 0;
std::size_t PviObject::childReadIndex = 0;

/* list for structure names  */
static char structnamesrootname[] = "DATATYPE";
static PVISTRUCTNAMES structnamesroot = {structnamesrootname, nullptr};

PviObject::PviObject()
	: linkid(0), type(POBJ_PVI), name{}, descriptor{}, error(0), watchsort(0),
	  gui_info{}, ex{}
{
}

/* read the INI file */

void PviObject::ReadSettings(void)
{
	PviObject cpuObject; /*, *pObject;*/
	PviObject deviceObject;

	char section[20];
	char key[20];
	char buffer[80];

	sprintf(section, "General");
	sprintf(key, "PLCDataChangeEvents");
	strcpy(buffer, "");
	GetPrivateProfileStringA(section, key, "", buffer, sizeof(buffer), SettingsGetFileName());
	PLCDataChangeEvents = atoi(buffer);

	// create DEVICE object
	strcpy(deviceObject.descriptor, "/IF=tcpip");
	CreateUniqueObjectName(tempstring, deviceObject.descriptor);
	snprintf(deviceObject.name, sizeof(deviceObject.name), "@Pvi/LNBRWATCH/DEV%s", tempstring);
	deviceObject.type = POBJ_DEVICE;
	deviceObject.ex.dev.number = 1;
	deviceObject.Add(TRUE);

	for (unsigned cpu = 1; cpu <= MAX_SUPPORTED_CPUS; ++cpu)
	{

		cpuObject = PviObject();
		sprintf(section, "CPU%u", cpu);
		strcpy(key, "descriptor");
		strcpy(buffer, "");
		GetPrivateProfileStringA(section, key, "", buffer, sizeof(buffer), SettingsGetFileName());
		strcpy(cpuObject.descriptor, buffer);

		if (strlen(buffer) != 0)
		{
			char devicename[256];
			CreateUniqueObjectName(devicename, deviceObject.descriptor);
			CreateUniqueObjectName(tempstring, cpuObject.descriptor);
			snprintf(cpuObject.name, sizeof(cpuObject.name), "@Pvi/LNBRWATCH/DEV%s/CPU%s", devicename, tempstring);
			cpuObject.type = POBJ_CPU;
			cpuObject.Add(TRUE);
		}
	}
}

/* create unique name from descriptor */

static void CreateUniqueObjectName(char *name, char *descriptor)
{
	while (*descriptor)
	{
		if (*descriptor == ' ')
			++descriptor;
		else if (*descriptor == '/')
		{
			*name++ = '#';
			++descriptor;
		}
		else if (*descriptor == '.')
		{
			*name++ = '_';
			++descriptor;
		}
		else
		{
			*name++ = toupper(*descriptor++);
		}
	}
	*name = 0;
}

/* register a copy of this object and return the registry-owned instance */

PviObject *PviObject::Add(BOOL create)
{

	PviObject *newobject = FindByName(name);
	char descriptor[256];
	int result = 0;
	const char *event_mask;

	if (newobject == nullptr)
	{
		try
		{
			auto entry = std::make_unique<PviObject>(*this);
			entry->gui_info = {};
			entry->gui_info.display_as_decimal = 1;
			entry->watchsort = -1;
			newobject = entry.get();
			objects.push_back(std::move(entry));
		}
		catch (const std::bad_alloc &)
		{
			AddRowToLog("PviObject::Add:registry allocation", -1);
			return nullptr;
		}
	}

	// create variable in PVI manager if necessary
	if (create)
	{
		strcpy(descriptor, "CD=\"");
		strcat(descriptor, newobject->descriptor);
		strcat(descriptor, "\"");
		event_mask = "Ev=e";
		if (newobject->type == POBJ_PVAR)
		{
			if (PLCDataChangeEvents && newobject->ex.pv.dimension <= 1 && newobject->ex.pv.type != BR_STRUCT && newobject->ex.pv.type != BR_STRING && newobject->ex.pv.type != BR_WSTRING)
			{
				strcat(descriptor, " RF=50 AT=rwe");
			}
			else
			{
				strcat(descriptor, " RF=50 ");
			}
			event_mask = "Ev=ed";
		}

		result = PviCreate(&newobject->linkid, newobject->name, newobject->type, descriptor, PviObject::Callback, SET_PVICALLBACK_DATA, 0, event_mask);

		if (result)
		{
			memset(tempstring, 0, sizeof(tempstring));
			snprintf(tempstring, sizeof(tempstring), "PviObject::Add:PviCreate \"%s\"", this->name);
			AddRowToLog(tempstring, result);
			return nullptr;
		}
		else
		{
			if (newobject->type == POBJ_PVAR)
			{
				PviReadRequest(newobject->linkid, POBJ_ACC_DATA, PviObject::Callback, SET_PVICALLBACK_DATA, 0);
				// read variable once
			}
		}
	}

	return newobject;
}

/* remove link to object */

PviObject *PviObject::Unlink()
{
	auto found = std::find_if(objects.begin(), objects.end(),
							 [this](const std::unique_ptr<PviObject> &object) { return object.get() == this; });
	if (found == objects.end())
		return nullptr;

	int result = PviUnlink(linkid);
	if (result == 0)
	{
		linkid = 0;
		return this;
	}
	AddRowToLog("PviObject::Unlink", result);
	return nullptr;
}

/* remove this object and return the preceding entry, or the root sentinel */

PviObject *PviObject::Remove()
{
	auto found = std::find_if(objects.begin(), objects.end(),
							 [this](const std::unique_ptr<PviObject> &object) { return object.get() == this; });
	if (found == objects.end())
		return objects.empty() ? &root : objects.back().get();

	std::size_t index = static_cast<std::size_t>(found - objects.begin());
	PviObject *previous = index == 0 ? &root : objects[index - 1].get();
	auto removed = std::move(*found);
	objects.erase(found);
	if (index < readIndex)
		--readIndex;
	if (index < childReadIndex)
		--childReadIndex;

	// Detach before PVI can dispatch callbacks, but keep the object alive until unlink completes.
	PviUnlink(removed->linkid);
	removed->linkid = 0;
	if (removed->type == POBJ_PVAR && removed->ex.pv.pvalue != nullptr)
	{
		free(removed->ex.pv.pvalue);
		removed->ex.pv.pvalue = nullptr;
	}
	return previous;
}

/* search for object with link id, expects PVI link id, returns object or NULL if not found */

PviObject *PviObject::FindByLinkId(DWORD linkid)
{
	for (const auto &object : objects)
	{
		if (object->linkid == linkid)
		{
			return object.get();
		}
	}
	return nullptr; // object not found
}

/* search for object by name, expects name of object */

PviObject *PviObject::FindByName(const char *name)
{
	if (name != nullptr)
	{
		for (const auto &object : objects)
		{
			if (strcmp(object->name, name) == 0)
			{
				return object.get();
			}
		}
	}
	return nullptr; // object not found
}

/* adds object to watch list */

PviObject *PviObject::Watch(BOOL watch)
{
	if (type != POBJ_PVAR)
		return this;

	if (!watch)
	{
		Unlink();
		watchsort = -1;
		if (ex.pv.pvalue != nullptr)
		{
			free(ex.pv.pvalue);
			ex.pv.pvalue = nullptr;
		}
		return this;
	}

	if (ex.pv.type == BR_STRUCT || ex.pv.dimension > 1)
	{
		watchsort = -1;
		MessageBoxA(MainWindowGetHandle(),
					"Arrays and structures cannot be watched directly.\n"
					"Expand the object in the tree and select individual elements or members.",
					"Watch list", MB_OK | MB_ICONINFORMATION);
		return nullptr;
	}

	PviObject *object = Add(TRUE);
	if (object != nullptr)
	{
		object->ex.pv.pvalue = malloc(object->ex.pv.length);
		if (object->ex.pv.pvalue)
		{
			if (object->ex.pv.type == BR_REAL)
				*((float *)object->ex.pv.pvalue) = 0.0;
			else if (object->ex.pv.type == BR_LREAL)
				*((double *)object->ex.pv.pvalue) = 0.0;
			else
				memset(object->ex.pv.pvalue, 0, object->ex.pv.length);
		}
	}
	return object;
}

/* get next object in list, returns next object or first one if 'first' is set to 1 */

PviObject *PviObject::GetNext(BOOL first)
{
	if (first)
	{
		readIndex = 0;
		return &root;
	}
	if (readIndex < objects.size())
		return objects[readIndex++].get();
	return nullptr;
}

/* search for child object, return first child if 'first' is set to 1 */

PviObject *PviObject::FindChild(BOOL first)
{
	size_t length;

	if (first)
		childReadIndex = 0;

	length = strlen(name);

	while (childReadIndex < objects.size())
	{
		PviObject *find = objects[childReadIndex++].get();
		if (strncmp(name, find->name, length) == 0)
		{
			if (find->name[length] == '/' || find->name[length] == '[' || find->name[length] == '.')
				if (strrchr(find->name + length + 1, '.') == nullptr &&
					strrchr(find->name + length + 1, '/') == nullptr) // no grandchilds
					return (find);
		}
	}

	return nullptr;
}

/* determines parent / child relationship */

BOOL PviObject::IsParentOf(const PviObject *child) const
{
	size_t length;

	if (child == nullptr)
		return FALSE;

	length = strlen(this->name);

	if (strncmp(this->name, child->name, length) == 0)
	{
		if (this != child)
			return TRUE;
	}
	return FALSE;
}

/* count number of objects */

int PviObject::Count(void)
{
	return static_cast<int>(objects.size());
}

/* global PVI event */

void WINAPI PviObject::GlobalEvent(WPARAM /*wParam*/, LPARAM /*lParam*/, LPVOID /*pData*/, DWORD /*DataLen*/, T_RESPONSE_INFO *pInfo)
{
	PviObject po, *ppo;

	switch (pInfo->nType)
	{
	case POBJ_EVENT_PVI_CONNECT:

		break;

	case POBJ_EVENT_PVI_DISCONN:
		AddRowToLog("POBJ_EVENT_PVI_DISCONN", -1);
		break;

	case POBJ_EVENT_PVI_ARRANGE:
		AddRowToLog("POBJ_EVENT_PVI_ARRANGE", 0);
		ppo = &po;
		strcpy(po.name, "@Pvi/LNBRWATCH");
		strcpy(po.descriptor, "LnAnsl");
		po.type = POBJ_LINE;
		ppo->Add(1);
		PviObject::ReadSettings();

		if (pvi_interface_notify.cbobjects_valid != nullptr)
		{
			pvi_interface_notify.cbobjects_valid();
		}
		break;
	}
}

/* PVI callback */

void WINAPI PviObject::Callback(WPARAM /*wParam*/, LPARAM /*lParam*/, LPVOID pData, DWORD DataLen, T_RESPONSE_INFO *pInfo)
{
	PviObject *object;
	static char value[128];
	char *s, *d;

	switch (pInfo->nType)
	{
	case POBJ_EVENT_DATA: // data change event
		if (DataLen == 0) // no data
			return;
		object = PviObject::FindByLinkId(pInfo->LinkID);
		if (object != nullptr)
		{
			if (object->type == POBJ_PVAR)
			{
				switch (object->ex.pv.type)
				{
				case BR_STRING:
				case BR_WSTRING:
					if (object->ex.pv.pvalue != nullptr)
					{
						DWORD unitSize = object->ex.pv.type == BR_WSTRING ? sizeof(WCHAR) : sizeof(char);
						DWORD capacity = object->ex.pv.length;
						if (capacity < unitSize || DataLen > capacity ||
							capacity % unitSize != 0 || DataLen % unitSize != 0)
						{
							AddRowToLog("PviObject::Callback:invalid string data length", -1);
							return;
						}
						auto bytes = static_cast<char *>(object->ex.pv.pvalue);
						memcpy(bytes, pData, DataLen);
						memset(bytes + DataLen, 0, capacity - DataLen);
						memset(bytes + capacity - unitSize, 0, unitSize);
					}
					break;

				case BR_USINT:
				case BR_SINT:
				case BR_BOOL:
					if (object->ex.pv.pvalue != nullptr)
						memcpy(object->ex.pv.pvalue, pData, 1);
					break;

				case BR_UINT:
				case BR_INT:
					if (object->ex.pv.pvalue != nullptr)
						memcpy(object->ex.pv.pvalue, pData, 2);
					break;

				case BR_UDINT:
				case BR_DINT:
				case BR_DATI:
				case BR_TOD:
				case BR_DATE:
				case BR_TIME:
				case BR_REAL:
					if (object->ex.pv.pvalue != nullptr)
						memcpy(object->ex.pv.pvalue, pData, 4);
					break;

				case BR_LREAL:
					if (object->ex.pv.pvalue != nullptr)
						memcpy(object->ex.pv.pvalue, pData, 8);
					break;

				default:
					break;
				}
				if (object->error)
				{ // reset error state
					object->error = 0;
					if (pvi_interface_notify.cberror_changed != nullptr)
						pvi_interface_notify.cberror_changed(object, 0);
				}
				if (pvi_interface_notify.cbdata_changed != nullptr)
					pvi_interface_notify.cbdata_changed(object);

				if (object->ex.pv.pvalue != nullptr)
				{
					object->ex.pv.value_changed = 1; /* value was changed */
					LoggerDataChanged(object);
				}
			}
		}
		break;

	case POBJ_EVENT_ERROR: // change of error state
		object = PviObject::FindByLinkId(pInfo->LinkID);
		if (object != nullptr)
		{
			object->error = pInfo->ErrCode;

			if (pvi_interface_notify.cberror_changed != nullptr)
				pvi_interface_notify.cberror_changed(object, pInfo->ErrCode);
		}

		break;

	case POBJ_ACC_STATUS: // change of state
		if (pInfo->ErrCode != 0)
			return;
		object = PviObject::FindByLinkId(pInfo->LinkID);
		if (object != nullptr)
		{
			if (pvi_interface_notify.cbdata_changed != nullptr)
			{
				s = (char *)pData;
				d = value;

				*d = 0;
				while (*s)
				{
					if (FindToken(&s, "ST="))
					{
						while ((*d++ = *s++) != 0)
							;
						*d = 0;
						break;
					}
					++s;
				}

				switch (object->type)
				{
				case POBJ_CPU:
					strncpy(object->ex.cpu.status, value, sizeof(object->ex.cpu.status) - 1);
					if (strcmp(value, KWSTATUS_WARMSTART) == 0 || strcmp(value, KWSTATUS_COLDSTART) == 0)
					{
						object->ex.cpu.running = TRUE;
					}
					else
					{
						object->ex.cpu.running = FALSE;
					}
					break;

				case POBJ_TASK:
					strncpy(object->ex.task.status, value, sizeof(object->ex.task.status) - 1);
					break;

				default:
					break;
				}
				pvi_interface_notify.cbdata_changed(object);
			}
		}
		break;

	case POBJ_ACC_DATE_TIME: // read clock
		object = PviObject::FindByLinkId(pInfo->LinkID);
		if (object != nullptr)
		{
			if (object->type == POBJ_CPU)
			{
				if (DataLen > sizeof(struct tm))
					DataLen = sizeof(struct tm);
				memcpy(&object->ex.cpu.rtc_time, pData, DataLen);
				// not notification since POBJ_ACC_STATUS does that already...
			}
		}
		break;
	}
}

/* is used to send cyclic requests, since not all events are supported for all access types. */
void CALLBACK PviObject::CyclicRequests(HWND /*hwnd*/, UINT /*uMsg*/, UINT_PTR /*idEvent*/, DWORD /*dwTime*/)
{
	PviObject *object;
	static int zyklus_readpv = 0;
	static int zyklus_getlicenseinfo = 0;

	if (++zyklus_readpv > 4)
	{
		zyklus_readpv = 0;
	}

	// read PVI license information
	if (zyklus_getlicenseinfo < 10)
	{
		if (++zyklus_getlicenseinfo >= 10)
		{
			if (GetPviLicenceInfo() == PVIWORK_STATE_LOCKED)
			{
				MessageBoxA(MainWindowGetHandle(), "Sorry PVI is locked.\nProgram must shut down !", "Error Pvi License", MB_ICONERROR);
				SendMessage(MainWindowGetHandle(), WM_CLOSE, 0, 0);
			}
			else
			{
				zyklus_getlicenseinfo = 0;
			}
		}
	}

	object = PviObject::GetNext(TRUE);

	while ((object = PviObject::GetNext(FALSE)) != nullptr)
	{
		if (object->linkid == 0 || object->error != 0)
			continue;

		switch (object->type)
		{
		case POBJ_CPU:
			PviReadRequest(object->linkid, POBJ_ACC_STATUS, PviObject::Callback, SET_PVICALLBACK_DATA, object->linkid);
			PviReadRequest(object->linkid, POBJ_ACC_DATE_TIME, PviObject::Callback, SET_PVICALLBACK_DATA, object->linkid);
			break;

		case POBJ_TASK:
			PviReadRequest(object->linkid, POBJ_ACC_STATUS, PviObject::Callback, SET_PVICALLBACK_DATA, object->linkid);
			break;

		default:
			break;
		}
	}
}

static int GetPviLicenceInfo(void)
{
	DWORD LinkId;
	long result;
	T_PVI_INFO_LICENCE license;

	memset(&license, 0, sizeof(license));
	result = PviLink(&LinkId, "@Pvi", PVI_HMSG_NIL, 0, 0, 0);

	if (result == 0)
	{
		result = PviRead(LinkId, POBJ_ACC_INFO_LICENCE, 0, 0, &license, sizeof(license));
	}

	if (result == 12059)
	{
		return PVIWORK_STATE_LOCKED;
	}

	else if (result != 0)
	{
		AddRowToLog("GetPviLicenceInfo()", result);
	}

	PviUnlink(LinkId);

	return license.PviWorkState[0];
}

/* start PVI connection */
int PviObject::Start(void)
{
	int result;

	if (!objects.empty())
	{
		result = Stop();
		if (result != 0)
			return result;
	}
	root = PviObject();
	strcpy(root.name, "@Pvi");
	strcpy(root.descriptor, "");
	root.type = POBJ_PVI;
	readIndex = 0;
	childReadIndex = 0;

	DeleteLog(); // delete PVI log file

	// write PVI version to logfile
	strcpy(g_PVIVersionString, "");
	PviGetVersion(g_PVIVersionString, sizeof(g_PVIVersionString));
	AddRowToLog(g_PVIVersionString, 0);

	// start PVI
	{
		char buffer[80];

		memset(buffer, 0, sizeof(buffer));
		GetPrivateProfileStringA("REMOTE", "parameter", "COMT=10 AS=1 PT=20", buffer, sizeof(buffer), SettingsGetFileName());
		result = PviInitialize(0, 0, buffer, nullptr);
	}

	if (result)
	{
		AddRowToLog("PviInitialize", result);
		return result;
	}

	// global events
	result = PviSetGlobEventMsg(POBJ_EVENT_PVI_ARRANGE, PviObject::GlobalEvent, SET_PVICALLBACK_DATA, 0);

	if (result)
	{
		AddRowToLog("PviSetGlobEventMsg", result);
		return result;
	}

	// timer for cyclic requests
	timerid_cyclic_requests = SetTimer(nullptr, 0, 500, PviObject::CyclicRequests);

	return 0; // Alles OK
}

/* stop PVI */
int PviObject::Stop(void)
{

	int result;
	// kill timer for cyclic requests
	KillTimer(nullptr, timerid_cyclic_requests);

	while (!objects.empty())
		objects.front()->Remove();
	readIndex = 0;
	childReadIndex = 0;

	result = PviDeinitialize();

	// delete all data types
	AddPviDatatype(nullptr);

	AddRowToLog("PviDeinitialize", result);
	return result;
}

/* =================================================================================
   functions to work with data types
   ==================================================================================
*/

static const char *AddPviDatatype(const char *t)
{
	size_t length;
	PVISTRUCTNAMES *p, *kill;

	if (t == nullptr)
	{ // delete all data types
		p = structnamesroot.next;
		while (p != nullptr)
		{
			kill = p;
			p = kill->next;
			if (kill->pname != nullptr)
				free(kill->pname);
			free(kill);
		}
		structnamesroot.next = nullptr;
		return ("nil!");
	}

	length = strlen(t); // length of data type's name

	// search for data type
	p = &structnamesroot;
	while (p->next != nullptr)
	{
		p = p->next;
		if (p->pname != nullptr)
		{
			if (!strcmp(p->pname, t))
			{ // already exists
				return (p->pname);
			}
		}
		else
		{
			return ("ERROR!");
		}
	}

	// insert data type
	p->next = static_cast<PVISTRUCTNAMES *>(malloc(sizeof(PVISTRUCTNAMES)));
	if (p->next != nullptr)
	{
		p = p->next;
		p->pname = static_cast<char *>(malloc(length + 1)); // space for name
		p->next = nullptr;
		if (p->pname != nullptr)
		{
			return (strcpy(p->pname, t));
		}
	}

	return ("ERROR!");
}

/* ===================================================================================
	functions to extract object information
	==================================================================================
*/

/* gets the data type of object from CPU */
void PviObject::ReadDataType()
{
	char buffer[256];
	int result;
	char *s;

	result = PviRead(this->linkid, POBJ_ACC_TYPE_EXTERN, nullptr, 0, buffer, sizeof(buffer));
	if (result == 0)
	{
		s = buffer;

		while (*s)
		{
			if (*s == ' ')
			{
				++s; // ignore spaces
				continue;
			}

			if (FindToken(&s, "{"))
			{ // start of struct definition
				break;
			}

			if (FindToken(&s, KWDESC_PVLEN "="))
			{ // variable's byte length
				this->ex.pv.length = GetIntValue(&s);
				continue;
			}

			if (FindToken(&s, KWDESC_PVTYPE "="))
			{ // variable type
				if (FindToken(&s, KWPVTYPE_BOOLEAN))
				{
					this->ex.pv.type = BR_BOOL;
					this->ex.pv.length = 1;
					this->ex.pv.pdatatype = "BOOL";
				}
				else if (FindToken(&s, KWPVTYPE_UINT8))
				{
					this->ex.pv.type = BR_USINT;
					this->ex.pv.length = 1;
					this->ex.pv.pdatatype = "USINT";
				}
				else if (FindToken(&s, KWPVTYPE_INT8))
				{
					this->ex.pv.type = BR_SINT;
					this->ex.pv.length = 1;
					this->ex.pv.pdatatype = "SINT";
				}
				else if (FindToken(&s, KWPVTYPE_UINT16))
				{
					this->ex.pv.type = BR_UINT;
					this->ex.pv.length = 2;
					this->ex.pv.pdatatype = "UINT";
				}
				else if (FindToken(&s, KWPVTYPE_INT16))
				{
					this->ex.pv.type = BR_INT;
					this->ex.pv.length = 2;
					this->ex.pv.pdatatype = "INT";
				}
				else if (FindToken(&s, KWPVTYPE_UINT32))
				{
					this->ex.pv.type = BR_UDINT;
					this->ex.pv.length = 4;
					this->ex.pv.pdatatype = "UDINT";
				}
				else if (FindToken(&s, KWPVTYPE_INT32))
				{
					this->ex.pv.type = BR_DINT;
					this->ex.pv.length = 4;
					this->ex.pv.pdatatype = "DINT";
				}
				else if (FindToken(&s, KWPVTYPE_FLOAT32))
				{
					this->ex.pv.type = BR_REAL;
					this->ex.pv.length = 4;
					this->ex.pv.pdatatype = "REAL";
				}
				else if (FindToken(&s, KWPVTYPE_FLOAT64))
				{
					this->ex.pv.type = BR_LREAL;
					this->ex.pv.length = 8;
					this->ex.pv.pdatatype = "LREAL";
				}
				else if (FindToken(&s, KWPVTYPE_DATI))
				{
					this->ex.pv.type = BR_DATI;
					this->ex.pv.length = 4;
					this->ex.pv.pdatatype = "DATE_AND_TIME";
				}
				else if (FindToken(&s, KWPVTYPE_DATE))
				{
					this->ex.pv.type = BR_DATE;
					this->ex.pv.length = 4;
					this->ex.pv.pdatatype = "DATE";
				}
				else if (FindToken(&s, KWPVTYPE_TIME))
				{
					this->ex.pv.type = BR_TIME;
					this->ex.pv.length = 4;
					this->ex.pv.pdatatype = "TIME";
				}
				else if (FindToken(&s, KWPVTYPE_TOD))
				{
					this->ex.pv.type = BR_TOD;
					this->ex.pv.length = 4;
					this->ex.pv.pdatatype = "TOD";
				}
				else if (FindToken(&s, KWPVTYPE_STRUCTURE))
				{
					this->ex.pv.type = BR_STRUCT;
					if (this->ex.pv.pdatatype == nullptr)
						this->ex.pv.pdatatype = "DATATYPE";
				}
				else if (FindToken(&s, KWPVTYPE_STRING))
				{
					this->ex.pv.type = BR_STRING;
					this->ex.pv.pdatatype = "STRING";
				}
				else if (FindToken(&s, KWPVTYPE_WSTRING))
				{
					this->ex.pv.type = BR_WSTRING;
					this->ex.pv.pdatatype = "WSTRING";
				}
				else
				{
					this->ex.pv.type = static_cast<PVIPVARTYPE>(0);
					this->ex.pv.pdatatype = "UNKNOWN";
					while (*s != ' ' && *s != 0)
						++s; // ignore rest
				}
				continue;
			}

			if (FindToken(&s, KWDESC_SCOPE "="))
			{ // scope of variable
				this->ex.pv.scope[0] = *s++;
				this->ex.pv.scope[1] = 0;
				while (*s != ' ' && *s != 0)
					++s; // ignore rest
				continue;
			}

			if (FindToken(&s, KWDESC_PVCNT "="))
			{ // number of elements
				this->ex.pv.dimension = GetIntValue(&s);
				continue;
			}
			++s;
		} /* end while()*/

		if (this->ex.pv.type == BR_STRUCT)
		{
			// this->ReadStructElements();
		}
	}
}

/* creates the children of the object as seperate objects */
PviObject *PviObject::Expand(unsigned hostsPerAdapter, bool allowBeyondSubnet)
{
	int i;
	PviObject tempobject;

	if (gui_info.has_childs != 0)
		return nullptr;

	switch (type)
	{
	case POBJ_PVAR:
		if (ex.pv.dimension > 1)
		{
			if (ex.pv.type == BR_STRUCT)
				ReadStructElements();

			for (i = 0; i < ex.pv.dimension; ++i)
			{
				tempobject = *this;
				tempobject.ex.pv.dimension = 1;
				snprintf(tempobject.name, sizeof(tempobject.name), "%s[%i]", name, i);
				snprintf(tempobject.descriptor, sizeof(tempobject.descriptor), "%s[%i]", descriptor, i);
				if (tempobject.Add(FALSE) != nullptr)
					gui_info.has_childs = 1;
			}
		}
		else if (ex.pv.type == BR_STRUCT)
		{
			if (ReadStructElements() != nullptr)
				gui_info.has_childs = 1;
		}
		break;

	case POBJ_CPU:
		if (ReadTaskList() != nullptr)
			gui_info.has_childs = 1;
		gui_info.has_childs = 0;
		break;

	case POBJ_TASK:
		if (ReadPvarList() == nullptr)
			gui_info.has_childs = 1;
		break;

	case POBJ_DEVICE:
		ReadCpuList(hostsPerAdapter, allowBeyondSubnet);
		gui_info.has_childs = 0;
		break;

	default:
		break;
	}
	return nullptr;
}

PviObject *PviObject::AddCpuByIp(const char *ipAddress, unsigned timeoutMs)
{
	PviObject *deviceObject = nullptr;
	for (const auto &object : objects)
	{
		if (object->type == POBJ_DEVICE)
		{
			deviceObject = object.get();
			break;
		}
	}
	if (deviceObject == nullptr || deviceObject->linkid == 0)
	{
		MessageBoxA(MainWindowGetHandle(), "The PVI Ethernet device is not ready.", "Add Cpu", MB_OK | MB_ICONWARNING);
		return nullptr;
	}

	PviObject cpuObject = {};
	strncpy(cpuObject.ex.cpu.ethernetCpuInfo.ipAddress, ipAddress, sizeof(cpuObject.ex.cpu.ethernetCpuInfo.ipAddress) - 1);
	snprintf(cpuObject.descriptor, sizeof(cpuObject.descriptor), "/IP=%s /COMT=%u", ipAddress, timeoutMs);
	for (const auto &existing : objects)
	{
		if (existing->type == POBJ_CPU && strcmp(existing->descriptor, cpuObject.descriptor) == 0)
			return existing.get();
	}
	char uniqueName[256];
	CreateUniqueObjectName(uniqueName, cpuObject.descriptor);
	snprintf(cpuObject.name, sizeof(cpuObject.name), "%s/CPU%s", deviceObject->name, uniqueName);
	cpuObject.type = POBJ_CPU;
	PviObject *created = cpuObject.Add(TRUE);
	if (created == nullptr)
	{
		PviObject *failed = PviObject::FindByName(cpuObject.name);
		if (failed != nullptr)
			failed->Remove();
	}
	return created;
}

/* searches for reachable CPUs
	input: pointer to device object
	result : pointer to device object or NULL in case of error

*/

PviObject *PviObject::ReadCpuList(unsigned hostsPerAdapter, bool allowBeyondSubnet)
{
	char *s;
	char *pbuffer;


	if (this->type != POBJ_DEVICE)
		return nullptr;

	pbuffer = static_cast<char *>(malloc(65535));
	if (pbuffer == nullptr)
	{
		AddRowToLog("PviObject::ReadCpuList:malloc(65535)", -1);
		return nullptr;
	}

	s = _strupr(this->descriptor);
	while (*s)
	{
		// search for CPUs connected to a serial port
		// if (FindToken(&s, "/IF=COM"))
		// {

		// 	CreateUniqueObjectName(tempstring, cpuObject->descriptor);
		// 	snprintf(cpuObject->name, sizeof(cpuObject->name), "%s/CPU%s", this->name, tempstring);

		// 	cpuObject->type = POBJ_CPU;
		// 	snprintf(descriptor, sizeof(descriptor), "CD=\"%s\"", cpuObject->descriptor);

		// 	result = PviCreate(&cpuObject->linkid, cpuObject->name, cpuObject->type, descriptor, PviObject::Callback, SET_PVICALLBACK_DATA, 0, "Ev=e");
		// 	if (result == 0)
		// 	{
		// 		result = PviRead(cpuObject->linkid, POBJ_ACC_VERSION, NULL, 0, pbuffer, 65535);
		// 		if (result == 0)
		// 		{
		// 			if (cpuObject->Add(1) == NULL)
		// 			{
		// 				this = NULL; // Fehler
		// 			}
		// 			break;
		// 		}
		// 	}
		// 	switch (result)
		// 	{
		// 	case 4808:
		// 		MessageBoxA(NULL, "No CPU found !", "Error", MB_OK | MB_ICONEXCLAMATION);
		// 		break;

		// 	case 13076:
		// 		MessageBoxA(NULL, "Device does not exist !", "Error", MB_OK | MB_ICONEXCLAMATION);
		// 		break;

		// 	default:
		// 		AddRowToLog("PviReadCPUList()", result);
		// 		break;
		// 	}
		// 	break;
		// }

		// search for CPUs connected via ethernet
		if (FindToken(&s, "/IF=TCPIP"))
		{
			struct stEthernetCpuInfo *buffer = static_cast<struct stEthernetCpuInfo *>(calloc(256, sizeof(struct stEthernetCpuInfo)));
			struct stEthernetCpuInfo *ethernetCpuInfo = buffer;

			if (ethernetCpuInfo != nullptr)
			{

				int noOfCpu = SearchEthernetCpus(ethernetCpuInfo, 256, hostsPerAdapter, allowBeyondSubnet);
				if (noOfCpu >= 0)
					AddEthernetCpus(ethernetCpuInfo, noOfCpu);
				free(buffer);
			}
			break;
		}

		++s;
	}

	free(pbuffer);
	return this;
}

PviObject *PviObject::AddEthernetCpus(const stEthernetCpuInfo *cpus, int entries)
{
	if (type != POBJ_DEVICE || entries < 0 || (entries != 0 && cpus == nullptr))
	{
		AddRowToLog("PviObject::AddEthernetCpus:invalid input", -1);
		return nullptr;
	}
	for (int i = 0; i < entries; ++i)
	{
		PviObject cpuObject;
		cpuObject.ex.cpu.ethernetCpuInfo = cpus[i];
		snprintf(cpuObject.descriptor, sizeof(cpuObject.descriptor), "/IP=%s", cpus[i].ipAddress);
		strcpy(cpuObject.ex.cpu.arversion, cpus[i].arVersion);
		strcpy(cpuObject.ex.cpu.cputype, cpus[i].targetTypeDescription);
		if (cpus[i].macAddress[0] != '\0')
			snprintf(cpuObject.name, sizeof(cpuObject.name), "%s/CPU%s", name, cpus[i].macAddress);
		else
		{
			char uniqueName[256];
			CreateUniqueObjectName(uniqueName, cpuObject.descriptor);
			snprintf(cpuObject.name, sizeof(cpuObject.name), "%s/CPU%s", name, uniqueName);
		}
		cpuObject.type = POBJ_CPU;
		char descriptor[256];
		snprintf(descriptor, sizeof(descriptor), "CD=\"%s\"", cpuObject.descriptor);
		int result = PviCreate(&cpuObject.linkid, cpuObject.name, cpuObject.type, descriptor,
			PviObject::Callback, SET_PVICALLBACK_DATA, 0, "Ev=s");
		if (result == 0)
			cpuObject.Add(1);
		else
			AddRowToLog("PviObject::AddEthernetCpus:PviCreate", result);
	}
	gui_info.has_childs = 0;
	return this;
}

/* Reads a list of tasks available on the CPU */
PviObject *PviObject::ReadTaskList()
{
	PviObject tempobject;
	PviObject *o;
	int result = 0;
	char *pbuffer;
	char *s, *d;


	if (this->type != POBJ_CPU)
		return nullptr;

	pbuffer = (char *)malloc(65535);

	if (pbuffer == nullptr)
	{
		AddRowToLog("PviObject::ReadTaskList:malloc(65535)", -1);
		return nullptr; // error
	}

	/* get version of Automation Runtime  */
	this->ex.cpu.arversion[0] = 0;
	result = PviRead(this->linkid, POBJ_ACC_VERSION, nullptr, 0, this->ex.cpu.arversion, sizeof(this->ex.cpu.arversion) - 1);

	if (result != 0)
	{
		return nullptr;
	}

	/* get CPU type */
	this->ex.cpu.cputype[0] = 0;
	result = PviRead(this->linkid, POBJ_ACC_CPU_INFO, nullptr, 0, pbuffer, 65535);
	if (result == 0)
	{
		s = pbuffer;
		while (s)
		{
			if (FindToken(&s, "CT="))
			{
				strcpy(this->ex.cpu.cputype, s);
				break;
			}
			++s;
		}
	}

	/* get CPU status */
	result = PviRead(this->linkid, POBJ_ACC_STATUS, nullptr, 0, pbuffer, 65535);
	if (result != 0)
	{
		return nullptr;
	}

	/* read list of tasks */
	if (this->error != 0)
		return nullptr;

	result = PviRead(this->linkid, POBJ_ACC_LIST_TASK, nullptr, 0, pbuffer, 65535);
	if (result == 0)
	{
		s = pbuffer;
		while (*s)
		{
			tempobject = PviObject();
			d = tempobject.descriptor;
			do
			{
				*d++ = *s++;
			} while ((*s != 0) && (*s != '\t'));
			++s;	// ignore tabs
			*d = 0; // terminate with zero
			strcpy(tempobject.name, this->name);
			strcat(tempobject.name, "/");
			strcat(tempobject.name, tempobject.descriptor);
			tempobject.type = POBJ_TASK;

			if ((o = tempobject.Add(1)) != nullptr)
			{
				o->ex.task.cpu = this;		 // store pointer to CPU this
				this->gui_info.has_childs = 1; // children were found
			}
		}
	}
	else
	{
		AddRowToLog("PviObject::ReadTaskList", result);
	}

	free(pbuffer);
	return this; // return pointer to CPU this
}

/* reads the list of variables that are accessible via the specified object */
PviObject *PviObject::ReadPvarList()
{
	PviObject newobject;
	char *pbuffer, *s, *d;
	int result;


	if (this->type != POBJ_CPU && this->type != POBJ_TASK)
		return nullptr;

	pbuffer = (char *)malloc(65535);

	if (pbuffer == nullptr)
	{
		AddRowToLog("PviObject::ReadPvarList:malloc(65535)", -1);
		return nullptr; // error
	}

	memset(pbuffer, 0, 65535);

	/* link this */
	this->Add(1);
	/* read list of variables */
	result = PviRead(this->linkid, POBJ_ACC_LIST_PVAR, nullptr, 0, pbuffer, 65530);
	if (result == 0)
	{
		s = pbuffer;
		strcat(pbuffer, "\t");

		newobject = PviObject();
		while (*s)
		{
			if (*s == ' ')
			{
				++s; // ignore space character
				continue;
			}

			if (strlen(newobject.descriptor) == 0)
			{ // get variable name
				d = newobject.descriptor;
				while (*s && *s != ' ')
				{
					*d++ = *s++;
				}
				*d = 0; // terminate with zero;
				continue;
			}

			if (FindToken(&s, KWDESC_PVLEN "="))
			{ // variable's byte length
				newobject.ex.pv.length = GetIntValue(&s);
				continue;
			}

			if (FindToken(&s, KWDESC_PVTYPE "="))
			{ // variable type
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
					if (newobject.ex.pv.pdatatype == nullptr)
						newobject.ex.pv.pdatatype = "DATATYPE";
				}
				else if (FindToken(&s, KWPVTYPE_STRING))
				{
					newobject.ex.pv.type = BR_STRING;
					newobject.ex.pv.pdatatype = "STRING";
				}
				else if (FindToken(&s, KWPVTYPE_WSTRING))
				{
					newobject.ex.pv.type = BR_WSTRING;
					newobject.ex.pv.pdatatype = "WSTRING";
				}
				else
				{
					newobject.ex.pv.type = static_cast<PVIPVARTYPE>(0); // unknown
					newobject.ex.pv.pdatatype = "UNKNOWN";
					while (*s != ' ' && *s != 0)
						++s; // ignore rest
				}
				continue;
			}

			if (FindToken(&s, KWDESC_SCOPE "="))
			{ // variable scope
				newobject.ex.pv.scope[0] = *s++;
				newobject.ex.pv.scope[1] = 0;
				while (*s != ' ' && *s != 0)
					++s; // ignore rest
				continue;
			}

			if (FindToken(&s, KWDESC_PVCNT "="))
			{ // number of elements
				newobject.ex.pv.dimension = GetIntValue(&s);
				continue;
			}

			if (FindToken(&s, "\t") || *(s + 1) == 0)
			{ // end of entry or of complete list
				newobject.type = POBJ_PVAR;
				strcpy(newobject.name, this->name);
				strcat(newobject.name, "/");
				strcat(newobject.name, newobject.descriptor);
				if (this->type == POBJ_CPU)
				{								   // global variable ?
					newobject.ex.pv.task = this; // since task does not exist we store CPU this instead
					newobject.ex.pv.cpu = this;  // store CPU this
				}
				else
				{ // local variable
					PviObject *cpu = this->ex.task.cpu;

					newobject.ex.pv.task = this; // store pointer to task this
					newobject.ex.pv.cpu = cpu;	   // store pointer to CPU this
				}
				if (newobject.Add(0) != nullptr)
				{
					this->gui_info.has_childs = 1; // children were created as separate objects
				}
				newobject = PviObject();
				continue;
			}

			++s;
		} /* end of while (*s != 0)... */
	}
	else
	{
		AddRowToLog("PviObject::ReadPvarList", result);
	}

	// Speicher freigeben
	free(pbuffer);
	return (this);
}

/* read struct elements and use them as separate objects */
PviObject *PviObject::ReadStructElements()
{
	const unsigned int buffer_size = 258084;
	char *pbuffer;
	int result;
	char *s, *d;
	PviObject newobject;
	PviObject *o;
	BOOL element_definition;


	if (this->type != POBJ_PVAR && this->ex.pv.type != BR_STRUCT)
		return nullptr;

	pbuffer = (char *)calloc(buffer_size, 1);

	if (pbuffer == nullptr)
	{
		AddRowToLog("PviObject::ReadStructElements:calloc(buffer_size)", -1);
		return nullptr; // error
	}

	newobject = *this;
	strcpy(newobject.descriptor, "CD=\"");
	strcat(newobject.descriptor, this->descriptor);
	strcat(newobject.descriptor, "\"");

	result = PviCreate(&newobject.linkid, newobject.name, newobject.type, newobject.descriptor, PviObject::Callback, SET_PVICALLBACK_DATA, 0, "Ev=e");
	if (result == 0 || result == 12002)
	{
		result = PviRead(newobject.linkid, POBJ_ACC_TYPE_EXTERN, nullptr, 0, pbuffer, buffer_size );
	}
	PviDelete(newobject.name);

	if (result == 0)
	{
		if (memchr(pbuffer, '\0', buffer_size) == nullptr)
		{
			AddRowToLog("PviObject::ReadStructElements:unterminated type description", -1);
			free(pbuffer);
			return nullptr;
		}

		// AddRowToLog( pbuffer, 0 );

		s = pbuffer;
		element_definition = FALSE;

		while (*s != 0)
		{
			if (*s == ' ')
			{
				++s; // ignore space character
				continue;
			}

			if (!element_definition)
			{ // Strukturname des Parents
				if (FindToken(&s, KWDESC_SNAME "="))
				{
					d = tempstring;
					while (*s && *s != ' ')
					{
						*d++ = *s++;
					}
					*d = 0;
					this->ex.pv.pdatatype = AddPviDatatype(tempstring);
				}
			}

			if (FindToken(&s, "{"))
			{ // start of element definition
				element_definition = TRUE;
				newobject = PviObject();
				memset(tempstring, 0, sizeof(tempstring));
				d = tempstring;
				while (*s && *s == ' ') // ignore space character
					++s;
				while (*s && *s != ' ')
					*d++ = *s++;
				*d = 0;
				strcpy(newobject.descriptor, this->descriptor); // create descriptor
				strcat(newobject.descriptor, tempstring);
				strcpy(newobject.name, this->name); // create name
				strcat(newobject.name, tempstring);
				newobject.type = POBJ_PVAR;
				// count number of points. If > then element is a child
				if (CountToken(tempstring, '.') > 1)
				{
					char *end = strchr(s, '}');
					if (end == nullptr)
					{
						AddRowToLog("PviObject::ReadStructElements:missing closing brace", -1);
						free(pbuffer);
						return nullptr;
					}
					s = end + 1;
					element_definition = FALSE;
				}
				continue;
			}

			else

				if (element_definition == FALSE)
			{
				++s;
				continue;
			}

			if (FindToken(&s, KWDESC_PVLEN "="))
			{ // variable byte length
				newobject.ex.pv.length = GetIntValue(&s);
				continue;
			}

			if (FindToken(&s, KWDESC_PVCNT "="))
			{ // number of elements
				newobject.ex.pv.dimension = GetIntValue(&s);
				continue;
			}

			if (FindToken(&s, KWDESC_PVTYPE "="))
			{ // variable type
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
					if (newobject.ex.pv.pdatatype == nullptr)
						newobject.ex.pv.pdatatype = "DATATYPE(STRUCT)";
				}
				else if (FindToken(&s, KWPVTYPE_STRING))
				{
					newobject.ex.pv.type = BR_STRING;
					newobject.ex.pv.pdatatype = "STRING";
				}
				else if (FindToken(&s, KWPVTYPE_WSTRING))
				{
					newobject.ex.pv.type = BR_WSTRING;
					newobject.ex.pv.pdatatype = "WSTRING";
				}
				else
				{
					newobject.ex.pv.type = static_cast<PVIPVARTYPE>(0); // unknown
					newobject.ex.pv.pdatatype = "UNKNOWN";
					while (*s != ' ' && *s != 0)
						++s; // ignore rest
				}
				continue;
			}

			if (element_definition && newobject.ex.pv.type == BR_STRUCT)
			{ // Structure name of the element
				if (FindToken(&s, KWDESC_SNAME "="))
				{
					d = tempstring;
					while (*s && *s != ' ')
					{
						*d++ = *s++;
					}
					*d = 0;
					newobject.ex.pv.pdatatype = AddPviDatatype(tempstring);
				}
			}
			if (FindToken(&s, KWDESC_PVCNT "="))
			{ // Number of elements
				newobject.ex.pv.dimension = GetIntValue(&s);
				continue;
			}

			if (FindToken(&s, "}") || *(s + 1) == 0)
			{ // End of the entry or the entire list
				PviObject newelement;
				int i;
				//				int j;
				element_definition = FALSE;

				for (i = 0; i < this->ex.pv.dimension; ++i)
				{
#ifdef RAUS
					for (j = 0; j < newobject.ex.pv.dimension; ++j)
					{
#endif
						newelement = newobject;
#ifdef RAUS
						newelement.ex.pv.dimension = 1; // Dimension wird nicht geerbt
#endif

						if (this->ex.pv.dimension > 1)
						{																			   // Elternobject ist ein Array
							strcpy(tempstring, newobject.descriptor + strlen(this->descriptor) + 1); // Name des Elementes
							snprintf(newelement.name, sizeof(newelement.name), "%s[%i].%s", this->name, i, tempstring);
							snprintf(newelement.descriptor, sizeof(newelement.descriptor), "%s[%i].%s", this->descriptor, i, tempstring);
						}

#ifdef RAUS
						if (newobject.ex.pv.dimension > 1)
						{ // Kindobject ist ein Array
							snprintf(tempstring, sizeof(tempstring), "[%i]", j);
							strcat(newelement.name, tempstring);
							strcat(newelement.descriptor, tempstring);
						}
#endif

						strncpy(newelement.ex.pv.scope, this->ex.pv.scope, 1); // Scope is inherited
						newelement.ex.pv.task = this->ex.pv.task;				 // Task is inherited
						newelement.ex.pv.cpu = this->ex.pv.cpu;				 // CPU is inherited
						if ((o = newelement.Add(FALSE)) != nullptr)
						{
							// this->gui_info.has_childs = 1;  // mind. 1 "Kind" wurde fÃƒÆ’Ã†â€™Ãƒâ€ Ã¢â‚¬â„¢ÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â¼r das Objekt angelegt
							//
							// if( o->ex.pv.type == BR_STRUCT ){
							//	o->gui_info.has_childs = 1; // Strukturen haben mind. 1 Kind
							// }

							// sprintf( tempstring, "%s[%i]", this->name, i );
							// if( (o = PviObject::FindByName( tempstring )) != NULL ){
							//	o->gui_info.has_childs = 1;  // Arrayelemente haben mind. 1 "Kind"
							// }
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

	if (result)
	{
		AddRowToLog("PviObject::ReadStructElements:PviCreate/PviRead", result);
	}

	free(pbuffer);
	return result ? nullptr : this;
}

/* ======================================================================================
   log file handling
   ====================================================================================== */

/* writes into log file */
static void AddRowToLog(const char *row, int errcode)
{
	FILE *f;
	char zeit[10];
	char datum[10];
	char *pmessage;
	char filename[MAX_PATH];

	strcpy(filename, GetApplicationPath());
	strcat(filename, "\\" PVILOGFILE);

	_strtime(zeit);
	_strdate(datum);

	f = fopen(filename, "a+");

	if (f != nullptr)
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
		const char *errtext;
		pmessage = (char *)malloc(512);

		if (pmessage != nullptr)
		{
			switch (errcode)
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
			sprintf(pmessage, "Error in PVI-Interface\nError: %u\n\"%s\"\n(%s)", errcode, errtext, row);
			MessageBoxA(nullptr, pmessage, "Error", MB_OK | MB_ICONERROR);
		}
		free(pmessage);
	}
}

static void DeleteLog(void)
{
	char filename[MAX_PATH];

	strcpy(filename, GetApplicationPath());
	strcat(filename, "\\" PVILOGFILE);
	remove(filename);
}

/* =====================================================================================
	watch file handling
	====================================================================================
*/

/* writes object to watch file */
int PviObject::AddToWatchFile(char *filename)
{
	PviObject *parent;
	const char *section;
	int entries;
	//	int result;
	int i;

	// store file version info
	WritePrivateProfileStringA("File", "Application", "BRWATCH.EXE", filename);	 // type
	WritePrivateProfileStringA("File", "Version", WATCH_FILE_VERSION, filename); // version

	parent = PviObject::GetNext(TRUE);

	// Eltern suchen und ggfs. speichern
	while (parent != nullptr)
	{
		if (parent->IsParentOf(this) && strrchr(parent->name, '.') == nullptr)
		{ // find parents
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
				section = nullptr;
				break;
			}
			if (section != nullptr)
			{
				char keyname[80];
				BOOL found = FALSE;
				tempstring[0] = 0;
				GetPrivateProfileStringA(section, section, "0", tempstring, sizeof(tempstring), filename);
				entries = atoi(tempstring);
				// does entry exist ?
				for (i = 1; i <= entries; ++i)
				{
					sprintf(keyname, "name%u", i);
					GetPrivateProfileStringA(section, keyname, "", tempstring, sizeof(tempstring), filename);
					if (strcmp(parent->name, tempstring) == 0)
					{
						found = TRUE;
					}
				}

				if (!found)
				{ // not found
					++entries;
					sprintf(tempstring, "%u", entries);
					WritePrivateProfileStringA(section, section, tempstring, filename);
					// Eltern-Objekt speichern
					sprintf(keyname, "name%u", entries);
					WritePrivateProfileStringA(section, keyname, parent->name, filename); // name
					sprintf(keyname, "desc%u", entries);
					WritePrivateProfileStringA(section, keyname, parent->descriptor, filename); // descriptor
					sprintf(keyname, "sort%u", entries);
					WritePrivateProfileStringA(section, keyname, "-1", filename); // sort criteria
				}
			}
		}
		parent = PviObject::GetNext(FALSE);
	}

	// Objekt und Sortierkritierium speichern
	switch (this->type)
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
		section = nullptr;
		break;
	}
	if (section != nullptr)
	{
		char keyname[80];
		tempstring[0] = 0;
		// number of entries
		GetPrivateProfileStringA(section, section, "0", tempstring, sizeof(tempstring), filename);
		entries = atoi(tempstring);
		++entries;
		sprintf(tempstring, "%u", entries);
		WritePrivateProfileStringA(section, section, tempstring, filename);
		// Objekt speichern
		sprintf(keyname, "name%u", entries);
		WritePrivateProfileStringA(section, keyname, this->name, filename); // name
		sprintf(keyname, "desc%u", entries);
		WritePrivateProfileStringA(section, keyname, this->descriptor, filename); // descriptor
		sprintf(keyname, "sort%u", entries);
		sprintf(tempstring, "%u", this->watchsort);
		WritePrivateProfileStringA(section, keyname, tempstring, filename); // sort criteria
	}

	return 0;
}

/* load objects of filtered type from watch file
and returns number of objects read */
int PviObject::LoadFromWatchFile(T_POBJ_TYPE typefilter, char *filename)
{
	const char *section = "";
	PviObject tempobject, *object, *parent;
	int entries, i, sort;
	int n = 0; // number of objects to be inserted

	switch (typefilter)
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

	// read number of objects in this section
	tempstring[0] = 0;
	GetPrivateProfileStringA(section, section, "0", tempstring, sizeof(tempstring), filename);
	entries = atoi(tempstring);
	if (entries == 0)
	{
		sprintf(tempstring, "No entries found in section [%s] !", section);
		MessageBoxA(nullptr, tempstring, "Error Loading Watch File", MB_OK | MB_ICONERROR);
		return -1;
	}
	for (i = 1; i <= entries; ++i)
	{
		char keyname[80];

		tempobject = PviObject();
		// read name
		sprintf(keyname, "name%u", i);
		GetPrivateProfileStringA(section, keyname, "", tempobject.name, sizeof(tempobject.name), filename);
		if (strlen(tempobject.name) == 0)
		{
			sprintf(tempstring, "section [%s]\n%s=\ninvalid or not found !", section, keyname);
			MessageBoxA(nullptr, tempstring, "Error Loading Watch File", MB_OK | MB_ICONERROR);
			return -1;
		}
		// read descriptor
		sprintf(keyname, "desc%u", i);
		GetPrivateProfileStringA(section, keyname, "", tempobject.descriptor, sizeof(tempobject.descriptor), filename);
		if (strlen(tempobject.descriptor) == 0)
		{
			sprintf(tempstring, "section [%s]\n%s=\ninvalid or not found !", section, keyname);
			MessageBoxA(nullptr, tempstring, "Error Loading Watch File", MB_OK | MB_ICONERROR);
			return -1;
		}
		// read sort criteria
		sprintf(keyname, "sort%u", i);
		GetPrivateProfileStringA(section, keyname, "-1", tempstring, sizeof(tempstring), filename);
		sort = atoi(tempstring);

		// add object
		tempobject.type = typefilter;
		object = tempobject.Add(1);

		if (object != nullptr)
		{
			object->gui_info.loaded_from_file = 1;
			object->watchsort = sort;
			if (object->watchsort >= 0)
				++n;
			object->ReadDataType();
		}

		// find the corresponding task and CPU
		if (object->type == POBJ_PVAR)
		{

			parent = PviObject::GetNext(TRUE);
			while (parent != nullptr)
			{
				if (parent->IsParentOf(object) && parent->type == POBJ_CPU)
				{ // find parents
					object->ex.pv.cpu = parent;
				}
				parent = PviObject::GetNext(FALSE);
			}

			parent = PviObject::GetNext(TRUE);
			while (parent != nullptr)
			{
				if (parent->IsParentOf(object) && parent->type == POBJ_TASK)
				{ // find parents
					object->ex.pv.task = parent;
				}
				parent = PviObject::GetNext(FALSE);
			}
		}

		// find the corresponding CPU
		if (object->type == POBJ_TASK)
		{

			parent = PviObject::GetNext(TRUE);
			while (parent != nullptr)
			{
				if (parent->IsParentOf(object) && parent->type == POBJ_CPU)
				{ // find parents
					object->ex.task.cpu = parent;
				}
				parent = PviObject::GetNext(FALSE);
			}
		}
	}
	return n;
}
