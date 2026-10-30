/*
 * brwatch - PVI diagnosis tool for B&R PLCs
 *
 * Repository: https://github.com/hilch/brwatch
 *
 * SPDX-License-Identifier: GPL-3.0-only
 * Licensed under the GNU General Public License version 3.
 * See LICENSE for details.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#define WIN32_LEAN_AND_MEAN /* speed up compilations */
#include <windows.h>
#include <vector>
#include "watch_listview.h"
#include "main.h"
#include "stringtools.h"
#include "settings.h"
#include "dlg_writepar.h"
#include "dlg_editcpu.h"
#include "dlg_edittask.h"

/*
The parent owns the native window; GetHandle() returns a borrowed HWND.
Notifications are forwarded to HandleMessages(). PVI callbacks call UpdateValue()
through the application's callback bridge.
*/

void WatchListView::SetItemText(int index, int subItem, char *text)
{
	LVITEMA item = {};
	item.iSubItem = subItem;
	item.pszText = text;
	SendMessageA(window, LVM_SETITEMTEXTA, index, reinterpret_cast<LPARAM>(&item));
}

HWND WatchListView::Create(HINSTANCE instance, HWND parent)
{
	HFONT hfont;
	LVCOLUMNA lvcolumn;
	int i;

	parentWindow = parent;
	window = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEWW, nullptr, WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT,
							0, 0, 0, 0, parentWindow, nullptr, instance, nullptr);
	if (window == nullptr)
	{
		MessageBoxA(parentWindow, "Could not create the watch list view.", "Error", MB_OK | MB_ICONERROR);
		return nullptr;
	}

	const char *columnNames[] = {"Name", "Type", "Scope", "Value"};
	const int columnWidths[] = {100, 100, 100, 600};
	for (i = 0; i < 4; ++i)
	{
		memset(&lvcolumn, 0, sizeof(lvcolumn));
		lvcolumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvcolumn.fmt = LVCF_TEXT;
		lvcolumn.cx = columnWidths[i];
		lvcolumn.cchTextMax = (int)strlen(columnNames[i]);
		lvcolumn.iSubItem = i;
		std::vector<char> columnName(columnNames[i], columnNames[i] + strlen(columnNames[i]) + 1);
		lvcolumn.pszText = columnName.data();
		SendMessageA(window, LVM_INSERTCOLUMNA, i, reinterpret_cast<LPARAM>(&lvcolumn));
	}

	ListView_SetExtendedListViewStyleEx(window, 0,
										LVS_EX_FULLROWSELECT		// select full row
											| LVS_EX_GRIDLINES
											| LVS_EX_HEADERDRAGDROP
											| LVS_EX_SUBITEMIMAGES
											| LVS_EX_TRACKSELECT
	);

	hfont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
	SendMessage(window, WM_SETFONT, (WPARAM)hfont, TRUE);

	return window;
}

LRESULT WatchListView::HandleMessages(UINT wMsg, WPARAM /*wParam*/, LPARAM lParam)
{
	LPNMLISTVIEW pnmlv;
	LPNMLVKEYDOWN pnkd;

	switch (wMsg)
	{

	case WM_NOTIFY:
		pnmlv = (LPNMLISTVIEW)lParam;

		if (pnmlv->hdr.hwndFrom != window)
		{
			return 1;
		}

		switch (pnmlv->hdr.code)
		{
		case NM_DBLCLK:
			DblClick(pnmlv);
			return 0;
			break;

		case NM_RCLICK:
			RClick(pnmlv);
			return 0;
			break;

		case LVN_KEYDOWN:
			pnkd = (LPNMLVKEYDOWN)lParam;
			Keydown(pnkd->wVKey);
			return 0;
			break;

		case LVN_ITEMACTIVATE:
			Activate(pnmlv);
			return 0;
			break;

		case LVN_BEGINDRAG: // Begin Draggin
			BeginDragging(pnmlv->iItem);
			return 0;
		}

		break;

	default:

		break;
	}

	return 1;
}

