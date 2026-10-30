#include "main.h"
#include "mylistview.h"
#include "object_treeview.h"
#include "dlg_about.h"
#include "resource.h"
#include "settings.h"
#include <shellapi.h>
#include <type_traits>
#include <vector>

static_assert(std::is_standard_layout<PviObject>::value, "Keep the Win32-facing data layout");
static_assert(std::is_trivially_copyable<PviObject>::value, "Object descriptions remain value-copyable");

typedef void(WINAPI *PviTestCallback)(WPARAM, LPARAM, LPVOID, DWORD, T_RESPONSE_INFO *);

static DWORD nextLinkId = 1;
static PviTestCallback callback;
static const char *readResponse;
static unsigned dataNotifications;
static unsigned errorNotifications;
static DWORD detachedLinkId;
static unsigned expectedErrorMessages;
static unsigned createRequests;
static const char *expectedMessageCaption = "Error";
static unsigned browserLaunches;
static INT_PTR browserResult = 33;

static void Check(bool condition, const char *expression, int line)
{
	if (!condition)
	{
		fprintf(stderr, "Line %d: %s\n", line, expression);
		exit(EXIT_FAILURE);
	}
}

#define CHECK(expression) Check((expression), #expression, __LINE__)

static int WINAPI TestMessageBoxA(HWND, LPCSTR, LPCSTR caption, UINT)
{
	CHECK(expectedErrorMessages != 0);
	CHECK(strcmp(caption, expectedMessageCaption) == 0);
	--expectedErrorMessages;
	return IDOK;
}

static HINSTANCE WINAPI TestShellExecuteW(HWND, LPCWSTR operation, LPCWSTR url, LPCWSTR, LPCWSTR, INT show)
{
	CHECK(lstrcmpW(operation, L"open") == 0);
	CHECK(lstrcmpW(url, L"https://github.com/hilch/brwatch") == 0);
	CHECK(show == SW_SHOWNORMAL);
	++browserLaunches;
	return reinterpret_cast<HINSTANCE>(browserResult);
}

extern "C"
{
	decltype(&MessageBoxA) __imp_MessageBoxA = TestMessageBoxA;
	decltype(&ShellExecuteW) __imp_ShellExecuteW = TestShellExecuteW;
}

extern "C" INT WINAPI PviInitialize(LONG, LONG, LPCSTR, LPVOID)
{
	return 0;
}

extern "C" INT WINAPI PviDeinitialize()
{
	return 0;
}

extern "C" VOID WINAPI PviGetVersion(LPSTR buffer, DWORD size)
{
	snprintf(buffer, size, "PVI test");
}

extern "C" INT WINAPI PviSetGlobEventMsg(DWORD, LPVOID, DWORD, LPARAM)
{
	return 0;
}

extern "C" INT WINAPI PviCreate(LPDWORD link, LPCSTR, DWORD, LPCSTR,
							   LPVOID eventCallback, DWORD, LPARAM, LPCSTR)
{
	++createRequests;
	*link = nextLinkId++;
	callback = reinterpret_cast<PviTestCallback>(eventCallback);
	return 0;
}

extern "C" INT WINAPI PviRead(DWORD, DWORD, LPVOID, LONG, LPVOID buffer, LONG size)
{
	CHECK(readResponse != nullptr);
	CHECK(strlen(readResponse) < static_cast<size_t>(size));
	strcpy(static_cast<char *>(buffer), readResponse);
	return 0;
}

extern "C" INT WINAPI PviReadRequest(DWORD, DWORD, LPVOID, DWORD, LPARAM)
{
	return 0;
}

extern "C" INT WINAPI PviDelete(LPCSTR)
{
	return 0;
}

extern "C" INT WINAPI PviUnlink(DWORD linkid)
{
	if (detachedLinkId != 0 && linkid == detachedLinkId)
	{
		CHECK(PviObject::FindByLinkId(linkid) == nullptr);
		T_RESPONSE_INFO info = {};
		info.nType = POBJ_EVENT_ERROR;
		info.LinkID = linkid;
		info.ErrCode = 123;
		unsigned previousErrors = errorNotifications;
		callback(0, 0, nullptr, 0, &info);
		CHECK(errorNotifications == previousErrors);
	}
	return 0;
}

static void DataChanged(PviObject *)
{
	++dataNotifications;
}

static void ErrorChanged(PviObject *, int)
{
	++errorNotifications;
}

