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
#include "main.h"
#include "pvi_interface.h"
#include "resource.h"

static HWND hWndListView;
/* prototypes */
static void ShowPviObjectsInitListView(HWND hlistview);
static void ShowPviObjectsFillListView(HWND hlistview);
INT_PTR CALLBACK ShowPviObjectsDlgProc(HWND, UINT, WPARAM, LPARAM);

void ShowWindowPviObjects(void)
{
	HWND hWndMainWindow = MainWindowGetHandle();
	RECT rect;

	InitCommonControls();
	GetWindowRect(hWndMainWindow, &rect);
	hWndListView = CreateWindowA(WC_LISTVIEWA, "",
								 WS_POPUPWINDOW | WS_CAPTION | LVS_REPORT | LVS_EDITLABELS,
								 rect.right - 640, rect.bottom - 480, 640, 480,
								 hWndMainWindow, 0, (HINSTANCE)((LONG_PTR)GetWindowLongPtr(hWndMainWindow, GWLP_HINSTANCE)), nullptr);
	ShowWindow(hWndListView, SW_SHOW);
	ShowPviObjectsInitListView(hWndListView);
	ShowPviObjectsFillListView(hWndListView);
}

static void ShowPviObjectsInitListView(HWND hlistview)
{
	LVCOLUMNA lvcolumn;
	char columnName[32];

	/* column 0 */
	lvcolumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvcolumn.fmt = LVCFMT_LEFT;
	lvcolumn.cx = 250;
	lvcolumn.pszText = columnName;
	strcpy(columnName, "Name");
	SendMessageA(hlistview, LVM_INSERTCOLUMNA, 0, reinterpret_cast<LPARAM>(&lvcolumn));
	/* column 1 */
	lvcolumn.cx = 120;
	strcpy(columnName, "Typ");
	SendMessageA(hlistview, LVM_INSERTCOLUMNA, 1, reinterpret_cast<LPARAM>(&lvcolumn));
	/* column 2 */
	lvcolumn.cx = 120;
	strcpy(columnName, "Descriptor");
	SendMessageA(hlistview, LVM_INSERTCOLUMNA, 2, reinterpret_cast<LPARAM>(&lvcolumn));
	/* column 3 */
	lvcolumn.cx = 120;
	strcpy(columnName, "Status");
	SendMessageA(hlistview, LVM_INSERTCOLUMNA, 3, reinterpret_cast<LPARAM>(&lvcolumn));
	/* column 4 */
	lvcolumn.cx = 200;
	strcpy(columnName, "Extended");
	SendMessageA(hlistview, LVM_INSERTCOLUMNA, 4, reinterpret_cast<LPARAM>(&lvcolumn));
	ListView_SetItemCount(hlistview, PviObject::Count());
}

static void ShowPviObjectsFillListView(HWND hlistview)
{
	LVITEMA lvitem;
	int i;
	PviObject *p;
	char tempstring[256];
	int no_of_items;

	ListView_DeleteAllItems(hlistview);

	p = PviObject::GetNext(1);
	no_of_items = PviObject::Count();

	sprintf(tempstring, "List of Pvi-Objects - count = %u", no_of_items);
	SetWindowTextA(hlistview, tempstring);

	for (i = 0; i < no_of_items; ++i)
	{
		if ((p = PviObject::GetNext(0)) == nullptr)
			break; // Ende der Liste
		memset(&lvitem, 0, sizeof(lvitem));
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = i;
		lvitem.iSubItem = 0;
		lvitem.state = LVIS_FOCUSED;
		lvitem.stateMask = (UINT)-1;
		lvitem.pszText = p->name;
		lvitem.cchTextMax = 0;
		SendMessageA(hlistview, LVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&lvitem));
		const char *typeName;
		switch (p->type)
		{
		case POBJ_PVI:
			typeName = KWOBJTYPE_PVI;
			break;
		case POBJ_LINE:
			typeName = KWOBJTYPE_LINE;
			break;
		case POBJ_DEVICE:
			typeName = KWOBJTYPE_DEVICE;
			break;
		case POBJ_STATION:
			typeName = KWOBJTYPE_STATION;
			break;
		case POBJ_CPU:
			typeName = KWOBJTYPE_CPU;
			break;
		case POBJ_MODULE:
			typeName = KWOBJTYPE_MODULE;
			break;
		case POBJ_TASK:
			typeName = KWOBJTYPE_TASK;
			break;
		case POBJ_PVAR:
			typeName = KWOBJTYPE_PVAR;
			break;
		default:
			typeName = "???";
		}
		snprintf(tempstring, sizeof(tempstring), "%s", typeName);
		lvitem.iSubItem = 1;
		lvitem.pszText = tempstring;
		SendMessageA(hlistview, LVM_SETITEMTEXTA, i, reinterpret_cast<LPARAM>(&lvitem));
		// OemToChar(p->descriptor, tempstring );
		lvitem.iSubItem = 2;
		lvitem.pszText = p->descriptor;
		SendMessageA(hlistview, LVM_SETITEMTEXTA, i, reinterpret_cast<LPARAM>(&lvitem));
		if (p->linkid != 0)
		{
			if (p->error == 0)
			{
				strcpy(tempstring, "OK");
			}
			else
			{
				sprintf(tempstring, "Err: %5u", p->error);
			}
		}
		else
		{
			strcpy(tempstring, "not linked");
		}
		lvitem.iSubItem = 3;
		lvitem.pszText = tempstring;
		SendMessageA(hlistview, LVM_SETITEMTEXTA, i, reinterpret_cast<LPARAM>(&lvitem));
		if (p->type == POBJ_PVAR)
		{
			if (p->ex.pv.dimension > 1)
				sprintf(tempstring, "%s[%u]", p->ex.pv.pdatatype, p->ex.pv.dimension);
			else
				sprintf(tempstring, "%s", p->ex.pv.pdatatype);
			lvitem.iSubItem = 4;
			lvitem.pszText = tempstring;
			SendMessageA(hlistview, LVM_SETITEMTEXTA, i, reinterpret_cast<LPARAM>(&lvitem));
		}
	}
}