/* insert object to list view */

void WatchListView::InsertPVIObjects(PviObject *object)
{
	LVITEMA lvitem;
	char *text;
	BOOL found = FALSE;
	int i;

	if (object != nullptr)
	{
		// do not insert if item already exists
		found = FALSE;
		i = ListView_GetNextItem(window, -1, LVNI_ALL);
		while (i != -1)
		{
			memset(&lvitem, 0, sizeof(lvitem));
			lvitem.mask = LVIF_PARAM;
			lvitem.iItem = i;
			ListView_GetItem(window, &lvitem);
			if (lvitem.lParam != 0)
			{
				if (strcmp(object->name, (char *)lvitem.lParam) == 0)
				{
					found = TRUE;
				}
			}
			i = ListView_GetNextItem(window, i, LVNI_ALL);
		}

		if (found == TRUE)
			return;

		if (object->watchsort < 0)
		{ // object is not listed
			return;
		}

		/* insert a process variable */
		if (object->type == POBJ_PVAR)
		{
			if (object->Watch(TRUE) != nullptr)
			{
				char tempstring[256];

				if (object->ex.pv.task != nullptr)
				{
					PviObject *task = object->ex.pv.task;

					strcpy(tempstring, task->descriptor);
					strcat(tempstring, ":");
				}
				else
				{
					strcpy(tempstring, ""); // should never happen
				}
				strcat(tempstring, object->descriptor);

				memset(&lvitem, 0, sizeof(lvitem));
				lvitem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
				lvitem.pszText = tempstring;
				lvitem.lParam = (LPARAM)object->name;
				lvitem.iImage = ResourcesGetImageIndex(IDR_ICO_VARIABLE);
				lvitem.iItem = object->watchsort;

				/* name */
				SendMessageA(window, LVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&lvitem));
				ListView_SetColumnWidth(window, 0, LVSCW_AUTOSIZE);

				/* data type */
				if (object->ex.pv.type == BR_STRING)
				{
					sprintf(tempstring, "%1s%s(%lu)", object->ex.pv.scope[0] == 'd' ? "*" : "", object->ex.pv.pdatatype, object->ex.pv.length - 1);
				}
				else if (object->ex.pv.type == BR_WSTRING)
				{
					sprintf(tempstring, "%1s%s(%lu)", object->ex.pv.scope[0] == 'd' ? "*" : "", object->ex.pv.pdatatype, object->ex.pv.length / 2 - 1);
				}
				else
				{
					sprintf(tempstring, "%1s%s", object->ex.pv.scope[0] == 'd' ? "*" : "", object->ex.pv.pdatatype);
				}
				text = tempstring;
				SetItemText(lvitem.iItem, 1, text); // data type
				memset(&lvitem, 0, sizeof(lvitem));
				lvitem.mask = LVIF_IMAGE;
				lvitem.iImage = ResourcesGetPviObjectImage(object);
				lvitem.iSubItem = 1;
				lvitem.iItem = object->watchsort;
				SendMessageA(window, LVM_SETITEMA, 0, reinterpret_cast<LPARAM>(&lvitem)); // set icon
				ListView_SetColumnWidth(window, 1, LVSCW_AUTOSIZE);

				/* value range */
				switch (object->ex.pv.scope[0])
				{
				case 'g':
					strcpy(text, "<GLOBAL>");
					break;
				case 'l':
					strcpy(text, "<LOCAL >");
					break;
				case 'd':
					strcpy(text, "<DYNAM.>");
					break;
				}
				SetItemText(lvitem.iItem, 2, text); // Scope
				ListView_SetColumnWidth(window, 2, LVSCW_AUTOSIZE);

				/* value */
				strcpy(text, " ");
				SetItemText(lvitem.iItem, 3, text); // value
				//
				ListView_EnsureVisible(window, lvitem.iItem, TRUE);
				ListView_SetColumnWidth(window, 3, LVSCW_AUTOSIZE);
			}

			/* insert a CPU */
		}
		else if (object->type == POBJ_CPU)
		{
			char tempstring[512];

			sprintf(tempstring, "%s %s %s", object->ex.cpu.cputype, object->ex.cpu.arversion, object->descriptor);
			memset(&lvitem, 0, sizeof(lvitem));
			lvitem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
			lvitem.pszText = tempstring;
			lvitem.lParam = (LPARAM)object->name;
			lvitem.iImage = ResourcesGetPviObjectImage(object);
			lvitem.iItem = object->watchsort;

			/* name */
			SendMessageA(window, LVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&lvitem));
			ListView_SetColumnWidth(window, 0, LVSCW_AUTOSIZE_USEHEADER);

			/* type */
			strcpy(tempstring, "CPU ");
			SetItemText(lvitem.iItem, 1, tempstring);
			ListView_SetColumnWidth(window, 1, LVSCW_AUTOSIZE_USEHEADER);

			/* scope */
			ListView_SetColumnWidth(window, 2, LVSCW_AUTOSIZE_USEHEADER);

			/* value */
			switch (object->error)
			{
			case 0:
				strcpy(tempstring, "(running) RTC:0000/00/00-00:00:00");
				break;

			case 4808: // no connection to plc
			case 11022:
				sprintf(tempstring, "(OFFLINE)");
				break;

			default:
				sprintf(tempstring, "Err:%i", object->error);
				break;
			}
			SetItemText(lvitem.iItem, 3, tempstring); // value
			ListView_SetColumnWidth(window, 3, LVSCW_AUTOSIZE);

			/* insert a task */
		}
		else if (object->type == POBJ_TASK)
		{
			char tempstring[256];

			strcpy(tempstring, object->descriptor);

			memset(&lvitem, 0, sizeof(lvitem));
			lvitem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
			lvitem.pszText = tempstring;
			lvitem.lParam = (LPARAM)object->name;
			lvitem.iImage = ResourcesGetPviObjectImage(object);
			lvitem.iItem = object->watchsort;

			/* name */
			SendMessageA(window, LVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&lvitem));
			ListView_SetColumnWidth(window, 0, LVSCW_AUTOSIZE);

			/* type */
			strcpy(tempstring, "TASK");
			SetItemText(lvitem.iItem, 1, tempstring);
			ListView_SetColumnWidth(window, 1, LVSCW_AUTOSIZE);

		}

		// save list index of element into object information
		i = ListView_GetNextItem(window, -1, LVNI_ALL);
		while (i != -1)
		{
			memset(&lvitem, 0, sizeof(lvitem));
			lvitem.mask = LVIF_PARAM;
			lvitem.iItem = i;
			ListView_GetItem(window, &lvitem);
			if (lvitem.lParam != 0)
			{
				object = PviObject::FindByName((char *)lvitem.lParam);
				if (object != nullptr)
				{
					object->watchsort = lvitem.iItem;
				}
			}
			i = ListView_GetNextItem(window, i, LVNI_ALL);
		}
	}
}

