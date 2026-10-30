#define WIN32_LEAN_AND_MEAN /* speed up compilations */
#include "mytreeview.h"
#include "mylistview.h"
#include "main.h"
#include "settings.h"

#define CX_BITMAP 16 /* Breite eines Bitmaps */
#define CY_BITMAP 16 /* Hoehe eines Bitmaps */

/* delete all elements from tree view */

void MyTreeViewClearAll(MYTREEVIEWPARAM *tv)
{

	TreeView_DeleteAllItems(tv->hwndTV);
}

// CreateMyTreeView - creates a tree view control.
// Returns the handle to the new control if successful,
//  or NULL otherwise.
// hwndParent - handle to the control's parent window.

#define MY_TREEVIEW_STYLE WS_VISIBLE | WS_CHILD | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS /*| TVS_TRACKSELECT */

HWND MyTreeViewCreateWindow(MYTREEVIEWPARAM *param)
{
	// RECT rcClient;				// dimensions of client area
	//  HIMAGELIST himl;  // handle to image list
	//  HICON hicon;
	//  int i;
	HFONT hfont;

	/* Ensure that the common control DLL is loaded.  */
	InitCommonControls();

	// GetClientRect( param->hwndParent, &rcClient);

	param->hwndTV = CreateWindowExA(WS_EX_CLIENTEDGE, WC_TREEVIEWA, "Select Object", MY_TREEVIEW_STYLE, 0, 0, 0, 0, param->hwndParent, nullptr, param->hinstance, nullptr);

	hfont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
	SendMessage(param->hwndTV, WM_SETFONT, (WPARAM)hfont, 1);

	MyTreeViewClearAll(param);

	return param->hwndTV;
}

// AddItemToMyTreeview - adds items to a tree view control.
// Returns the handle to the newly added item.
// hwndTV - handle to the tree view control.
// text - text of the item to add.
// level - level at which to add the item.

HTREEITEM MyTreeViewAddItem(MYTREEVIEWPARAM *tv, HTREEITEM parent, LPSTR text, LPARAM id, int icon_index)
{
	TVITEMA tvi;
	TVINSERTSTRUCTA tvins;
	HTREEITEM hti;

	memset(&tvi, 0, sizeof(tvi));

	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

	// Set the text of the item.
	tvi.pszText = text;
	// tvi.cchTextMax = lstrlen(text);

	tvi.iImage = icon_index;
	tvi.iSelectedImage = icon_index;
	tvi.cChildren = 1;

	// Eltern und Bruder setzen
	tvins.hParent = parent;
	tvins.hInsertAfter = TVI_LAST;

	// lParam setzen
	tvi.lParam = (LPARAM)id;

	// Add the item to the tree view control.
	tvins.item = tvi;
	hti = reinterpret_cast<HTREEITEM>(SendMessageA(tv->hwndTV, TVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&tvins)));

	// The new item is a child item. Give the parent item a
	// closed folder bitmap to indicate it now has child items.
	// if (level > 1) {
	// hti = TreeView_GetParent(hwndTV, hPrev);
	// tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	// tvi.hItem = hti;
	// tvi.iImage = 0;
	// tvi.iSelectedImage = 0;
	// TreeView_SetItem(hwndTV, &tvi);
	// }

	return hti;
}

LRESULT CALLBACK MyTreeViewHandleMessages(MYTREEVIEWPARAM *tv, UINT wMsg, WPARAM, LPARAM lParam)
{
	LPNMTREEVIEWA pnmtv;
	HTREEITEM hti;
	TVITEMA item;

	switch (wMsg)
	{

	case WM_NOTIFY:
		pnmtv = (LPNMTREEVIEWA)lParam;

		if (pnmtv->hdr.hwndFrom != tv->hwndTV)
		{
			return 1;
		}

		switch (pnmtv->hdr.code)
		{
		case NM_DBLCLK: // double click
			if (tv->cbdblclick != nullptr)
			{
				hti = TreeView_GetSelection(tv->hwndTV);
				memset(&item, 0, sizeof(item));
				item.mask = TVIF_HANDLE;
				item.hItem = hti;
				SendMessageA(tv->hwndTV, TVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&item));

				if (hti != nullptr)
				{
					(tv->cbdblclick)(hti, item.lParam);
				}
			}
			return 0;
			break;

		case NM_RCLICK: // right click
			if (tv->cbrclick != nullptr)
			{
				TVHITTESTINFO hit = {};
				GetCursorPos(&hit.pt);
				ScreenToClient(tv->hwndTV, &hit.pt);
				hti = TreeView_HitTest(tv->hwndTV, &hit);
				if (hti == nullptr || !(hit.flags & TVHT_ONITEM))
					return 0;
				memset(&item, 0, sizeof(item));
				item.mask = TVIF_PARAM;
				item.hItem = hti;
				SendMessageA(tv->hwndTV, TVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&item));

				if (hti != nullptr)
				{
					(tv->cbrclick)(hti, item.lParam);
				}
			}
			return 0;
			break;

		case TVN_SELCHANGEDA: // Selection changed
			if (tv->cbselected != nullptr)
			{
				(tv->cbselected)(pnmtv->itemNew.hItem, pnmtv->itemNew.lParam);
			}
			return 0;
			break;

		case TVN_BEGINDRAGA: // Begin Draggin
			if (tv->cbbegindrag != nullptr)
			{
				(tv->cbbegindrag)(pnmtv->itemNew.hItem, pnmtv->itemNew.lParam);
			}
			return 0;
		}
		break;
	}
	return 1;
}