static PviObject *AddObject(const char *name, const char *descriptor, T_POBJ_TYPE type)
{
	PviObject description;
	strcpy(description.name, name);
	strcpy(description.descriptor, descriptor);
	description.type = type;
	PviObject *object = description.Add(FALSE);
	CHECK(object != nullptr);
	CHECK(object != &description);
	CHECK(PviObject::FindByName(name) == object);
	return object;
}

static void CheckRegistryGrowth(PviObject *watched, PviObject *cpu, PviObject *task, PviObject *axisName)
{
	const char *retainedName = watched->name;
	int previousCount = PviObject::Count();
	std::vector<PviObject *> added;
	for (unsigned i = 0; i < 2048; ++i)
	{
		char name[80];
		snprintf(name, sizeof(name), "@Pvi/Stress/%u", i);
		added.push_back(AddObject(name, "Stress", POBJ_TASK));
	}
	CHECK(PviObject::Count() == previousCount + 2048);
	CHECK(PviObject::FindByName(retainedName) == watched);
	CHECK(PviObject::FindByLinkId(watched->linkid) == watched);
	CHECK(PviObject::FindByName("@Pvi/Device/CPU") == cpu);
	CHECK(task->ex.task.cpu == cpu);
	CHECK(axisName->ex.pv.cpu == cpu && axisName->ex.pv.task == task);

	for (unsigned i : {0u, 1024u, 2047u})
	{
		added[i]->Remove();
		char name[80];
		snprintf(name, sizeof(name), "@Pvi/Stress/%u", i);
		CHECK(PviObject::FindByName(name) == nullptr);
	}
	CHECK(PviObject::Count() == previousCount + 2045);
	CHECK(PviObject::FindByName("@Pvi/Stress/1025") == added[1025]);
	CHECK(PviObject::FindByName(retainedName) == watched);
	CHECK(axisName->ex.pv.cpu == cpu && axisName->ex.pv.task == task);

	T_RESPONSE_INFO info = {};
	info.nType = POBJ_EVENT_DATA;
	info.LinkID = watched->linkid;
	LONG value = 84;
	callback(0, 0, &value, sizeof(value), &info);
	CHECK(*static_cast<LONG *>(watched->ex.pv.pvalue) == value);
}

static void CheckCursorRemoval()
{
	PviObject *parent = AddObject("@Pvi/Cursor", "Cursor", POBJ_TASK);
	PviObject *first = AddObject("@Pvi/Cursor/First", "First", POBJ_TASK);
	PviObject *second = AddObject("@Pvi/Cursor/Second", "Second", POBJ_TASK);
	PviObject *third = AddObject("@Pvi/Cursor/Third", "Third", POBJ_TASK);
	CHECK(parent->FindChild(TRUE) == first);
	PviObject::GetNext(TRUE);
	PviObject *object;
	while ((object = PviObject::GetNext(FALSE)) != first)
		CHECK(object != nullptr);
	CHECK(first->Remove() == parent);
	CHECK(PviObject::GetNext(FALSE) == second);
	CHECK(parent->FindChild(FALSE) == second);
	second->Remove();
	CHECK(PviObject::GetNext(FALSE) == third);
	CHECK(parent->FindChild(FALSE) == third);
	CHECK(PviObject::GetNext(FALSE) == nullptr);
	CHECK(parent->FindChild(FALSE) == nullptr);
	third->Remove();
	parent->Remove();
}

