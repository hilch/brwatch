
#define WIN32_LEAN_AND_MEAN /* speed up compilations */

#include "main.h"
#include "object_treeview.h"
#include "watch_listview.h"
#include "pvi_interface.h"
#include "stringtools.h"
#include "logger.h"
#include "settings.h"
#include "dlg_writepar.h"
#include "dlg_about.h"
#include "dlg_editcpu.h"
#include "dlg_showpviobjects.h"
#include "compressfile.h"
#include "resource.h"
#include <wingdi.h>

/** Prototypes **************************************************************/

static LRESULT WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
// static void Main_OnDestroy(HWND);
static void PviObjectsValid(void);
static void PviObjectErrorChanged(PviObject *object, int errcode);
static void PviObjectDataChanged(PviObject *object);
// static void PviObjectStatusChanged( PviObject *object, char* status );

static void LoadWatchFile(char *filename);
static void SaveWatchFile(char *filename);

/** Global variables ********************************************************/

static char application_path[MAX_PATH];
static LPSTR commandLine;

/* Global variables (extern in main.h) */
HWND g_hwndMainWindow = nullptr;
HINSTANCE g_hInstance = nullptr;
PviObject *g_draggedPVIObject = nullptr;
PviObject *g_selectedPVIObject = nullptr;
char g_PVIVersionString[50] = {0};

/* Global variables (extern in watch_listview.h) */
WatchListView watchListView;

/* Global variables (extern in object_treeview.h) */
ObjectTreeView objectTreeView;

/* Global variables (extern in pvi_interface.h) */
PVIOBJECTNOTIFICATION pvi_interface_notify = {};

// static HTREEITEM hselected_treeitem;

static int drop_target_index;
static int instance_counter;

static NOTIFYICONDATAA notifyicondata;

char *GetApplicationPath(void)
{
	return application_path;
}

HWND MainWindowGetHandle(void)
{
	return (g_hwndMainWindow);
}
/* ============================================================================================== */

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM /*lParam*/)
{
	char windowname[256];

	GetWindowTextA(hwnd, windowname, sizeof(windowname));
	if (!strncmp(windowname, APPLICATION_NAME, lstrlenA(APPLICATION_NAME)))
		instance_counter++;

	return TRUE;
}

