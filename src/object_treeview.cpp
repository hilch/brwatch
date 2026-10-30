#define WIN32_LEAN_AND_MEAN /* speed up compilations */
#include "object_treeview.h"
#include "mylistview.h"
#include "main.h"
#include "settings.h"

/*
The parent window owns the native control; GetHandle() returns a borrowed HWND.
The parent forwards notifications to HandleMessages(), which dispatches them
to this instance's event handlers.
*/

/* delete all elements from tree view */

void ObjectTreeView::ClearAll()
{

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

void ObjectTreeView::InsertChildObjects(HTREEITEM hti, PviObject *parent)
{
	PviObject *child;
	int imageindex;
	TVITEMA tvitem;

	if (parent != nullptr)
	{

		parent->Expand();

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

			AddItem(hti, tempstring, (LPARAM)child->name, imageindex);
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
				oldcursor = SetCursor(LoadCursor(nullptr, IDC_WAIT));
				InsertChildObjects(hti, parent);
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
		object->watchsort = ListView_GetItemCount(mylistviewparam.hwndLV);
		MyListViewInsertPVIObjects(object);
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