/* get all child of object and enters them in tree view */

static void TreeViewInsertChildObjects(HTREEITEM hti, PviObject *parent)
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
			SendMessageA(mytreeviewparam.hwndTV, TVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&tvitem));
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
				SendMessageA(mytreeviewparam.hwndTV, TVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&tvitem));
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
					SendMessageA(mytreeviewparam.hwndTV, TVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&tvitem));
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

			MyTreeViewAddItem(&mytreeviewparam, hti, tempstring, (LPARAM)child->name, imageindex);
			child = parent->FindChild(FALSE);
		} /* End while()*/
	}
}

/* an element in tree view was clicked */

void MyTreeViewItemSelected(HTREEITEM hti, LPARAM lParam)
{
	PviObject *parent; // das angeklickte Objekte ist parent
	HCURSOR oldcursor;

	if (TreeView_GetChild(mytreeviewparam.hwndTV, hti) == nullptr)
	{ // noch nix eingelesen
		parent = PviObject::FindByName((char *)lParam);
		if (parent != nullptr)
		{
			if (parent->error == 0)
			{
				oldcursor = SetCursor(LoadCursor(nullptr, IDC_WAIT));
				TreeViewInsertChildObjects(hti, parent);
				SetCursor(oldcursor);
			}
		}
	}
}

void MyTreeViewBeginDragging(HTREEITEM hti, LPARAM lParam)
{
	HIMAGELIST himl;

	TreeView_Select(mytreeviewparam.hwndTV, hti, TVGN_DROPHILITE); // nur wg. Optik

	himl = TreeView_CreateDragImage(mytreeviewparam.hwndTV, hti);
	ImageList_BeginDrag(himl, 0, 0, 0);
	// start drag effect
	ImageList_DragEnter(g_hwndMainWindow, 0, 0);
	SetCapture(g_hwndMainWindow);

	g_draggedPVIObject = PviObject::FindByName((char *)lParam);
}

/* an element in tree view was double clicked */

void MyTreeViewDblClick(HTREEITEM /* hti */, LPARAM lParam)
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

static PviObject *MyTreeViewGetSelectedCpu(void)
{
	TVITEMA item = {};
	item.hItem = TreeView_GetSelection(mytreeviewparam.hwndTV);
	item.mask = TVIF_PARAM;
	if (item.hItem == nullptr ||
		!SendMessageA(mytreeviewparam.hwndTV, TVM_GETITEMA, 0, reinterpret_cast<LPARAM>(&item)) ||
		item.lParam == 0)
		return nullptr;
	PviObject *object = PviObject::FindByName(reinterpret_cast<char *>(item.lParam));
	return object != nullptr && object->type == POBJ_CPU ? object : nullptr;
}

static void MyTreeViewUpdateCpuSettings(PviObject *object, UINT command)
{
	if (object == nullptr || object->type != POBJ_CPU)
		return;
	BOOL success = TRUE;
	if (command == IDM_STORE_CPU_SETTINGS)
		success = SettingsStoreCpu(object->descriptor);
	else if (command == IDM_REMOVE_CPU_SETTINGS)
		success = SettingsRemoveCpu(object->descriptor);
	if (!success)
		MessageBoxA(g_hwndMainWindow, "Could not update the CPU settings. Check that brwatch.ini is writable and has a free CPU slot.",
			"CPU settings", MB_OK | MB_ICONERROR);
}

void MyTreeViewUpdateCpuSettingsMenu(HMENU menu)
{
	PviObject *object = MyTreeViewGetSelectedCpu();
	BOOL stored = object != nullptr && SettingsFindCpuSection(object->descriptor) != 0;
	EnableMenuItem(menu, IDM_STORE_CPU_SETTINGS, MF_BYCOMMAND |
		(object != nullptr && !stored ? MF_ENABLED : MF_GRAYED));
	EnableMenuItem(menu, IDM_REMOVE_CPU_SETTINGS, MF_BYCOMMAND |
		(stored ? MF_ENABLED : MF_GRAYED));
}

void MyTreeViewCpuSettingsCommand(UINT command)
{
	MyTreeViewUpdateCpuSettings(MyTreeViewGetSelectedCpu(), command);
}

/* mouse right click on tree view */
void MyTreeViewRClick(HTREEITEM hti, LPARAM lParam)
{
	PviObject *object;
	RECT rect_window;
	RECT rect_item;
	LPRECT lprect_item = &rect_item;

	if (GetWindowRect(mytreeviewparam.hwndTV, &rect_window))
	{
		if (TreeView_GetItemRect(mytreeviewparam.hwndTV, hti, lprect_item, TRUE))
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
						position.x, position.y, 0, g_hwndMainWindow, nullptr);
					DestroyMenu(menu);
					MyTreeViewUpdateCpuSettings(object, command);
					break;
				}
				case POBJ_DEVICE:
					break;

				case POBJ_PVAR:
					switch (object->ex.pv.type)
					{
					case BR_STRUCT:
						MyTreeViewItemSelected(hti, lParam);
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