/* main entry point */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSA wc;
	MSG msg;
	char windowname[30];

	commandLine = lpCmdLine;

	HMODULE m_user32Dll = LoadLibraryA("User32.dll");
	if (m_user32Dll)
	{
		FARPROC pFunc = GetProcAddress(m_user32Dll, "SetProcessDPIAware");
		if (pFunc)
		{
			pFunc();
		}
		FreeLibrary(m_user32Dll);
		m_user32Dll = nullptr;
	}

	// save application path
	if (GetCurrentDirectoryA(sizeof(application_path), application_path))
	{
	}
	else
	{
		MessageBoxA(nullptr, "GetCurrentDirectory", "Error", MB_OK);
		return -1;
	}

	SettingsInitialize(); // ggf. Default- Ini kreieren

	// instance counter
	instance_counter = 1;
	EnumWindows(EnumWindowsProc, 0);

	sprintf(windowname, APPLICATION_NAME " {%u. instance}", instance_counter);

	INITCOMMONCONTROLSEX controls = {sizeof(INITCOMMONCONTROLSEX), ICC_WIN95_CLASSES | ICC_LINK_CLASS};
	if (!InitCommonControlsEx(&controls))
	{
		MessageBoxA(nullptr, "Could not initialize Windows common controls.", "Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	g_hInstance = hInstance;
	memset(&wc, 0, sizeof(wc));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIconA(hInstance, MAKEINTRESOURCEA(IDR_ICO_MAIN));
	// wc.hCursor    = LoadCursor(NULL, IDC_ARROW);
	wc.hCursor = LoadCursor(nullptr, IDC_SIZEWE); // for splitter
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = MAKEINTRESOURCEA(IDR_MNU_MAIN);
	wc.lpszClassName = "brwatchClass";

	if (!RegisterClassA(&wc))
		return 0;

	g_hwndMainWindow = CreateWindowA("brwatchClass", windowname, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);
	if (!g_hwndMainWindow)
		return 0;
	ShowWindow(g_hwndMainWindow, nCmdShow);
	UpdateWindow(g_hwndMainWindow);

	// Notify area
	memset(&notifyicondata, 0, sizeof(notifyicondata));
	notifyicondata.cbSize = sizeof(notifyicondata);
	notifyicondata.hWnd = g_hwndMainWindow;
	strcpy(notifyicondata.szTip, windowname);
	strcat(notifyicondata.szTip, " - Click here to size up");
	notifyicondata.hIcon = LoadIconA(hInstance, MAKEINTRESOURCEA(IDR_ICO_MAIN));
	notifyicondata.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	notifyicondata.uCallbackMessage = NOTIFY_ICON_MESSAGE;

	// set callback functions for PVI
	pvi_interface_notify.cbobjects_valid = PviObjectsValid;
	pvi_interface_notify.cberror_changed = PviObjectErrorChanged;
	pvi_interface_notify.cbdata_changed = PviObjectDataChanged;

	while (GetMessageA(&msg, nullptr, 0, 0))
	{
		HWND scanDialog = objectTreeView.GetCpuScanDialogHandle();
		if (scanDialog != nullptr && IsDialogMessageA(scanDialog, &msg))
			continue;
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	PviObject::Stop();
	return (int)msg.wParam;
}

/* is called when app is connected to PVI manager and device objects were created */

static void PviObjectsValid(void)
{
	PviObject *object;
	char logger_watch_file[MAX_PATH];

	object = PviObject::GetNext(TRUE);
	while ((object = PviObject::GetNext(FALSE)) != nullptr)
	{
		if (object->type == POBJ_DEVICE)
		{
			objectTreeView.AddItem(TVI_ROOT, object->descriptor, (LPARAM)object->name, 0);
		}
	}

	// if logger was active when program ended it will be started again.
	if (GetPrivateProfileIntA("General", "LoggerActive", 0, SettingsGetFileName()))
	{ // was logger active ?
		strcpy(logger_watch_file, GetApplicationPath());
		strcat(logger_watch_file, "\\_logger.wtc");
		LoadWatchFile(logger_watch_file);
		Logger();
		SendMessage(MainWindowGetHandle(), NOTIFY_LOGGER_STATUS, 0, 0);
		Shell_NotifyIconA(NIM_ADD, &notifyicondata);
		ShowWindow(g_hwndMainWindow, SW_HIDE);
	}
}

static void PviObjectDataChanged(PviObject *object)
{
	watchListView.UpdateValue(object);
}

/* is called when error status of object has changed */

static void PviObjectErrorChanged(PviObject *object, int errcode)
{
	LVFINDINFO lvfindinfo;
	int index;
	char errtext[80];

	memset(&lvfindinfo, 0, sizeof(lvfindinfo));

	lvfindinfo.flags = LVFI_PARAM;
	lvfindinfo.lParam = (LPARAM)object->name;

	index = ListView_FindItem(watchListView.GetHandle(), -1, &lvfindinfo);
	if (index >= 0)
	{
		switch (errcode)
		{
		case 0:
			strcpy(errtext, "");
			break;

		case 4177: // not initialized dynamic variable
			strcpy(errtext, "(NULL)");
			break;

		case 4808:	// no connection to plc (INA2000)
		case 11022: // no connection to plc (ANSL)
			strcpy(errtext, "(OFFLINE)");
			break;

		case 4813: // object not found
			strcpy(errtext, "(Object not found)");
			break;

		default:
			sprintf(errtext, "Error:%u", errcode);
			break;
		}
		LVITEMA item = {};
		item.iSubItem = 3;
		item.pszText = errtext;
		SendMessageA(watchListView.GetHandle(), LVM_SETITEMTEXTA, index, reinterpret_cast<LPARAM>(&item));
	}
}

/* save watch file */

static void SaveWatchFile(char *filename)
{
	int index;
	LVITEM lvitem;
	PviObject *object;

	remove(filename);
	index = ListView_GetNextItem(watchListView.GetHandle(), -1, LVNI_ALL);
	while (index != -1)
	{
		memset(&lvitem, 0, sizeof(lvitem));
		lvitem.mask = LVIF_PARAM;
		lvitem.iItem = index;
		ListView_GetItem(watchListView.GetHandle(), &lvitem);
		if (lvitem.lParam != 0)
		{
			object = PviObject::FindByName((char *)lvitem.lParam);
			if (object != nullptr)
			{
				object->watchsort = lvitem.iItem;
				object->AddToWatchFile(filename);
			}
		}
		index = ListView_GetNextItem(watchListView.GetHandle(), index, LVNI_ALL);
	}
}

/* load watch file */

static void LoadWatchFile(char *filename)
{
	const T_POBJ_TYPE objectTypes[] = {POBJ_DEVICE, POBJ_CPU, POBJ_TASK, POBJ_PVAR};
	PviObject *object;
	HCURSOR oldcursor;
	int result, n;
	int sort = 0;

	oldcursor = SetCursor(LoadCursor(nullptr, IDC_WAIT));

	// load objects from file
	n = 0;
	for (const T_POBJ_TYPE objectType : objectTypes)
	{
		result = PviObject::LoadFromWatchFile(objectType, filename);
		if (result > 0)
		{
			n += result;
		}
	}

	// add objects to tree view

	while (n)
	{
		object = PviObject::GetNext(TRUE);
		while ((object = PviObject::GetNext(FALSE)) != nullptr)
		{
			if (object->gui_info.loaded_from_file && object->watchsort == sort)
			{
				object->gui_info.loaded_from_file = 0;
				++sort;
				--n;
				watchListView.InsertPVIObjects(object);
			}
		}
	}

	UpdateWindow(watchListView.GetHandle());
	SetCursor(oldcursor);
}

/* window procedure */

static LRESULT CALLBACK WndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	static BOOL drag_splitter = FALSE;	 // for splitter
	static UINT splitter_xpos = 250;	 // for splitter
	static UINT writewnd_height = 0;	 // height of write window
										 //	static HWND hwndSettings=NULL; // settings- window
	static HFONT hfont_watchlist = nullptr; // font for ListView
	RECT rect;
	HIMAGELIST himl;

	if (watchListView.HandleMessages(wMsg, wParam, lParam) == 0)
		return (0);

	if (objectTreeView.HandleMessages(wMsg, wParam, lParam) == 0)
		return (0);

	switch (wMsg)
	{

	case WM_INITMENUPOPUP:
		objectTreeView.UpdateCpuSettingsMenu(reinterpret_cast<HMENU>(wParam));
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDM_STORE_CPU_SETTINGS:
		case IDM_REMOVE_CPU_SETTINGS:
			objectTreeView.CpuSettingsCommand(static_cast<UINT>(wParam));
			break;

		case IDM_ABOUT:
			DialogBoxA(g_hInstance, MAKEINTRESOURCEA(DLG_ABOUT), hWnd, AboutDlgProc);
			break;

		case IDM_STATUSPVIOBJEKTE:
			// DialogBox(ghInstance, MAKEINTRESOURCE(DLG_SHOWPVIOBJECTS), hWnd, ShowPviObjectsDlgProc);
			ShowWindowPviObjects();
			break;
		case IDM_ADD_CPU:
		{
			PviObject *cpuObject = DlgAddCpuShowDialog();
			if (cpuObject == nullptr)
				break;
			HWND tree = objectTreeView.GetHandle();
			for (HTREEITEM deviceItem = TreeView_GetRoot(tree); deviceItem != nullptr;
				deviceItem = TreeView_GetNextSibling(tree, deviceItem))
			{
				TVITEMA item = {};
				item.mask = TVIF_PARAM;
				item.hItem = deviceItem;
				if (!SendMessageA(tree, TVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&item)))
					continue;
				const char *deviceName = reinterpret_cast<const char *>(item.lParam);
				size_t length = strlen(deviceName);
				if (strncmp(cpuObject->name, deviceName, length) != 0 || cpuObject->name[length] != '/')
					continue;
				if (TreeView_GetChild(tree, deviceItem) == nullptr)
				{
					PviObject *deviceObject = PviObject::FindByName(reinterpret_cast<char *>(item.lParam));
					PviObject *child = deviceObject != nullptr ? deviceObject->FindChild(TRUE) : nullptr;
					while (child != nullptr)
					{
						objectTreeView.AddItem(deviceItem, child->descriptor,
							reinterpret_cast<LPARAM>(child->name), ResourcesGetPviObjectImage(child));
						child = deviceObject->FindChild(FALSE);
					}
				}
				HTREEITEM cpuItem = TreeView_GetChild(tree, deviceItem);
				while (cpuItem != nullptr)
				{
					item.hItem = cpuItem;
					if (SendMessageA(tree, TVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&item)) &&
						strcmp(reinterpret_cast<const char *>(item.lParam), cpuObject->name) == 0)
						break;
					cpuItem = TreeView_GetNextSibling(tree, cpuItem);
				}
				if (cpuItem == nullptr)
					cpuItem = objectTreeView.AddItem(deviceItem, cpuObject->descriptor,
						reinterpret_cast<LPARAM>(cpuObject->name), ResourcesGetPviObjectImage(cpuObject));
				TreeView_Expand(tree, deviceItem, TVE_EXPAND);
				TreeView_EnsureVisible(tree, cpuItem);
				break;
			}
		}
		break;
		case IDM_MNU_REFRESH:
			if (MessageBoxA(hWnd, "Delete all entries.\n\nAre you sure ?", "Clear Objects", MB_YESNO | MB_ICONWARNING) == IDYES)
			{
				HCURSOR oldcursor;

				oldcursor = SetCursor(LoadCursor(nullptr, IDC_WAIT));
				objectTreeView.ClearAll();
				ListView_DeleteAllItems(watchListView.GetHandle());
				PviObject::Stop();
				Sleep(1000);
				PviObject::Start();
				SetCursor(oldcursor);
			}
			break;
		case IDM_SAVE_WATCH_FILE:
		{
			char filename[MAX_PATH];
			OPENFILENAMEA ofn;
			strcpy(filename, "default");
			memset(&ofn, 0, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = "WatchFile\0*.wtc\0\0";
			ofn.lpstrFile = filename;
			ofn.lpstrDefExt = "wtc";
			ofn.nMaxFile = sizeof(filename);
			ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
			if (GetSaveFileNameA(&ofn))
			{
				SaveWatchFile(filename);
			}
		}
		break;

		case IDM_LOAD_WATCH_FILE:
		{
			char filename[MAX_PATH];
			OPENFILENAMEA ofn;
			memset(&ofn, 0, sizeof(ofn));
			strcpy(filename, "default");
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = "WatchFile\0*.wtc\0\0";
			ofn.lpstrFile = filename;
			ofn.lpstrDefExt = "wtc";
			ofn.nMaxFile = sizeof(filename);
			ofn.Flags = OFN_HIDEREADONLY;
			if (GetOpenFileNameA(&ofn))
			{
				LoadWatchFile(filename);
			}
		}
		break;

		case IDM_LOGGER_CONFIG:
			DialogBoxA(g_hInstance, MAKEINTRESOURCEA(DLG_LOGGER_CONFIG), hWnd, LoggerConfigDlgProc);
			break;
			break;

		case IDM_LOGGER_START:
			Logger();
			SendMessageA(hWnd, NOTIFY_LOGGER_STATUS, 0, 0);
			break;

		case IDM_LOGGER_UNZIPLOGGERFILE:
		{
			char filename[MAX_PATH];
			int result;
			char tempstring[80];

			OPENFILENAMEA ofn;
			memset(&ofn, 0, sizeof(ofn));
			strcpy(filename, "");
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = "ZippedLoggerFile(*.gz)\0*.gz\0\0";
			ofn.lpstrFile = filename;
			ofn.lpstrDefExt = "gz";
			ofn.nMaxFile = sizeof(filename);
			ofn.Flags = OFN_HIDEREADONLY;
			if (GetOpenFileNameA(&ofn))
			{
				result = DecompressFile(filename);
				if (result == 0)
				{
					MessageBoxA(hWnd, "File was successfully unzipped !", "Unzip Logger file", MB_OK);
				}
				else
				{
					sprintf(tempstring, "Error unzipping file: %i", result);
					MessageBoxA(hWnd, tempstring, "Unzip Logger file", MB_OK | MB_ICONERROR);
				}
			}
		}
		break;

		case IDM_CHANGE_VIEW_DECIMAL:
			if (g_selectedPVIObject != nullptr)
			{
				g_selectedPVIObject->gui_info.display_as_decimal ^= 1;
				watchListView.UpdateValue(g_selectedPVIObject);
			}
			break;

		case IDM_CHANGE_VIEW_HEX:
			if (g_selectedPVIObject != nullptr)
			{
				g_selectedPVIObject->gui_info.display_as_hex ^= 1;
				watchListView.UpdateValue(g_selectedPVIObject);
			}
			break;

		case IDM_CHANGE_VIEW_BINARY:
			if (g_selectedPVIObject != nullptr)
			{
				g_selectedPVIObject->gui_info.display_as_binary ^= 1;
				watchListView.UpdateValue(g_selectedPVIObject);
			}
			break;

		case IDM_CHANGE_VIEW_CHAR:
			if (g_selectedPVIObject != nullptr)
			{
				g_selectedPVIObject->gui_info.display_as_char ^= 1;
				watchListView.UpdateValue(g_selectedPVIObject);
			}
			break;

		case IDM_CHANGE_VIEW_OEMCHAR:
			if (g_selectedPVIObject != nullptr)
			{
				g_selectedPVIObject->gui_info.interpret_as_oem ^= 1;
				watchListView.UpdateValue(g_selectedPVIObject);
			}
			break;

		case IDM_DELETE_OBJECT_FROM_LIST:
		{
			LVITEM lvitem;
			int i;
			// char *temp;

			i = ListView_GetNextItem(watchListView.GetHandle(), -1, LVNI_ALL);
			while (i != -1)
			{
				memset(&lvitem, 0, sizeof(lvitem));
				lvitem.mask = LVIF_PARAM;
				lvitem.iItem = i;
				ListView_GetItem(watchListView.GetHandle(), &lvitem);
				if (lvitem.lParam != 0)
				{
					// temp = (char*) lvitem.lParam;
					if (strcmp(g_selectedPVIObject->name, (char *)lvitem.lParam) == 0)
					{
						g_selectedPVIObject->Watch(FALSE);		// remove from watch
						ListView_DeleteItem(watchListView.GetHandle(), i); // remove from list view
						break;
					}
				}
				i = ListView_GetNextItem(watchListView.GetHandle(), i, LVNI_ALL);
			}
		}
		break;

		case IDM_VIEW_SETDEFAULTFONT:
		{
			HFONT hfont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
			SendMessage(watchListView.GetHandle(), WM_SETFONT, (WPARAM)hfont, TRUE);
			if (hfont_watchlist != nullptr)
			{
				DeleteObject(hfont_watchlist);
				hfont_watchlist = nullptr;
			}
		}
		break;

		case IDM_VIEW_SELECT_FONT:
		{
			CHOOSEFONTA cf;
			LOGFONTA font;

			if (hfont_watchlist != nullptr)
			{
				DeleteObject(hfont_watchlist);
				hfont_watchlist = nullptr;
			}

			ZeroMemory(&font, sizeof(font));
			font.lfCharSet = ANSI_CHARSET;
			font.lfWeight = FW_THIN;
			strncpy(font.lfFaceName, "System", LF_FACESIZE);

			ZeroMemory(&cf, sizeof(cf));
			cf.lStructSize = sizeof(cf);
			cf.hInstance = g_hInstance;
			cf.hwndOwner = hWnd;
			cf.Flags = CF_SCREENFONTS | CF_NOVERTFONTS | CF_INITTOLOGFONTSTRUCT;
			cf.lpLogFont = &font;
			if (ChooseFontA(&cf))
			{
				hfont_watchlist = CreateFontIndirectA(&font);
				SendMessage(watchListView.GetHandle(), WM_SETFONT, (WPARAM)hfont_watchlist, 1);
			}
		}
		break;

		case IDM_OPTIONS_SETTINGS:
			DialogBoxA(g_hInstance, MAKEINTRESOURCEA(DLG_SETTINGS), hWnd, SettingsDlg);
			break;

		case IDM_QUIT:
			DestroyWindow(hWnd);
			break;
		}

		return 0;

	case WM_SIZE:
	{
		int nWidth = LOWORD(lParam);  // width of client area
		int nHeight = HIWORD(lParam); // height of client area

		// für Splitter
		MoveWindow(objectTreeView.GetHandle(), 0, 0, splitter_xpos - 1, nHeight, TRUE);
		MoveWindow(watchListView.GetHandle(), splitter_xpos + 1, 0, nWidth - 101, nHeight - writewnd_height, TRUE);
		//		MoveWindow(mywritelistviewparam.hwndLV, splitter_xpos + 1, nHeight-writewnd_height, nWidth - 101, writewnd_height, TRUE);
	}
		return 0;

	case WM_LBUTTONDOWN:
		// for splitter
		SetCapture(hWnd);
		drag_splitter = TRUE;
		return 0;

	case WM_LBUTTONUP:
		// for splitter
		ReleaseCapture();
		drag_splitter = FALSE;

		// drop object
		if (g_draggedPVIObject != nullptr)
		{
			ImageList_EndDrag();
			ImageList_DragLeave(g_hwndMainWindow);
			ReleaseCapture();
			g_draggedPVIObject->watchsort = drop_target_index;
			watchListView.InsertPVIObjects(g_draggedPVIObject);
			g_draggedPVIObject = nullptr;
			TreeView_Select(objectTreeView.GetHandle(), 0, TVGN_DROPHILITE); // only for nicer view
		}
		return 0;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_TAB:
			SetFocus(objectTreeView.GetHandle());
			break;
		}
		return 0;

	case WM_MOUSEMOVE:
		if (drag_splitter)
		{
			GetClientRect(hWnd, &rect);
			if ((LOWORD(lParam) > 50) && (LOWORD(lParam) < (rect.right) - 50))
			{
				MoveWindow(objectTreeView.GetHandle(), 0, 0, LOWORD(lParam) - 1, rect.bottom, TRUE);
				MoveWindow(watchListView.GetHandle(), LOWORD(lParam) + 1, 0, rect.right - LOWORD(lParam) + 1, rect.bottom - writewnd_height, TRUE);
				//				MoveWindow(mywritelistviewparam.hwndLV, LOWORD(lParam) + 1, rect.bottom - writewnd_height, rect.right - LOWORD(lParam) + 1, writewnd_height, TRUE);
				splitter_xpos = LOWORD(lParam);
			}
		}
		if (g_draggedPVIObject != nullptr)
		{ // drag object from tree view to list view
			POINTS Pos;
			LVHITTESTINFO lvht;
			Pos.x = GET_X_LPARAM(lParam);
			Pos.y = GET_Y_LPARAM(lParam);
			ImageList_DragMove(Pos.x, Pos.y + 40);
			//
			memset(&lvht, 0, sizeof(lvht));
			lvht.pt.x = Pos.x;
			lvht.pt.y = Pos.y;
			ListView_HitTest(watchListView.GetHandle(), &lvht);
			if (lvht.flags & LVHT_NOWHERE)
			{																	   // drag within list view and not over other entry ?
				drop_target_index = ListView_GetItemCount(watchListView.GetHandle()); // to end of list
			}
			else if (lvht.flags & LVHT_ONITEM)
			{
				drop_target_index = lvht.iItem;
			}
			else
				drop_target_index = -1; // do not insert
			ListView_SetHotItem(watchListView.GetHandle(), lvht.iItem);
		}
		return 0;

	case WM_SYSCOMMAND:
		if (wParam == SC_MINIMIZE)
		{
			Shell_NotifyIconA(NIM_ADD, &notifyicondata);
			ShowWindow(g_hwndMainWindow, SW_HIDE);
			return 0;
		}
		break;

	case NOTIFY_ICON_MESSAGE:
		switch (LOWORD(lParam))
		{
		case WM_LBUTTONDOWN:
			ShowWindow(g_hwndMainWindow, SW_RESTORE);
			Shell_NotifyIconA(NIM_DELETE, &notifyicondata);
			break;
		}
		break;

	case NOTIFY_LOGGER_STATUS:
		if (IsLoggerRunning())
		{
			char logger_watch_file[MAX_PATH];

			// logger was started...
			ModifyMenuA(GetMenu(hWnd), IDM_LOGGER_START, MF_BYCOMMAND | MF_STRING, IDM_LOGGER_START, "Logger &Stop");
			EnableWindow(watchListView.GetHandle(), FALSE);
			EnableWindow(objectTreeView.GetHandle(), FALSE);
			WritePrivateProfileStringA("General", "LoggerActive", "1", SettingsGetFileName());
			strcpy(logger_watch_file, GetApplicationPath());
			strcat(logger_watch_file, "\\_logger.wtc");
			SaveWatchFile(logger_watch_file);
		}
		else
		{
			// logger was stopped
			ModifyMenuA(GetMenu(hWnd), IDM_LOGGER_START, MF_BYCOMMAND | MF_STRING, IDM_LOGGER_START, "Logger &Start");
			EnableWindow(watchListView.GetHandle(), TRUE);
			EnableWindow(objectTreeView.GetHandle(), TRUE);
			WritePrivateProfileStringA("General", "LoggerActive", "0", SettingsGetFileName());
		}
		break;

	case WM_CREATE:
		InitCommonControls();
		/* Imagelist */
		himl = ResourcesCreateImageList();

		g_draggedPVIObject = nullptr;

		/* TreeView */
		if (objectTreeView.Create(g_hInstance, hWnd) == nullptr)
			return -1;
		TreeView_SetImageList(objectTreeView.GetHandle(), himl, TVSIL_NORMAL);

		/* Listview */
		if (watchListView.Create(g_hInstance, hWnd) == nullptr)
			return -1;
		ListView_SetImageList(watchListView.GetHandle(), himl, LVSIL_SMALL);

		/* Schreibfenster */
		writewnd_height = 0;
		// if( write_enabled ){
		// writewnd_height = 20;
		// mywritelistviewparam.hwndLV = CreateWindowEx( 0, "EDIT", "", WS_CHILD | WS_VISIBLE,
		// 0, 0, 0, 0, hWnd, 0, ghInstance, NULL);
		// }

		if (PviObject::Start())
			MessageBoxA(g_hwndMainWindow, "Error during PVI initialization.\nssee log file !", "Error", MB_OK);
		EnableMenuItem(GetMenu(hWnd), IDM_LOGGER_UNZIPLOGGERFILE, MF_ENABLED);

		return (0);

	case WM_DESTROY:
		objectTreeView.StopCpuScan();
		if (hfont_watchlist != nullptr)
		{
			DeleteObject(hfont_watchlist);
			hfont_watchlist = nullptr;
		}
		PostQuitMessage(0);
		return (0);
	}

	return DefWindowProcA(hWnd, wMsg, wParam, lParam);
}
