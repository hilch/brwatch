#define WIN32_LEAN_AND_MEAN /* speed up compilations */
#include "object_treeview.h"
#include "watch_listview.h"
#include "main.h"
#include "settings.h"
#include "dlg_cpuscan.h"
#include <process.h>
#include <cerrno>
#include <algorithm>

struct ObjectTreeView::CpuScanTask {
	CpuScanControl control;
	std::vector<stEthernetCpuInfo> cpus = std::vector<stEthernetCpuInfo>(256);
	std::string parentName;
	DWORD parentLink = 0;
	HTREEITEM item = nullptr;
	HWND dialog = nullptr;
	HANDLE thread = nullptr;
	unsigned hosts = CpuScanDefaultHosts;
	bool allowBeyondSubnet = false;
	std::atomic<bool> done{false};
	int entries = 0;
	DWORD failure = ERROR_SUCCESS;
};

ObjectTreeView::ObjectTreeView() = default;

ObjectTreeView::~ObjectTreeView()
{
	StopCpuScan();
}

HWND ObjectTreeView::GetCpuScanDialogHandle() const
{
	return scanTask != nullptr ? scanTask->dialog : nullptr;
}

void ObjectTreeView::StopCpuScan()
{
	if (scanTask == nullptr)
		return;
	scanTask->control.cancelRequested.store(true);
	if (scanTask->thread != nullptr)
	{
		WaitForSingleObject(scanTask->thread, INFINITE);
		CloseHandle(scanTask->thread);
	}
	if (IsWindow(scanTask->dialog))
		DestroyWindow(scanTask->dialog);
	scanTask.reset();
}

unsigned __stdcall ObjectTreeView::CpuScanThread(void *data)
{
	auto task = static_cast<CpuScanTask *>(data);
	try
	{
		task->entries = SearchEthernetCpus(task->cpus.data(), static_cast<int>(task->cpus.size()),
			task->hosts, task->allowBeyondSubnet, &task->control);
	}
	catch (const std::bad_alloc &)
	{
		task->failure = ERROR_NOT_ENOUGH_MEMORY;
	}
	task->done.store(true);
	return 0;
}

void ObjectTreeView::StartCpuScan(HTREEITEM item, PviObject *parent, unsigned hosts, bool allowBeyondSubnet)
{
	try
	{
		scanTask = std::make_unique<CpuScanTask>();
		scanTask->parentName = parent->name;
	}
	catch (const std::bad_alloc &)
	{
		scanTask.reset();
		MessageBoxA(parentWindow, "Not enough memory to start the CPU scan.", "CPU scan", MB_OK | MB_ICONERROR);
		return;
	}
	scanTask->parentLink = parent->linkid;
	scanTask->item = item;
	scanTask->hosts = hosts;
	scanTask->allowBeyondSubnet = allowBeyondSubnet;
	scanTask->dialog = CreateDialogParamA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(DLG_CPU_SCAN_PROGRESS),
		parentWindow, CpuScanProgressDlgProc, reinterpret_cast<LPARAM>(this));
	if (scanTask->dialog == nullptr)
	{
		DWORD error = GetLastError();
		scanTask.reset();
		char message[160];
		snprintf(message, sizeof(message), "Could not create CPU scan progress dialog (error %lu).", error);
		MessageBoxA(parentWindow, message, "CPU scan", MB_OK | MB_ICONERROR);
		return;
	}
	if (SetTimer(scanTask->dialog, 1, 50, nullptr) == 0)
	{
		StopCpuScan();
		MessageBoxA(parentWindow, "Could not start the CPU scan progress timer.", "CPU scan", MB_OK | MB_ICONERROR);
		return;
	}
	scanTask->thread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, CpuScanThread, scanTask.get(), 0, nullptr));
	if (scanTask->thread == nullptr)
	{
		int error = errno;
		StopCpuScan();
		char message[160];
		snprintf(message, sizeof(message), "Could not start CPU scan worker (error %d).", error);
		MessageBoxA(parentWindow, message, "CPU scan", MB_OK | MB_ICONERROR);
		return;
	}
	ShowWindow(scanTask->dialog, IsWindowVisible(parentWindow) ? SW_SHOW : SW_HIDE);
	UpdateCpuScanProgress();
}