/* translates ojbect's data into ASCII and enters them into list view */

void WatchListView::UpdateValue(PviObject *object)
{
	LVFINDINFO lvfindinfo;
	int index;
	const char *format_decimal = nullptr;
	const char *format_hex = nullptr;
	const char *format_binary = nullptr;
	const char *format_char = nullptr;
	char *text_binary = nullptr;
	long long intval = 0;
	int hours, minutes, seconds, milliseconds;
	char value[2048];
	char tempstring[256];

	memset(&lvfindinfo, 0, sizeof(lvfindinfo));

	lvfindinfo.flags = LVFI_PARAM;
	lvfindinfo.lParam = (LPARAM)object->name;

	index = ListView_FindItem(window, -1, &lvfindinfo);
	if (index >= 0)
	{ // found object in list view
		switch (object->type)
		{
		case POBJ_PVAR:
			switch (object->ex.pv.type)
			{
			case BR_USINT:
				if (object->ex.pv.pvalue != nullptr)
				{
					unsigned char x;
					memcpy(&x, object->ex.pv.pvalue, 1);
					intval = x;
				}
				format_decimal = "10#%+3llu  ";
				format_hex = "16#%2.2llX  ";
				format_binary = "2#%s  ";
				format_char = "'%c'";
				text_binary = int2bin(intval, 8);
				break;

			case BR_SINT:
				if (object->ex.pv.pvalue != nullptr)
				{
					signed char x;
					memcpy(&x, object->ex.pv.pvalue, 1);
					intval = x;
				}
				format_decimal = "10#%+3lli  ";
				format_hex = "16#%2.2llX  ";
				format_binary = "2#%s  ";
				format_char = "'%c'";
				text_binary = int2bin(intval, 8);
				break;

			case BR_UINT:
				if (object->ex.pv.pvalue != nullptr)
				{
					unsigned short x;
					memcpy(&x, object->ex.pv.pvalue, 2);
					intval = x;
				}
				format_decimal = "10#%+5llu  ";
				format_hex = "16#%4.4llX  ";
				format_binary = "2#%s";
				format_char = "";
				text_binary = int2bin(intval, 16);
				break;

			case BR_INT:
				if (object->ex.pv.pvalue != nullptr)
				{
					signed short x;
					memcpy(&x, object->ex.pv.pvalue, 2);
					intval = x;
				}
				format_decimal = "10#%+5lli  ";
				format_hex = "16#%4.4llX  ";
				format_binary = "2#%s";
				format_char = "";
				text_binary = int2bin(intval, 16);
				break;

			case BR_UDINT:
				if (object->ex.pv.pvalue != nullptr)
				{
					unsigned int x;
					memcpy(&x, object->ex.pv.pvalue, 4);
					intval = x;
				}
				format_decimal = "10#%+10llu  ";
				format_hex = "16#%8.8llX  ";
				format_binary = "2#%s";
				format_char = "";
				text_binary = int2bin(intval, 32);
				break;

			case BR_DINT:
			case BR_TIME:
				if (object->ex.pv.pvalue != nullptr)
				{
					signed int x;
					memcpy(&x, object->ex.pv.pvalue, 4);
					intval = x;
				}
				format_decimal = "10#%+10lli  ";
				format_hex = "16#%8.8llX  ";
				format_binary = "2#%s";
				format_char = "";
				text_binary = int2bin(intval, 32);
				break;

			case BR_BOOL:
				if (object->ex.pv.pvalue != nullptr)
				{
					unsigned char x;
					memcpy(&x, object->ex.pv.pvalue, 1);
					intval = x;
				}
				format_decimal = "%1.1i    ";
				format_hex = "";
				format_char = "";
				format_binary = intval ? "TRUE" : "FALSE";
				break;

			case BR_REAL:
				if (object->ex.pv.pvalue != nullptr)
				{
					sprintf(value, "%g", *((float *)object->ex.pv.pvalue));
				}
				break;

			case BR_LREAL:
				if (object->ex.pv.pvalue != nullptr)
				{
					sprintf(value, "%g", *((double *)object->ex.pv.pvalue));
				}
				break;

			case BR_STRING:
			{
				std::vector<char> text;
				if (object->ex.pv.pvalue != nullptr)
				{
					const char *s = static_cast<const char *>(object->ex.pv.pvalue);
					for (DWORD i = 0; i < object->ex.pv.length && s[i] != 0; ++i)
					{
						unsigned char character = static_cast<unsigned char>(s[i]);
						if (character < ' ')
						{
							char escaped[6];
							snprintf(escaped, sizeof(escaped), "\\x%3.3x", character);
							text.insert(text.end(), escaped, escaped + 5);
						}
						else
						{
							text.push_back(s[i]);
						}
					}
				}
				text.push_back(0);
				if (object->gui_info.interpret_as_oem)
				{
					const auto oemText = text;
					OemToCharA(oemText.data(), text.data());
				}
				SetItemText(index, 3, text.data());
				return;
			}

			case BR_WSTRING:
				if (object->ex.pv.pvalue != nullptr)
				{
					LV_ITEMW item;
					item.iSubItem = 3;
					item.pszText = static_cast<LPWSTR>(object->ex.pv.pvalue);
					SendMessageW(window, LVM_SETITEMTEXTW, index, (LPARAM)(LV_ITEMW *)&item);
				}
				return;

			case BR_DATI:
			case BR_DATE:
				if (object->ex.pv.pvalue != nullptr)
				{
					struct tm *ptst;
					__time64_t t = 0;

					memset(value, 0, sizeof(value));

					memcpy(&t, object->ex.pv.pvalue, sizeof(time_t));
					ptst = _gmtime64(&t);
					if (ptst != nullptr)
						strncpy(value, asctime(ptst), 24);
					else
						strcpy(value, "(illegal time!)");

					if (object->gui_info.display_as_decimal)
					{
						char tempstring[20];
						sprintf(tempstring, "  (10#%.5u)", *((unsigned int *)object->ex.pv.pvalue));
						strcat(value, tempstring);
					}
				}
				break;

			case BR_TOD:
				if (object->ex.pv.pvalue != nullptr)
				{
					memcpy(&intval, object->ex.pv.pvalue, 4);
					printf("%u", (unsigned int)intval);
					hours = (int)intval / (3600 * 1000);
					intval -= hours * (3600 * 1000);
					minutes = (int)intval / (60 * 1000);
					intval -= minutes * (60 * 1000);
					seconds = (int)intval / 1000;
					intval -= seconds * 1000;
					milliseconds = (int)intval;
					sprintf(value, "%2.1u:%2.1u:%2.1u:%3.3u", hours, minutes, seconds, milliseconds);
					if (object->gui_info.display_as_decimal)
					{
						char tempstring[20];
						sprintf(tempstring, "  (10#%.5u)", *((unsigned int *)object->ex.pv.pvalue));
						strcat(value, tempstring);
					}
				}
				break;

			default:
				strcpy(value, "???");
				break;
			}

			// change display format of integers
			if (object->ex.pv.type == BR_USINT || object->ex.pv.type == BR_SINT ||
				object->ex.pv.type == BR_UINT || object->ex.pv.type == BR_INT ||
				object->ex.pv.type == BR_UDINT || object->ex.pv.type == BR_DINT ||
				object->ex.pv.type == BR_TIME ||
				object->ex.pv.type == BR_BOOL)
			{
				strcpy(value, "");
				if (object->gui_info.display_as_decimal)
				{
					sprintf(tempstring, format_decimal, intval);
					strcat(value, tempstring);
				}
				if (object->gui_info.display_as_hex)
				{
					sprintf(tempstring, format_hex, intval);
					strcat(value, tempstring);
				}
				if (object->gui_info.display_as_binary)
				{
					sprintf(tempstring, format_binary, text_binary);
					strcat(value, tempstring);
				}
				if (object->gui_info.display_as_char)
				{
					sprintf(tempstring, format_char, intval);
					strcat(value, tempstring);
				}
			}

			break;

		case POBJ_CPU:
			sprintf(value, "(%s) RTC:%4.4u/%2.2u/%2.2u-%2.2u:%2.2u:%2.2u",
					object->ex.cpu.running ? "running" : "stopped",
					object->ex.cpu.rtc_time.tm_year + 1900,
					object->ex.cpu.rtc_time.tm_mon + 1,
					object->ex.cpu.rtc_time.tm_mday,
					object->ex.cpu.rtc_time.tm_hour,
					object->ex.cpu.rtc_time.tm_min,
					object->ex.cpu.rtc_time.tm_sec);
			break;

		case POBJ_TASK:
			if (((PviObject *)object->ex.task.cpu)->ex.cpu.running == FALSE)
			{
				strcpy(value, "(CPU not running)");
			}
			else
			{
				strcpy(value, object->ex.task.status);
			}
			break;

		default:
			strcpy(value, "");
			break;
		}

		SetItemText(index, 3, value);
	}
}