static void CheckStringCallbacks(PVIPVARTYPE type)
{
	PviObject *object = AddObject("@Pvi/StringBounds", "StringBounds", POBJ_PVAR);
	object->ex.pv.type = type;
	object->ex.pv.length = 32;
	object->ex.pv.dimension = 1;
	CHECK(object->Watch(TRUE) == object);
	object->ex.pv.length = 8;
	auto bytes = static_cast<unsigned char *>(object->ex.pv.pvalue);
	memset(bytes, 0x7e, 32);
	T_RESPONSE_INFO info = {};
	info.nType = POBJ_EVENT_DATA;
	info.LinkID = object->linkid;

	char oversized[16];
	memset(oversized, 'X', sizeof(oversized));
	unsigned previousNotifications = dataNotifications;
	expectedErrorMessages = 1;
	callback(0, 0, oversized, sizeof(oversized), &info);
	for (unsigned i = 8; i < 32; ++i)
		CHECK(bytes[i] == 0x7e);
	for (unsigned i = 0; i < 8; ++i)
		CHECK(bytes[i] == 0x7e);
	CHECK(expectedErrorMessages == 0);
	CHECK(dataNotifications == previousNotifications);

	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	auto pages = static_cast<char *>(VirtualAlloc(nullptr, systemInfo.dwPageSize * 2,
												MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
	CHECK(pages != nullptr);
	DWORD previousProtection;
	CHECK(VirtualProtect(pages + systemInfo.dwPageSize, systemInfo.dwPageSize,
						 PAGE_NOACCESS, &previousProtection) != 0);
	DWORD unitSize = type == BR_WSTRING ? sizeof(WCHAR) : sizeof(char);
	DWORD shortLength = unitSize * 2;
	char *shortData = pages + systemInfo.dwPageSize - shortLength;
	memset(shortData, 0, shortLength);
	shortData[0] = 'A';
	callback(0, 0, shortData, shortLength, &info);
	CHECK(bytes[0] == 'A');
	for (unsigned i = 1; i < object->ex.pv.length; ++i)
		CHECK(bytes[i] == 0);
	CHECK(VirtualFree(pages, 0, MEM_RELEASE) != 0);

	char unterminated[8];
	memset(unterminated, 'X', sizeof(unterminated));
	callback(0, 0, unterminated, sizeof(unterminated), &info);
	for (unsigned i = 0; i < 8 - unitSize; ++i)
		CHECK(bytes[i] == 'X');
	for (unsigned i = 8 - unitSize; i < 8; ++i)
		CHECK(bytes[i] == 0);
	for (unsigned i = 8; i < 32; ++i)
		CHECK(bytes[i] == 0x7e);
	object->Remove();
}

static void CheckStringListInsertion(PviObject *task)
{
	InitCommonControls();
	HWND parent = CreateWindowExA(0, "STATIC", "", WS_OVERLAPPEDWINDOW,
								 0, 0, 640, 480, nullptr, nullptr, GetModuleHandleA(nullptr), nullptr);
	CHECK(parent != nullptr);
	mylistviewparam = {};
	mylistviewparam.hinstance = GetModuleHandleA(nullptr);
	mylistviewparam.hwndParent = parent;
	const char *columns[] = {"Name", "Type", "Scope", "Value"};
	for (unsigned i = 0; i < 4; ++i)
		mylistviewparam.column[i].name = columns[i];
	CHECK(MyListViewCreateWindow(&mylistviewparam) != nullptr);

	PviObject *array = AddObject("@Pvi/StringList/errorTexts", "gAxisHandling.ax[0].errorText", POBJ_PVAR);
	array->ex.pv.type = BR_STRING;
	array->ex.pv.pdatatype = "STRING";
	array->ex.pv.dimension = 15;
	array->ex.pv.length = 121;
	array->ex.pv.task = task;
	array->watchsort = 0;
	unsigned previousCreates = createRequests;
	expectedMessageCaption = "Watch list";
	expectedErrorMessages = 1;
	MyListViewInsertPVIObjects(array);
	CHECK(ListView_GetItemCount(mylistviewparam.hwndLV) == 0);
	CHECK(expectedErrorMessages == 0);
	CHECK(createRequests == previousCreates);
	CHECK(array->watchsort == -1 && array->linkid == 0);
	CHECK(array->ex.pv.pvalue == nullptr);
	CHECK(array->ex.pv.dimension == 15 && array->ex.pv.length == 121);

	expectedErrorMessages = 1;
	CHECK(array->Watch(TRUE) == nullptr);
	CHECK(expectedErrorMessages == 0);
	CHECK(createRequests == previousCreates);
	array->Expand();
	PviObject *element = array->FindChild(TRUE);
	CHECK(element != nullptr && element->ex.pv.dimension == 1);
	CHECK(element->ex.pv.length == 121);
	element->watchsort = 0;
	MyListViewInsertPVIObjects(element);
	CHECK(ListView_GetItemCount(mylistviewparam.hwndLV) == 1);
	CHECK(element->ex.pv.pvalue != nullptr);
	CHECK(ListView_DeleteAllItems(mylistviewparam.hwndLV) != 0);
	element->Remove();

	array->ex.pv.type = BR_STRUCT;
	array->ex.pv.dimension = 1;
	previousCreates = createRequests;
	expectedErrorMessages = 1;
	CHECK(array->Watch(TRUE) == nullptr);
	CHECK(expectedErrorMessages == 0);
	CHECK(createRequests == previousCreates);
	CHECK(array->ex.pv.pvalue == nullptr);
	array->Remove();
	expectedMessageCaption = "Error";

	PviObject *object = AddObject("@Pvi/StringList/errorText", "gAxisHandling.ax[0].errorText", POBJ_PVAR);
	object->ex.pv.type = BR_STRING;
	object->ex.pv.pdatatype = "STRING";
	object->ex.pv.dimension = 1;
	object->ex.pv.length = 4097;
	object->ex.pv.task = task;
	object->watchsort = 0;
	MyListViewInsertPVIObjects(object);
	CHECK(ListView_GetItemCount(mylistviewparam.hwndLV) == 1);
	CHECK(object->ex.pv.pvalue != nullptr);
	pvi_interface_notify.cbdata_changed = MyListViewUpdateValue;
	T_RESPONSE_INFO info = {};
	info.nType = POBJ_EVENT_DATA;
	info.LinkID = object->linkid;
	std::vector<char> data(object->ex.pv.length, '\1');
	data.back() = 0;
	std::vector<char> displayed(4096 * 5 + 1);
	for (BOOL oem : {FALSE, TRUE})
	{
		object->gui_info.interpret_as_oem = oem;
		callback(0, 0, data.data(), static_cast<DWORD>(data.size()), &info);
		LVITEMA item = {};
		item.iSubItem = 3;
		item.pszText = displayed.data();
		item.cchTextMax = static_cast<int>(displayed.size());
		CHECK(SendMessageA(mylistviewparam.hwndLV, LVM_GETITEMTEXTA, 0,
						   reinterpret_cast<LPARAM>(&item)) == 4096 * 5);
		for (unsigned i = 0; i < 4096; ++i)
			CHECK(memcmp(displayed.data() + i * 5, "\\x001", 5) == 0);
		CHECK(displayed.back() == 0);
	}
	pvi_interface_notify.cbdata_changed = DataChanged;
	CHECK(DestroyWindow(parent) != 0);
	mylistviewparam = {};
	object->Remove();
}

static void CheckObjectTreeView(PviObject *cpu, PviObject *task, const char *testDirectory)
{
	HINSTANCE instance = GetModuleHandleA(nullptr);
	HWND parent = CreateWindowExA(0, "STATIC", "", WS_OVERLAPPEDWINDOW,
								 0, 0, 640, 480, nullptr, nullptr, instance, nullptr);
	CHECK(parent != nullptr);
	ObjectTreeView first;
	ObjectTreeView second;
	CHECK(first.GetHandle() == nullptr && second.GetHandle() == nullptr);
	CHECK(first.Create(instance, parent) != nullptr);
	CHECK(second.Create(instance, parent) != nullptr);
	CHECK(first.GetHandle() != second.GetHandle());
	CHECK(MoveWindow(first.GetHandle(), 0, 0, 300, 240, FALSE) != 0);
	CHECK(MoveWindow(second.GetHandle(), 300, 0, 300, 240, FALSE) != 0);
	mylistviewparam = {};
	mylistviewparam.hinstance = instance;
	mylistviewparam.hwndParent = parent;
	const char *columns[] = {"Name", "Type", "Scope", "Value"};
	for (unsigned i = 0; i < 4; ++i)
		mylistviewparam.column[i].name = columns[i];
	CHECK(MyListViewCreateWindow(&mylistviewparam) != nullptr);

	PviObject *array = AddObject("@Pvi/Tree/Values", "Values", POBJ_PVAR);
	array->ex.pv.type = BR_DINT;
	array->ex.pv.pdatatype = "DINT";
	array->ex.pv.dimension = 3;
	array->ex.pv.length = sizeof(LONG);
	array->ex.pv.task = task;
	array->ex.pv.cpu = cpu;
	HTREEITEM root = first.AddItem(TVI_ROOT, array->descriptor, reinterpret_cast<LPARAM>(array->name), 0);
	CHECK(root != nullptr);
	CHECK(second.AddItem(TVI_ROOT, array->descriptor, reinterpret_cast<LPARAM>(array->name), 0) != nullptr);
	NMTREEVIEWA selection = {};
	selection.hdr.hwndFrom = first.GetHandle();
	selection.hdr.code = TVN_SELCHANGEDA;
	selection.itemNew.hItem = root;
	selection.itemNew.lParam = reinterpret_cast<LPARAM>(array->name);
	CHECK(second.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 1);
	CHECK(first.HandleMessages(WM_SIZE, 0, 0) == 1);
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 0);
	CHECK(TreeView_GetCount(first.GetHandle()) == 4);
	CHECK(TreeView_GetCount(second.GetHandle()) == 1);
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&selection)) == 0);
	CHECK(TreeView_GetCount(first.GetHandle()) == 4);
	char label[80];
	TVITEMA item = {};
	item.mask = TVIF_TEXT;
	item.hItem = root;
	item.pszText = label;
	item.cchTextMax = sizeof(label);
	CHECK(SendMessageA(first.GetHandle(), TVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&item)) != 0);
	CHECK(strcmp(label, "Values   :DINT[3]") == 0);

	NMHDR doubleClick = {};
	doubleClick.hwndFrom = first.GetHandle();
	doubleClick.code = NM_DBLCLK;
	CHECK(TreeView_SelectItem(first.GetHandle(), root) != 0);
	unsigned previousCreates = createRequests;
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&doubleClick)) == 0);
	CHECK(ListView_GetItemCount(mylistviewparam.hwndLV) == 0);
	CHECK(array->watchsort == -1 && createRequests == previousCreates);
	HTREEITEM childItem = TreeView_GetChild(first.GetHandle(), root);
	CHECK(childItem != nullptr);
	PviObject *element = PviObject::FindByName("@Pvi/Tree/Values[0]");
	CHECK(element != nullptr);
	CHECK(TreeView_SelectItem(first.GetHandle(), childItem) != 0);
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&doubleClick)) == 0);
	CHECK(ListView_GetItemCount(mylistviewparam.hwndLV) == 1);
	CHECK(element->watchsort == 0 && element->ex.pv.pvalue != nullptr);
	LVITEMA listItem = {};
	listItem.mask = LVIF_PARAM;
	CHECK(ListView_GetItem(mylistviewparam.hwndLV, &listItem) != 0);
	CHECK(reinterpret_cast<const char *>(listItem.lParam) == element->name);

	NMTREEVIEWA drag = {};
	drag.hdr.hwndFrom = first.GetHandle();
	drag.hdr.code = TVN_BEGINDRAGA;
	drag.itemNew.hItem = childItem;
	drag.itemNew.lParam = reinterpret_cast<LPARAM>(element->name);
	CHECK(first.HandleMessages(WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&drag)) == 0);
	CHECK(g_draggedPVIObject == element);
	CHECK(GetCapture() == parent);
	CHECK(TreeView_GetDropHilight(first.GetHandle()) == childItem);
	CHECK(TreeView_GetDropHilight(second.GetHandle()) == nullptr);
	ImageList_EndDrag();
	ImageList_DragLeave(parent);
	ReleaseCapture();
	g_draggedPVIObject = nullptr;

	char originalSettings[MAX_PATH];
	strcpy(originalSettings, SettingsGetFileName());
	char settingsFile[MAX_PATH];
	snprintf(settingsFile, sizeof(settingsFile), "%s\\tree-settings.ini", testDirectory);
	strcpy(SettingsGetFileName(), settingsFile);
	HMENU menu = CreatePopupMenu();
	CHECK(menu != nullptr);
	CHECK(AppendMenuA(menu, MF_STRING, IDM_STORE_CPU_SETTINGS, "Store") != 0);
	CHECK(AppendMenuA(menu, MF_STRING, IDM_REMOVE_CPU_SETTINGS, "Remove") != 0);
	second.UpdateCpuSettingsMenu(menu);
	CHECK((GetMenuState(menu, IDM_STORE_CPU_SETTINGS, MF_BYCOMMAND) & MF_GRAYED) != 0);
	CHECK((GetMenuState(menu, IDM_REMOVE_CPU_SETTINGS, MF_BYCOMMAND) & MF_GRAYED) != 0);
	HTREEITEM cpuItem = second.AddItem(TVI_ROOT, cpu->descriptor, reinterpret_cast<LPARAM>(cpu->name), 0);
	CHECK(cpuItem != nullptr);
	CHECK(TreeView_SelectItem(second.GetHandle(), cpuItem) != 0);
	second.UpdateCpuSettingsMenu(menu);
	CHECK((GetMenuState(menu, IDM_STORE_CPU_SETTINGS, MF_BYCOMMAND) & MF_GRAYED) == 0);
	CHECK((GetMenuState(menu, IDM_REMOVE_CPU_SETTINGS, MF_BYCOMMAND) & MF_GRAYED) != 0);
	second.CpuSettingsCommand(IDM_STORE_CPU_SETTINGS);
	CHECK(SettingsFindCpuSection(cpu->descriptor) != 0);
	second.UpdateCpuSettingsMenu(menu);
	CHECK((GetMenuState(menu, IDM_STORE_CPU_SETTINGS, MF_BYCOMMAND) & MF_GRAYED) != 0);
	CHECK((GetMenuState(menu, IDM_REMOVE_CPU_SETTINGS, MF_BYCOMMAND) & MF_GRAYED) == 0);
	second.CpuSettingsCommand(IDM_REMOVE_CPU_SETTINGS);
	CHECK(SettingsFindCpuSection(cpu->descriptor) == 0);
	first.CpuSettingsCommand(IDM_STORE_CPU_SETTINGS);
	CHECK(SettingsFindCpuSection(cpu->descriptor) == 0);
	CHECK(DestroyMenu(menu) != 0);
	CHECK(DeleteFileA(settingsFile) != 0);
	strcpy(SettingsGetFileName(), originalSettings);

	second.ClearAll();
	CHECK(TreeView_GetCount(second.GetHandle()) == 0);
	CHECK(TreeView_GetCount(first.GetHandle()) == 4);
	first.ClearAll();
	CHECK(TreeView_GetCount(first.GetHandle()) == 0);
	CHECK(DestroyWindow(parent) != 0);
	mylistviewparam = {};
	for (unsigned i = 0; i < 3; ++i)
	{
		char name[80];
		snprintf(name, sizeof(name), "@Pvi/Tree/Values[%u]", i);
		PviObject *child = PviObject::FindByName(name);
		CHECK(child != nullptr);
		child->Remove();
	}
	array->Remove();
}