void ObjectTreeView::UpdateCpuScanProgress()
{
	if (scanTask == nullptr)
		return;
	HWND dialog = scanTask->dialog;
	if (scanTask->control.cancelRequested.load())
	{
		SetDlgItemTextA(dialog, IDC_CPU_SCAN_STATUS, "Cancelling... waiting for the current PVI/socket operation to finish.");
		EnableWindow(GetDlgItem(dialog, IDCANCEL), FALSE);
		return;
	}
	if (scanTask->control.phase.load() == CpuScanPhase::Snmp)
	{
		SendDlgItemMessageA(dialog, IDC_CPU_SCAN_PROGRESS, PBM_SETMARQUEE, TRUE, 30);
		SetDlgItemTextA(dialog, IDC_CPU_SCAN_STATUS, "Discovering CPUs via SNMP/PVI...");
	}
	else
	{
		HWND progress = GetDlgItem(dialog, IDC_CPU_SCAN_PROGRESS);
		LONG_PTR style = GetWindowLongPtrA(progress, GWL_STYLE);
		if ((style & PBS_MARQUEE) != 0)
		{
			SendMessageA(progress, PBM_SETMARQUEE, FALSE, 0);
			SetWindowLongPtrA(progress, GWL_STYLE, style & ~PBS_MARQUEE);
			SendMessageA(progress, PBM_SETRANGE32, 0, 1000);
		}
		unsigned total = scanTask->control.total.load();
		unsigned completed = (std::min)(scanTask->control.completed.load(), total);
		unsigned position = total == 0 ? 0 : static_cast<unsigned>(std::uint64_t(completed) * 1000 / total);
		SendMessageA(progress, PBM_SETPOS, position, 0);
		char message[160];
		snprintf(message, sizeof(message), "TCP scan: %u / %u addresses processed or skipped (%u%%).",
			completed, total, position / 10);
		SetDlgItemTextA(dialog, IDC_CPU_SCAN_STATUS, message);
	}
}

void ObjectTreeView::FinishCpuScan()
{
	auto task = std::move(scanTask);
	WaitForSingleObject(task->thread, INFINITE);
	CloseHandle(task->thread);
	if (IsWindow(task->dialog))
		DestroyWindow(task->dialog);
	if (task->control.cancelRequested.load())
		return;
	if (task->failure != ERROR_SUCCESS)
	{
		MessageBoxA(parentWindow, "Not enough memory to complete the CPU scan.", "CPU scan", MB_OK | MB_ICONERROR);
		return;
	}
	for (const auto &error : task->control.errors)
		MessageBoxA(parentWindow, error.c_str(), "CPU scan", MB_OK | MB_ICONERROR);
	PviObject *parent = PviObject::FindByName(task->parentName.c_str());
	if (parent == nullptr || parent->type != POBJ_DEVICE || parent->linkid != task->parentLink || task->entries < 0)
		return;
	TVITEMA item = {};
	item.hItem = task->item;
	item.mask = TVIF_PARAM;
	if (!IsWindow(window) || !TreeView_GetItem(window, &item) || item.lParam != reinterpret_cast<LPARAM>(parent->name))
		return;
	parent->AddEthernetCpus(task->cpus.data(), task->entries);
	InsertExistingChildObjects(task->item, parent);
}

INT_PTR CALLBACK ObjectTreeView::CpuScanProgressDlgProc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto view = reinterpret_cast<ObjectTreeView *>(GetWindowLongPtrA(dialog, DWLP_USER));
	if (message == WM_INITDIALOG)
	{
		SetWindowLongPtrA(dialog, DWLP_USER, lParam);
		return TRUE;
	}
	if (view == nullptr || view->scanTask == nullptr)
		return FALSE;
	if (message == WM_CLOSE || (message == WM_COMMAND && LOWORD(wParam) == IDCANCEL))
	{
		view->scanTask->control.cancelRequested.store(true);
		view->UpdateCpuScanProgress();
		return TRUE;
	}
	if (message == WM_TIMER)
	{
		if (view->scanTask->done.load())
			view->FinishCpuScan();
		else
			view->UpdateCpuScanProgress();
		return TRUE;
	}
	if (message == WM_DESTROY)
	{
		view->scanTask->control.cancelRequested.store(true);
		view->scanTask->dialog = nullptr;
		KillTimer(dialog, 1);
	}
	return FALSE;
}

/*
The parent window owns the native control; GetHandle() returns a borrowed HWND.
The parent forwards notifications to HandleMessages(), which dispatches them
to this instance's event handlers.
*/

/* delete all elements from tree view */

void ObjectTreeView::ClearAll()
{
	StopCpuScan();
	TreeView_DeleteAllItems(window);
}