/* is called on key change in list view */

void WatchListView::Keydown(WORD vkey)
{
	int index;
	LVITEM lvitem;
	PviObject *object;

	switch (vkey)
	{
	case VK_DELETE:
		index = ListView_GetNextItem(window, -1, LVNI_SELECTED);
		while (index != -1)
		{
			memset(&lvitem, 0, sizeof(lvitem));
			lvitem.iItem = index;
			lvitem.mask = LVIF_PARAM;
			ListView_GetItem(window, &lvitem);
			object = PviObject::FindByName((char *)lvitem.lParam);
			if (object != nullptr)
				object->Watch(FALSE);
			ListView_DeleteItem(window, index); // remove from list view
			index = ListView_GetNextItem(window, index, LVNI_SELECTED);
		}
		break;

	case VK_RETURN:
		index = ListView_GetNextItem(window, -1, LVNI_SELECTED);
		if (index != -1)
		{
			memset(&lvitem, 0, sizeof(lvitem));
			lvitem.iItem = index;
			lvitem.mask = LVIF_PARAM;
			ListView_GetItem(window, &lvitem);

			if (lvitem.lParam != 0)
			{
				object = PviObject::FindByName((char *)lvitem.lParam);
				DlgWritePvarShowDialog(object);
				DlgEditCpuShowDialog(object);
				DlgEditTaskShowDialog(object);
			}
		}
		break;
	}
}

