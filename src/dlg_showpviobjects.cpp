#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include "main.h"
#include "pvi_interface.h"
#include "resource.h"


static HWND hWndListView;
/* prototypes */
static void ShowPviObjectsInitListView(HWND hlistview);
static void ShowPviObjectsFillListView(HWND hlistview);
INT_PTR CALLBACK ShowPviObjectsDlgProc(HWND, UINT, WPARAM, LPARAM);


void ShowWindowPviObjects( void ) {
	HWND hWndMainWindow = MainWindowGetHandle();
	RECT rect;

	InitCommonControls();
	GetWindowRect( hWndMainWindow, &rect );
	hWndListView = CreateWindow (WC_LISTVIEW, "",
	                             WS_POPUPWINDOW | WS_CAPTION | LVS_REPORT | LVS_EDITLABELS,
	                             rect.right -640, rect.bottom-480, 640, 480,
	                             hWndMainWindow, 0, (HINSTANCE)((LONG_PTR) GetWindowLongPtr(hWndMainWindow, GWLP_HINSTANCE)), NULL);
	ShowWindow( hWndListView, SW_SHOW  );
	ShowPviObjectsInitListView(hWndListView);
	ShowPviObjectsFillListView(hWndListView);
}


static void ShowPviObjectsInitListView(HWND hlistview) {
	LVCOLUMN lvcolumn;
	char columnName[32];


	/* column 0 */
	lvcolumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvcolumn.fmt = LVCFMT_LEFT;
	lvcolumn.cx = 250;
	lvcolumn.pszText = columnName;
	strcpy(columnName, "Name");
	ListView_InsertColumn(hlistview, 0, &lvcolumn);
	/* column 1 */
	lvcolumn.cx = 120;
	strcpy(columnName, "Typ");
	ListView_InsertColumn(hlistview, 1, &lvcolumn);
	/* column 2 */
	lvcolumn.cx = 120;
	strcpy(columnName, "Descriptor");
	ListView_InsertColumn(hlistview, 2, &lvcolumn);
	/* column 3 */
	lvcolumn.cx = 120;
	strcpy(columnName, "Status");
	ListView_InsertColumn(hlistview, 3, &lvcolumn);
	/* column 4 */
	lvcolumn.cx = 200;
	strcpy(columnName, "Extended");
	ListView_InsertColumn(hlistview, 4, &lvcolumn);
	ListView_SetItemCount(hlistview, GetNumberOfPviObjects());

}



static void ShowPviObjectsFillListView(HWND hlistview) {
	LVITEM lvitem;
	int i;
	PVIOBJECT *p;
	char tempstring[256];
	int no_of_items;

	ListView_DeleteAllItems(hlistview);

	p = GetNextPviObject(1);
	no_of_items = GetNumberOfPviObjects();

	sprintf( tempstring, "List of Pvi-Objects - count = %u", no_of_items );
	SetWindowText( hlistview, tempstring );

	for (i = 0; i < no_of_items; ++i) {
		if ((p = GetNextPviObject(0)) == NULL)
			break;				// Ende der Liste
		memset(&lvitem, 0, sizeof(lvitem));
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = i;
		lvitem.iSubItem = 0;
		lvitem.state = LVIS_FOCUSED;
		lvitem.stateMask = (UINT) - 1;
		lvitem.pszText = p->name;
		lvitem.cchTextMax = 0;
		ListView_InsertItem(hlistview, &lvitem);
		const char *typeName;
		switch (p->type) {
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
		ListView_SetItemText(hlistview, i, 1, tempstring);
		//OemToChar(p->descriptor, tempstring );
		ListView_SetItemText(hlistview, i, 2, p->descriptor);
		if( p->linkid != 0 ) {
			if (p->error == 0) {
				strcpy(tempstring, "OK");
			} else {
				sprintf(tempstring, "Err: %5u", p->error);
			}
		} else {
			strcpy(tempstring, "not linked");
		}
		ListView_SetItemText(hlistview, i, 3, tempstring);
		if( p->type == POBJ_PVAR ) {
			if( p->ex.pv.dimension > 1 )
				sprintf( tempstring, "%s[%u]", p->ex.pv.pdatatype, p->ex.pv.dimension );
			else
				sprintf( tempstring, "%s", p->ex.pv.pdatatype );
			ListView_SetItemText(hlistview, i, 4, tempstring);
		}

	}
}