#define MY_TREEVIEW_STYLE WS_VISIBLE | WS_CHILD | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS /*| TVS_TRACKSELECT */

HWND ObjectTreeView::Create(HINSTANCE instance, HWND parent)
{
	HFONT hfont;

	/* Ensure that the common control DLL is loaded.  */
	InitCommonControls();

	parentWindow = parent;
	window = CreateWindowExA(WS_EX_CLIENTEDGE, WC_TREEVIEWA, "Select Object", MY_TREEVIEW_STYLE, 0, 0, 0, 0, parentWindow, nullptr, instance, nullptr);
	if (window == nullptr)
	{
		MessageBoxA(parentWindow, "Could not create the object tree view.", "Error", MB_OK | MB_ICONERROR);
		return nullptr;
	}

	hfont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
	SendMessage(window, WM_SETFONT, (WPARAM)hfont, 1);

	ClearAll();

	return window;
}

HTREEITEM ObjectTreeView::AddItem(HTREEITEM parent, LPSTR text, LPARAM id, int iconIndex)
{
	TVITEMA tvi;
	TVINSERTSTRUCTA tvins;
	HTREEITEM hti;

	memset(&tvi, 0, sizeof(tvi));

	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

	// Set the text of the item.
	tvi.pszText = text;
	// tvi.cchTextMax = lstrlen(text);

	tvi.iImage = iconIndex;
	tvi.iSelectedImage = iconIndex;
	tvi.cChildren = 1;

	tvins.hParent = parent;
	tvins.hInsertAfter = TVI_LAST;

	tvi.lParam = (LPARAM)id;

	// Add the item to the tree view control.
	tvins.item = tvi;
	hti = reinterpret_cast<HTREEITEM>(SendMessageA(window, TVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&tvins)));

	return hti;
}

LRESULT ObjectTreeView::HandleMessages(UINT wMsg, WPARAM, LPARAM lParam)
{
	LPNMTREEVIEWA pnmtv;
	HTREEITEM hti;
	TVITEMA item;

	switch (wMsg)
	{

	case WM_NOTIFY:
		pnmtv = (LPNMTREEVIEWA)lParam;

		if (pnmtv->hdr.hwndFrom != window)
		{
			return 1;
		}

		switch (pnmtv->hdr.code)
		{
		case NM_DBLCLK: // double click
			hti = TreeView_GetSelection(window);
			if (hti != nullptr)
			{
				memset(&item, 0, sizeof(item));
				item.mask = TVIF_PARAM;
				item.hItem = hti;
				SendMessageA(window, TVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&item));
				DblClick(hti, item.lParam);
			}
			return 0;
			break;

		case NM_RCLICK: // right click
			{
				TVHITTESTINFO hit = {};
				GetCursorPos(&hit.pt);
				ScreenToClient(window, &hit.pt);
				hti = TreeView_HitTest(window, &hit);
				if (hti == nullptr || !(hit.flags & TVHT_ONITEM))
					return 0;
				memset(&item, 0, sizeof(item));
				item.mask = TVIF_PARAM;
				item.hItem = hti;
				SendMessageA(window, TVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&item));

				if (hti != nullptr)
				{
					RClick(hti, item.lParam);
				}
			}
			return 0;
			break;

		case TVN_SELCHANGEDA: // Selection changed
			ItemSelected(pnmtv->itemNew.hItem, pnmtv->itemNew.lParam);
			return 0;
			break;

		case TVN_BEGINDRAGA: // Begin Draggin
			BeginDragging(pnmtv->itemNew.hItem, pnmtv->itemNew.lParam);
			return 0;
		}
		break;
	}
	return 1;
}

/* get all child of object and enters them in tree view */

void ObjectTreeView::InsertChildObjects(HTREEITEM hti, PviObject *parent, unsigned hostsPerAdapter, bool allowBeyondSubnet)
{
	if (parent != nullptr)
	{
		parent->Expand(hostsPerAdapter, allowBeyondSubnet);
		InsertExistingChildObjects(hti, parent);
	}
}