/* is called on double click in list view */

void WatchListView::DblClick(LPNMLISTVIEW lpnm)
{
	LVITEM lvitem;
	PviObject *object;
	BOOL write_allowed;

	// Read write permissions from the INI file.
	write_allowed = GetPrivateProfileIntA("General", "WriteAllowed", 0, SettingsGetFileName());

	if (!write_allowed)
		return;

	memset(&lvitem, 0, sizeof(lvitem));
	lvitem.mask = LVIF_PARAM;
	lvitem.iItem = lpnm->iItem;
	ListView_GetItem(window, &lvitem);

	if (lvitem.lParam != 0)
	{
		object = PviObject::FindByName((char *)lvitem.lParam);
		DlgWritePvarShowDialog(object);
		DlgEditCpuShowDialog(object);
		DlgEditTaskShowDialog(object);
	}
}

void WatchListView::RClick(LPNMLISTVIEW lpnm)
{
	HMENU hmenu;
	// RECT offset;
	LVITEM lvitem;
	UINT enabled;
	const char *entry;
	POINT pt;

	memset(&lvitem, 0, sizeof(lvitem));
	lvitem.mask = LVIF_PARAM;
	lvitem.iItem = lpnm->iItem;
	ListView_GetItem(window, &lvitem);

	if (lvitem.lParam == 0)
		return; // should not happen :-)

	g_selectedPVIObject = PviObject::FindByName((char *)lvitem.lParam);

	if (g_selectedPVIObject == nullptr)
		return; // should also not happen :-)

	GetCursorPos(&pt);

	hmenu = CreatePopupMenu();

	switch (g_selectedPVIObject->type)
	{
	case POBJ_PVAR:
		switch (g_selectedPVIObject->ex.pv.type)
		{
		case BR_USINT:
		case BR_SINT:
			enabled = g_selectedPVIObject->gui_info.display_as_char ? MF_CHECKED : MF_UNCHECKED;
			AppendMenuA(hmenu, MF_ENABLED | MF_STRING | enabled, IDM_CHANGE_VIEW_CHAR, "char");
			break;
		case BR_UINT:
		case BR_INT:
		case BR_UDINT:
		case BR_DINT:
		case BR_TIME:
			enabled = g_selectedPVIObject->gui_info.display_as_decimal ? MF_CHECKED : MF_UNCHECKED;
			AppendMenuA(hmenu, MF_ENABLED | MF_STRING | enabled, IDM_CHANGE_VIEW_DECIMAL, "dec");
			enabled = g_selectedPVIObject->gui_info.display_as_hex ? MF_CHECKED : MF_UNCHECKED;
			AppendMenuA(hmenu, MF_ENABLED | MF_STRING | enabled, IDM_CHANGE_VIEW_HEX, "hex");
			enabled = g_selectedPVIObject->gui_info.display_as_binary ? MF_CHECKED : MF_UNCHECKED;
			AppendMenuA(hmenu, MF_ENABLED | MF_STRING | enabled, IDM_CHANGE_VIEW_BINARY, "bin");
			break;

		case BR_REAL:
		case BR_LREAL:
			AppendMenuA(hmenu, MF_ENABLED | MF_STRING, 0, "no option");
			break;

		case BR_STRING:
			entry = g_selectedPVIObject->gui_info.interpret_as_oem ? "interpret as ANSI" : "interpret as OEM";
			AppendMenuA(hmenu, MF_ENABLED | MF_STRING, IDM_CHANGE_VIEW_OEMCHAR, entry);
			break;

		case BR_DATI:
			entry = g_selectedPVIObject->gui_info.display_as_decimal ? "hide decimal value" : "show decimal value";
			AppendMenuA(hmenu, MF_ENABLED | MF_STRING, IDM_CHANGE_VIEW_DECIMAL, entry);
			break;

		default:
			break;
		}

		break;

	default:
		break;
	}

	AppendMenuA(hmenu, MF_ENABLED | MF_STRING, IDM_DELETE_OBJECT_FROM_LIST, "remove this object");
	// TrackPopupMenu( hmenu, TPM_VERTICAL, offset.left + lpnm->ptAction.x , offset.top + lpnm->ptAction.y, 0, hMainWindow, NULL );
	TrackPopupMenu(hmenu, TPM_VERTICAL, pt.x, pt.y, 0, parentWindow, nullptr);
}