static void CheckAboutLink()
{
	INITCOMMONCONTROLSEX controls = {sizeof(INITCOMMONCONTROLSEX), ICC_LINK_CLASS};
	CHECK(InitCommonControlsEx(&controls) != 0);
	HMODULE module = GetModuleHandleA(nullptr);
	HRSRC resource = FindResourceA(module, MAKEINTRESOURCEA(DLG_ABOUT), RT_DIALOG);
	CHECK(resource != nullptr);
	DWORD size = SizeofResource(module, resource);
	CHECK(size >= 26);
	HGLOBAL loaded = LoadResource(module, resource);
	CHECK(loaded != nullptr);
	const void *dialogTemplate = LockResource(loaded);
	CHECK(dialogTemplate != nullptr);
	std::vector<WORD> hiddenTemplate((size + sizeof(WORD) - 1) / sizeof(WORD));
	memcpy(hiddenTemplate.data(), dialogTemplate, size);
	CHECK(hiddenTemplate[1] == 0xffff);
	// DIALOGEX stores its style DWORD at byte offset 12.
	DWORD style;
	memcpy(&style, hiddenTemplate.data() + 6, sizeof(style));
	style &= ~WS_VISIBLE;
	memcpy(hiddenTemplate.data() + 6, &style, sizeof(style));
	HWND dialog = CreateDialogIndirectParamA(module,
		reinterpret_cast<const DLGTEMPLATE *>(hiddenTemplate.data()), nullptr, AboutDlgProc, 0);
	CHECK(dialog != nullptr);
	CHECK(!IsWindowVisible(dialog));
	HWND link = GetDlgItem(dialog, IDC_LINK_PROJECT);
	CHECK(link != nullptr);
	CHECK((GetWindowLongPtrA(link, GWL_STYLE) & WS_TABSTOP) != 0);
	NMLINK notification = {};
	notification.hdr.hwndFrom = link;
	notification.hdr.idFrom = IDC_LINK_PROJECT;
	notification.item.mask = LIF_ITEMINDEX | LIF_URL;
	notification.item.iLink = 0;
	CHECK(SendMessageW(link, LM_GETITEM, 0, reinterpret_cast<LPARAM>(&notification.item)) != 0);
	CHECK(lstrcmpW(notification.item.szUrl, L"https://github.com/hilch/brwatch") == 0);
	for (UINT code : {NM_CLICK, NM_RETURN})
	{
		notification.hdr.code = code;
		unsigned previousLaunches = browserLaunches;
		SendMessageA(dialog, WM_NOTIFY, IDC_LINK_PROJECT, reinterpret_cast<LPARAM>(&notification));
		CHECK(browserLaunches == previousLaunches + 1);
	}
	browserResult = SE_ERR_NOASSOC;
	expectedErrorMessages = 1;
	SendMessageA(dialog, WM_NOTIFY, IDC_LINK_PROJECT, reinterpret_cast<LPARAM>(&notification));
	CHECK(expectedErrorMessages == 0);
	browserResult = 33;
	CHECK(DestroyWindow(dialog) != 0);
}