void ObjectTreeView::InsertExistingChildObjects(HTREEITEM hti, PviObject *parent)
{
	PviObject *child;
	int imageindex;
	TVITEMA tvitem;

	if (parent != nullptr)
	{

		// there are more information after expanding
		if (parent->type == POBJ_CPU)
		{
			char tempstring[512];
			sprintf(tempstring, "%s %s %s", parent->ex.cpu.cputype, parent->ex.cpu.arversion, parent->descriptor);
			memset(&tvitem, 0, sizeof(tvitem));
			tvitem.hItem = hti;
			tvitem.mask = TVIF_TEXT;
			tvitem.pszText = tempstring;
			SendMessageA(window, TVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&tvitem));
		}
		else if (parent->type == POBJ_PVAR)
		{
			char tempstring[512];
			if (parent->ex.pv.type == BR_STRUCT)
			{
				if (parent->ex.pv.dimension > 1)
				{ // arrays of structs
					sprintf(tempstring, "%s   :%s[%u]", parent->descriptor, parent->ex.pv.pdatatype, parent->ex.pv.dimension);
				}
				else
				{
					sprintf(tempstring, "%s   :%s", parent->descriptor, parent->ex.pv.pdatatype);
				}
				memset(&tvitem, 0, sizeof(tvitem));
				tvitem.hItem = hti;
				tvitem.mask = TVIF_TEXT;
				tvitem.pszText = tempstring;
				SendMessageA(window, TVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&tvitem));
			}
			else
			{
				if (parent->ex.pv.dimension > 1)
				{ // arrays of basic data types
					sprintf(tempstring, "%s   :%s[%u]", parent->descriptor, parent->ex.pv.pdatatype, parent->ex.pv.dimension);
					memset(&tvitem, 0, sizeof(tvitem));
					tvitem.hItem = hti;
					tvitem.mask = TVIF_TEXT;
					tvitem.pszText = tempstring;
					SendMessageA(window, TVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&tvitem));
				}
			}
		}

		child = parent->FindChild(TRUE);
		while (child != nullptr)
		{
			char tempstring[512];

			switch (parent->type)
			{
			case POBJ_PVAR:
				if (parent->ex.pv.type == BR_STRUCT && parent->ex.pv.dimension == 1)
				{
					strcpy(tempstring, child->descriptor + strlen(parent->descriptor) + 1);
				}
				else
				{
					strcpy(tempstring, child->descriptor);
				}
				break;

			case POBJ_DEVICE:
				sprintf(tempstring, "%s %s %s", child->ex.cpu.cputype, child->ex.cpu.arversion, child->descriptor);
				break;

			default:
				strcpy(tempstring, child->descriptor);
				break;
			}

			imageindex = ResourcesGetPviObjectImage(child);

			bool exists = false;
			for (HTREEITEM existing = TreeView_GetChild(window, hti); existing != nullptr;
				existing = TreeView_GetNextSibling(window, existing))
			{
				TVITEMA item = {};
				item.hItem = existing;
				item.mask = TVIF_PARAM;
				if (TreeView_GetItem(window, &item) && item.lParam == reinterpret_cast<LPARAM>(child->name))
				{
					exists = true;
					break;
				}
			}
			if (!exists)
				AddItem(hti, tempstring, reinterpret_cast<LPARAM>(child->name), imageindex);
			child = parent->FindChild(FALSE);
		} /* End while()*/
	}
}

/* an element in tree view was clicked */

void ObjectTreeView::ItemSelected(HTREEITEM hti, LPARAM lParam)
{
	PviObject *parent;
	HCURSOR oldcursor;

	if (TreeView_GetChild(window, hti) == nullptr)
	{
		parent = PviObject::FindByName((char *)lParam);
		if (parent != nullptr)
		{
			if (parent->error == 0)
			{
				if (parent->type == POBJ_DEVICE && scanTask != nullptr)
				{
					SetForegroundWindow(scanTask->dialog);
					return;
				}
				unsigned hosts = CpuScanDefaultHosts;
				bool allowBeyondSubnet = false;
				if (parent->type == POBJ_DEVICE && !DlgCpuScanShowDialog(parentWindow, hosts, allowBeyondSubnet))
					return;
				if (parent->type == POBJ_DEVICE)
				{
					char descriptor[sizeof(parent->descriptor)];
					strcpy(descriptor, parent->descriptor);
					if (strstr(_strupr(descriptor), "/IF=TCPIP") != nullptr)
					{
						StartCpuScan(hti, parent, hosts, allowBeyondSubnet);
						return;
					}
				}
				oldcursor = SetCursor(LoadCursor(nullptr, IDC_WAIT));
				InsertChildObjects(hti, parent, hosts, allowBeyondSubnet);
				SetCursor(oldcursor);
			}
		}
	}
}