void WatchListView::Activate(LPNMLISTVIEW lpnm)
{
	LVITEM lvitem;

	memset(&lvitem, 0, sizeof(lvitem));
	lvitem.mask = LVIF_PARAM;
	lvitem.iItem = lpnm->iItem;
	ListView_GetItem(window, &lvitem);
}

void WatchListView::BeginDragging(int iItem)
{
	HIMAGELIST himl;
	LVITEM lvitem;
	POINT p;
	RECT rect_mainwindow, rect_listview;

	ZeroMemory(&rect_mainwindow, sizeof(rect_mainwindow));
	ZeroMemory(&rect_listview, sizeof(rect_listview));

	GetWindowRect(window, &rect_listview);
	GetWindowRect(parentWindow, &rect_mainwindow);

	memset(&lvitem, 0, sizeof(lvitem));
	lvitem.mask = LVIF_PARAM;
	lvitem.iItem = iItem;
	ListView_GetItem(window, &lvitem);
	g_draggedPVIObject = PviObject::FindByName((char *)lvitem.lParam);

	if (g_draggedPVIObject == nullptr)
		return;

	himl = ListView_CreateDragImage(window, iItem, &p);
	ImageList_BeginDrag(himl, 0, 0, 0);
	// start drag effect
	ImageList_DragEnter(parentWindow, rect_listview.left - rect_mainwindow.left,
						rect_listview.top - rect_mainwindow.top);
	SetCapture(parentWindow);

	if (g_draggedPVIObject != nullptr)
		g_draggedPVIObject->Watch(FALSE);
	ListView_DeleteItem(window, iItem);
	ListView_Update(window, iItem);
}