int main()
{
	char temporaryPath[MAX_PATH];
	char testDirectory[MAX_PATH];
	CHECK(GetTempPathA(sizeof(temporaryPath), temporaryPath) != 0);
	CHECK(GetTempFileNameA(temporaryPath, "pvo", 0, testDirectory) != 0);
	CHECK(DeleteFileA(testDirectory) != 0);
	CHECK(CreateDirectoryA(testDirectory, nullptr) != 0);
	strcpy(GetApplicationPath(), testDirectory);

	PviObject empty;
	CHECK(empty.linkid == 0);
	CHECK(empty.name[0] == '\0' && empty.descriptor[0] == '\0');
	CHECK(empty.ex.pv.pvalue == nullptr && empty.ex.pv.cpu == nullptr);
	CHECK(empty.gui_info.has_childs == 0);
	CHECK(PviObject::Start() == 0);
	CHECK(PviObject::Count() == 0);
	CHECK(strcmp(PviObject::GetNext(TRUE)->name, "@Pvi") == 0);

	PviObject *device = AddObject("@Pvi/Device", "/IF=tcpip", POBJ_DEVICE);
	PviObject duplicate = *device;
	CHECK(duplicate.Add(FALSE) == device);
	CHECK(PviObject::Count() == 1);
	PviObject *cpu = AddObject("@Pvi/Device/CPU", "/IP=127.0.0.1", POBJ_CPU);
	PviObject *task = AddObject("@Pvi/Device/CPU/Task", "Task", POBJ_TASK);
	task->ex.task.cpu = cpu;
	CHECK(device->FindChild(TRUE) == cpu);
	CHECK(device->FindChild(FALSE) == nullptr);
	CHECK(device->IsParentOf(task));
	CHECK(!task->IsParentOf(device));
	CHECK(!task->IsParentOf(nullptr));
	CHECK(!task->IsParentOf(task));

	PviObject *structure = AddObject("@Pvi/Device/CPU/Task/AxisHandling", "AxisHandling", POBJ_PVAR);
	structure->ex.pv.type = BR_STRUCT;
	structure->ex.pv.dimension = 1;
	structure->ex.pv.cpu = cpu;
	structure->ex.pv.task = task;
	readResponse =
		"VT=struct SN=AxisHandlingType VL=78056 VN=1 "
		"{.ax VT=struct SN=AxisHandlingAxisType VL=19240 VN=2 VO=0} "
		"{.ax.name VT=string VL=33 VN=1 VO=0} "
		"{.axGroupCnc VT=struct SN=AxisHandlingAxesGroupType VL=19788 VN=1 VO=38480} "
		"{.axGroupAux VT=struct SN=AxisHandlingAxesGroupType VL=19788 VN=1 VO=58268} "
		"{.unnamed VT=struct VL=4 VN=1 VO=0} "
		"{.state VT=i32 SN=StateEnum VL=4 VN=1 VO=0}";
	structure->Expand();
	CHECK(structure->gui_info.has_childs == 1);
	CHECK(strcmp(structure->ex.pv.pdatatype, "AxisHandlingType") == 0);
	const char *childNames[] = {".ax", ".axGroupCnc", ".axGroupAux", ".unnamed", ".state"};
	const char *childTypes[] = {"AxisHandlingAxisType", "AxisHandlingAxesGroupType",
							   "AxisHandlingAxesGroupType", "DATATYPE(STRUCT)", "DINT"};
	PviObject *child = structure->FindChild(TRUE);
	for (unsigned i = 0; i < sizeof(childNames) / sizeof(childNames[0]); ++i)
	{
		CHECK(child != nullptr);
		CHECK(strcmp(child->name + strlen(structure->name), childNames[i]) == 0);
		CHECK(strcmp(child->ex.pv.pdatatype, childTypes[i]) == 0);
		CHECK(child->ex.pv.dimension == (i == 0 ? 2 : 1));
		CHECK(child->ex.pv.cpu == cpu && child->ex.pv.task == task);
		child = structure->FindChild(FALSE);
	}
	CHECK(child == nullptr);

	PviObject *axisArray = PviObject::FindByName("@Pvi/Device/CPU/Task/AxisHandling.ax");
	CHECK(axisArray != nullptr);
	readResponse =
		"VT=struct SN=AxisHandlingAxisType VL=19240 VN=2 "
		"{.name VT=string VL=33 VN=1 VO=0}";
	axisArray->Expand();
	CHECK(axisArray->gui_info.has_childs == 1);
	PviObject *axis = axisArray->FindChild(TRUE);
	CHECK(axis != nullptr && axis->ex.pv.dimension == 1);
	CHECK(strcmp(axis->name, "@Pvi/Device/CPU/Task/AxisHandling.ax[0]") == 0);
	axis = axisArray->FindChild(FALSE);
	CHECK(axis != nullptr && axis->ex.pv.dimension == 1);
	CHECK(strcmp(axis->name, "@Pvi/Device/CPU/Task/AxisHandling.ax[1]") == 0);
	CHECK(axisArray->FindChild(FALSE) == nullptr);
	PviObject *axisName = PviObject::FindByName("@Pvi/Device/CPU/Task/AxisHandling.ax[1].name");
	CHECK(axisName != nullptr && axisName->ex.pv.type == BR_STRING);
	CHECK(axisName->ex.pv.cpu == cpu && axisName->ex.pv.task == task);

	PviObject *variable = AddObject("@Pvi/Device/CPU/Task/Value", "Value", POBJ_PVAR);
	variable->ex.pv.type = BR_DINT;
	variable->ex.pv.length = sizeof(LONG);
	variable->ex.pv.dimension = 1;
	variable->ex.pv.pdatatype = "DINT";
	CHECK(variable->Watch(TRUE) == variable);
	CHECK(variable->ex.pv.pvalue != nullptr);
	CHECK(*static_cast<LONG *>(variable->ex.pv.pvalue) == 0);
	CHECK(PviObject::FindByLinkId(variable->linkid) == variable);
	pvi_interface_notify.cbdata_changed = DataChanged;
	pvi_interface_notify.cberror_changed = ErrorChanged;
	T_RESPONSE_INFO info = {};
	info.nType = POBJ_EVENT_DATA;
	info.LinkID = variable->linkid;
	LONG value = 42;
	CHECK(callback != nullptr);
	callback(0, 0, &value, sizeof(value), &info);
	CHECK(*static_cast<LONG *>(variable->ex.pv.pvalue) == value);
	CHECK(dataNotifications == 1 && variable->ex.pv.value_changed == 1);
	info.nType = POBJ_EVENT_ERROR;
	info.ErrCode = 123;
	callback(0, 0, nullptr, 0, &info);
	CHECK(variable->error == 123 && errorNotifications == 1);
	info.nType = POBJ_EVENT_DATA;
	callback(0, 0, &value, sizeof(value), &info);
	CHECK(variable->error == 0 && errorNotifications == 2);
	CheckRegistryGrowth(variable, cpu, task, axisName);
	CheckCursorRemoval();
	CheckStringCallbacks(BR_STRING);
	CheckStringCallbacks(BR_WSTRING);
	CheckStringListInsertion(task);
	CheckObjectTreeView(cpu, task, testDirectory);
	CheckAboutLink();
	CHECK(variable->Watch(FALSE) == variable);
	CHECK(variable->ex.pv.pvalue == nullptr && variable->linkid == 0);
	CHECK(variable->watchsort == -1);

	char watchFile[MAX_PATH];
	snprintf(watchFile, sizeof(watchFile), "%s\\watch.wtc", testDirectory);
	variable->watchsort = 3;
	CHECK(variable->AddToWatchFile(watchFile) == 0);
	readResponse = "VT=i32 VL=4 VN=1 SC=g";
	CHECK(PviObject::LoadFromWatchFile(POBJ_PVAR, watchFile) == 1);
	CHECK(variable->watchsort == 3 && variable->gui_info.loaded_from_file == 1);
	CHECK(variable->ex.pv.type == BR_DINT);
	CHECK(variable->ex.pv.cpu == cpu && variable->ex.pv.task == task);
	CHECK(PviObject::FindByName(variable->name) == variable);

	int previousCount = PviObject::Count();
	detachedLinkId = variable->linkid;
	variable->Remove();
	detachedLinkId = 0;
	CHECK(PviObject::Count() == previousCount - 1);
	CHECK(PviObject::FindByName("@Pvi/Device/CPU/Task/Value") == nullptr);
	CHECK(PviObject::Stop() == 0);
	CHECK(PviObject::Count() == 0);
	CHECK(PviObject::GetNext(TRUE)->type == POBJ_PVI);
	CHECK(PviObject::GetNext(FALSE) == nullptr);
	CHECK(DeleteFileA(watchFile) != 0);
	char logFile[MAX_PATH];
	snprintf(logFile, sizeof(logFile), "%s\\pvilog.txt", testDirectory);
	CHECK(DeleteFileA(logFile) != 0);
	CHECK(RemoveDirectoryA(testDirectory) != 0);
	puts("PASS: PviObject lifecycle, vector growth, cursor removal, expansion, bounded string callbacks, scalar-only watch insertion, ObjectTreeView, About hyperlink, and watch files");
	return 0;
}
