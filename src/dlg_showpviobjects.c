#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include "main.h"
#include "pvi_interface.h"
#include "resource.h"


/* globale Variable */

static HWND hWndListView;

/* Prototypen */
static void InitListView(HWND hlistview);
static void FillListView(HWND hlistview);
LRESULT CALLBACK  ShowPviObjectsDlgProc(HWND, UINT, WPARAM, LPARAM);


void ShowPviObjects( void )
{
    HWND hWndMainWindow = GetMainWindow();
    RECT rect;

    InitCommonControls();
    GetWindowRect( hWndMainWindow, &rect );
    hWndListView = CreateWindow (WC_LISTVIEW, "",
                                 WS_POPUPWINDOW | WS_CAPTION | LVS_REPORT | LVS_EDITLABELS,
                                 rect.right -640, rect.bottom-480, 640, 480,
                                 hWndMainWindow, 0, (HINSTANCE)((LONG_PTR) GetWindowLongPtr(hWndMainWindow, GWLP_HINSTANCE)), NULL);
    ShowWindow( hWndListView, SW_SHOW  );
    InitListView(hWndListView);
    FillListView(hWndListView);
}


static void InitListView(HWND hlistview)
{
    LVCOLUMN lvcolumn;


    /* Spalte 0 */
    lvcolumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvcolumn.fmt = LVCFMT_LEFT;
    lvcolumn.cx = 250;
    lvcolumn.pszText = "Name";
    ListView_InsertColumn(hlistview, 0, &lvcolumn);
    /* Spalte 1 */
    lvcolumn.cx = 120;
    lvcolumn.pszText = "Typ";
    ListView_InsertColumn(hlistview, 1, &lvcolumn);
    /* Spalte 2 */
    lvcolumn.cx = 120;
    lvcolumn.pszText = "Descriptor";
    ListView_InsertColumn(hlistview, 2, &lvcolumn);
    /* Spalte 3 */
    lvcolumn.cx = 120;
    lvcolumn.pszText = "Status";
    ListView_InsertColumn(hlistview, 3, &lvcolumn);
    /* Spalte 4 */
    lvcolumn.cx = 200;
    lvcolumn.pszText = "Extended";
    ListView_InsertColumn(hlistview, 4, &lvcolumn);
    ListView_SetItemCount(hlistview, GetNumberOfPviObjects());

}



static void FillListView(HWND hlistview)
{
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

    for (i = 0; i < no_of_items; ++i)
    {
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
        switch (p->type)
        {
        case POBJ_PVI:
            ListView_SetItemText(hlistview, i, 1, KWOBJTYPE_PVI);
            break;
        case POBJ_LINE:
            ListView_SetItemText(hlistview, i, 1, KWOBJTYPE_LINE);
            break;
        case POBJ_DEVICE:
            ListView_SetItemText(hlistview, i, 1, KWOBJTYPE_DEVICE);
            break;
        case POBJ_STATION:
            ListView_SetItemText(hlistview, i, 1, KWOBJTYPE_STATION);
            break;
        case POBJ_CPU:
            ListView_SetItemText(hlistview, i, 1, KWOBJTYPE_CPU);
            break;
        case POBJ_MODULE:
            ListView_SetItemText(hlistview, i, 1, KWOBJTYPE_MODULE);
            break;
        case POBJ_TASK:
            ListView_SetItemText(hlistview, i, 1, KWOBJTYPE_TASK);
            break;
        case POBJ_PVAR:
            ListView_SetItemText(hlistview, i, 1, KWOBJTYPE_PVAR);
            break;
        default:
            ListView_SetItemText(hlistview, i, 1, "???");
        }
        //OemToChar(p->descriptor, tempstring );
        ListView_SetItemText(hlistview, i, 2, p->descriptor);
        if( p->linkid != 0 )
        {
            if (p->error == 0)
            {
                ListView_SetItemText(hlistview, i, 3, "OK");
            }
            else
            {
                sprintf(tempstring, "Err: %5u", p->error);
                ListView_SetItemText(hlistview, i, 3, tempstring);
            }
        }
        else
        {
            ListView_SetItemText(hlistview, i, 3, "not linked" );
        }
        if( p->type == POBJ_PVAR )
        {
            if( p->ex.pv.dimension > 1 )
                sprintf( tempstring, "%s[%u]", p->ex.pv.pdatatype, p->ex.pv.dimension );
            else
                sprintf( tempstring, "%s", p->ex.pv.pdatatype );
            ListView_SetItemText(hlistview, i, 4, tempstring);
        }

    }
}