void ObjectTreeView::BeginDragging(HTREEITEM hti, LPARAM lParam)
{
	HIMAGELIST himl;

	TreeView_Select(window, hti, TVGN_DROPHILITE);

	himl = TreeView_CreateDragImage(window, hti);
	ImageList_BeginDrag(himl, 0, 0, 0);
	// start drag effect
	ImageList_DragEnter(parentWindow, 0, 0);
	SetCapture(parentWindow);

	g_draggedPVIObject = PviObject::FindByName((char *)lParam);
}

/* an element in tree view was double clicked */

void ObjectTreeView::DblClick(HTREEITEM /* hti */, LPARAM lParam)
{
	PviObject *object;
	object = PviObject::FindByName((char *)lParam);
	if (object != nullptr)
	{
		if (object->type == POBJ_PVAR)
		{
			if (object->ex.pv.type == BR_STRUCT || object->ex.pv.dimension > 1)
			{
				object->watchsort = -1;
				return; // do not insert structures or arrays
			}
		}
		object->watchsort = ListView_GetItemCount(watchListView.GetHandle());
		watchListView.InsertPVIObjects(object);
	}
}

PviObject *ObjectTreeView::GetSelectedCpu() const
{
	TVITEMA item = {};
	item.hItem = TreeView_GetSelection(window);
	item.mask = TVIF_PARAM;
	if (item.hItem == nullptr ||
		!SendMessageA(window, TVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&item)) ||
		item.lParam == 0)
		return nullptr;
	PviObject *object = PviObject::FindByName(reinterpret_cast<char *>(item.lParam));
	return object != nullptr && object->type == POBJ_CPU ? object : nullptr;
}

void ObjectTreeView::UpdateCpuSettings(PviObject *object, UINT command)
{
	if (object == nullptr || object->type != POBJ_CPU)
		return;
	BOOL success = TRUE;
	if (command == IDM_STORE_CPU_SETTINGS)
		success = SettingsStoreCpu(object->descriptor);
	else if (command == IDM_REMOVE_CPU_SETTINGS)
		success = SettingsRemoveCpu(object->descriptor);
	if (!success)
		MessageBoxA(parentWindow, "Could not update the CPU settings. Check that brwatch.ini is writable and has a free CPU slot.",
			"CPU settings", MB_OK | MB_ICONERROR);
}

void ObjectTreeView::UpdateCpuSettingsMenu(HMENU menu)
{
	PviObject *object = GetSelectedCpu();
	BOOL stored = object != nullptr && SettingsFindCpuSection(object->descriptor) != 0;
	EnableMenuItem(menu, IDM_STORE_CPU_SETTINGS, MF_BYCOMMAND |
		(object != nullptr && !stored ? MF_ENABLED : MF_GRAYED));
	EnableMenuItem(menu, IDM_REMOVE_CPU_SETTINGS, MF_BYCOMMAND |
		(stored ? MF_ENABLED : MF_GRAYED));
}

void ObjectTreeView::CpuSettingsCommand(UINT command)
{
	UpdateCpuSettings(GetSelectedCpu(), command);
}

/* mouse right click on tree view */
void ObjectTreeView::RClick(HTREEITEM hti, LPARAM lParam)
{
	PviObject *object;
	RECT rect_window;
	RECT rect_item;
	LPRECT lprect_item = &rect_item;

	if (GetWindowRect(window, &rect_window))
	{
		if (TreeView_GetItemRect(window, hti, lprect_item, TRUE))
		{

			object = PviObject::FindByName((char *)lParam);
			if (object != nullptr)
			{
				switch (object->type)
				{
				case POBJ_CPU:
				{
					HMENU menu = CreatePopupMenu();
					if (menu == nullptr)
						break;
					BOOL stored = SettingsFindCpuSection(object->descriptor) != 0;
					AppendMenuA(menu, MF_STRING | (stored ? MF_GRAYED : MF_ENABLED),
						IDM_STORE_CPU_SETTINGS, "Store to settings");
					AppendMenuA(menu, MF_STRING | (stored ? MF_ENABLED : MF_GRAYED),
						IDM_REMOVE_CPU_SETTINGS, "Remove from settings");
					POINT position;
					GetCursorPos(&position);
					UINT command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON,
						position.x, position.y, 0, parentWindow, nullptr);
					DestroyMenu(menu);
					UpdateCpuSettings(object, command);
					break;
				}
				case POBJ_DEVICE:
					break;

				case POBJ_PVAR:
					switch (object->ex.pv.type)
					{
					case BR_STRUCT:
						ItemSelected(hti, lParam);
						if (object->ex.pv.pdatatype != nullptr)
						{
						}
						break;

					default:
						break;
					}
					break;

				default:
					break;
				}
			}
		}
	}
}
