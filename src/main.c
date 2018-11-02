
#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include "main.h"
#include "mytreeview.h"
#include "mylistview.h"
#include "pvi_interface.h"
#include "stringtools.h"
#include "logger.h"
#include "zip.h"
#include "settings.h"
#include <wingdi.h>

/** Prototypes **************************************************************/

static LRESULT WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
//static void Main_OnDestroy(HWND);
static void PviObjectsValid(void);
static void PviObjectErrorChanged( PVIOBJECT *object, int errcode );
//static void PviObjectDataChanged(PVIOBJECT *object, char* value );
//static void PviObjectStatusChanged( PVIOBJECT *object, char* status );
static void TreeViewItemSelected( HTREEITEM hti, LPARAM lParam );
static void TreeViewBeginDragging( HTREEITEM hti, LPARAM lParam );

static void TreeViewDblClick( HTREEITEM hti, LPARAM lParam );
static void TreeViewRClick( HTREEITEM hti, LPARAM lParam );
static void InsertInWatchList( PVIOBJECT *object);

static void ListViewKeydown( WORD vkey );
static void ListViewDblClick( LPNMLISTVIEW lpnm );
static void ListViewRClick( LPNMLISTVIEW lpnm );
static void ListViewActivate( LPNMLISTVIEW lpnm );
static void ListViewBeginDragging( int iItem );
static void ListViewUpdateValue( PVIOBJECT *object );

static void LoadWatchFile( char *filename );
static void SaveWatchFile( char *filename );

extern LRESULT CALLBACK  WritePvarDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

/* externe Funktionen */
extern LRESULT WINAPI AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT WINAPI SelectDeviceDlgProc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT WINAPI ShowPviObjectsDlgProc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT WINAPI SettingsDlgProc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT WINAPI ConfigAxTraceDlg(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern LRESULT WINAPI EthAdaptersDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern void ShowPviObjects( void );


/** Global variables ********************************************************/


static char application_path[MAX_PATH];


static HINSTANCE ghInstance;
static HWND hMainWindow;

static PVIOBJECT *dragged_object;
static PVIOBJECT *selected_object;
static HTREEITEM hselected_treeitem;

static int	drop_target_index;
static int instance_counter;

static NOTIFYICONDATA notifyicondata;
static MYTREEVIEWPARAM mytreeviewparam;
static MYLISTVIEWPARAM mylistviewparam;


static  WORD images[] =
{
    IDR_ICO_DEVICE,
    IDR_ICO_CPU,
    IDR_ICO_GLOBAL,
    IDR_ICO_TASK,
    IDR_ICO_VARIABLE,
    IDR_ICO_STRUCT,
    IDR_ICO_ARRAY,
    IDR_ICO_VARIABLE_U32,
    IDR_ICO_VARIABLE_I32,
    IDR_ICO_VARIABLE_U16,
    IDR_ICO_VARIABLE_I16,
    IDR_ICO_VARIABLE_U8,
    IDR_ICO_VARIABLE_I8,
    IDR_ICO_VARIABLE_BOOL,
    IDR_ICO_VARIABLE_REAL,
    IDR_ICO_VARIABLE_STRING,
    IDR_ICO_VARIABLE_TIME,
    IDR_ICO_VARIABLE_DATE_AND_TIME,
    0
};


/* ============================================================================================ */

static HIMAGELIST CreateImageList(void)
{
    HIMAGELIST himl;  // handle to image list
    HICON hicon;
    int i;

    // Imagelist erzeugen
    if ((himl = ImageList_Create( 16, 16, FALSE, sizeof(images)/sizeof(char*), 0)) == NULL)
        return NULL;

    // Images hinzufügen
    i = 0;
    while( images[i] != 0 )
    {
        hicon = LoadImage( ghInstance, MAKEINTRESOURCE(images[i]), IMAGE_ICON, 0,0, LR_DEFAULTCOLOR  | LR_LOADTRANSPARENT | LR_VGACOLOR  );
        ImageList_AddIcon( himl, hicon );
        DestroyIcon( hicon );
        ++i;
    }

    return(himl);

}

static int GetImageIndex( WORD resource )
{
    int i = 0;

    while( images[i] != 0 )
    {
        if( images[i] == resource )
            return i;
        ++i;
    }
    return 0;
}


int GetPviObjectImage( PVIOBJECT *object )
{
    int imageindex=0;

    if( object == NULL )
        return 0;

    switch( object->type )
    {
    case POBJ_PVAR:
        if( object->ex.pv.dimension > 1 )
        {
            imageindex = GetImageIndex(IDR_ICO_ARRAY);
            break;
        }

        switch( object->ex.pv.type )
        {
        case BR_STRUCT:
            imageindex = GetImageIndex(IDR_ICO_STRUCT);
            break;
        case BR_UDINT:
            imageindex = GetImageIndex(IDR_ICO_VARIABLE_U32);
            break;
        case BR_DINT:
            imageindex = GetImageIndex(IDR_ICO_VARIABLE_I32);
            break;
        case BR_UINT:
            imageindex = GetImageIndex(IDR_ICO_VARIABLE_U16);
            break;
        case BR_INT:
            imageindex = GetImageIndex(IDR_ICO_VARIABLE_I16);
            break;
        case BR_USINT:
            imageindex = GetImageIndex(IDR_ICO_VARIABLE_U8);
            break;
        case BR_SINT:
            imageindex = GetImageIndex(IDR_ICO_VARIABLE_I8);
            break;
        case BR_BOOL:
            imageindex = GetImageIndex(IDR_ICO_VARIABLE_BOOL);
            break;
        case BR_REAL:
        case BR_LREAL:
            imageindex = GetImageIndex(IDR_ICO_VARIABLE_REAL);
            break;
        case BR_STRING:
            imageindex = GetImageIndex(IDR_ICO_VARIABLE_STRING);
            break;
        case BR_TIME:
            imageindex = GetImageIndex(IDR_ICO_VARIABLE_TIME);
            break;
        case BR_DATI:
            imageindex = GetImageIndex(IDR_ICO_VARIABLE_DATE_AND_TIME);
            break;
        default:
            imageindex = GetImageIndex(IDR_ICO_VARIABLE);
            break;
        }
        break;

    case POBJ_CPU:
        imageindex = GetImageIndex(IDR_ICO_CPU);
        break;

    case POBJ_DEVICE:
        imageindex = GetImageIndex(IDR_ICO_PVI);
        break;

    case POBJ_TASK:
        imageindex = GetImageIndex(IDR_ICO_TASK);
        break;

    default:
        break;
    }
    return imageindex;
}





char *GetApplicationPath(void)
{
    return application_path;
}

HWND GetMainWindow( void )
{
    return( hMainWindow );
}
/* ============================================================================================== */


BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
    char windowname[256];

    GetWindowText( hwnd, windowname, sizeof(windowname) );
    if( !strncmp( windowname, APPLICATION_NAME, lstrlen(APPLICATION_NAME) ) )
        instance_counter++;

    return TRUE;
}



/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: WinMain
 Created  : Mon Feb 20 15:31:18 2006
 Modified : Mon Feb 20 15:31:18 2006

 Synopsys : startet Hauptprogramm
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc;
    MSG msg;
    char windowname[20];

    // Pfad der Applikation speichern
    if (GetCurrentDirectory(sizeof(application_path), application_path))
    {
    }
    else
    {
        MessageBox( NULL, "GetCurrentDirectory", "Error", MB_OK );
        return -1;
    }

    InitializeSettings(); // ggf. Default- Ini kreieren




    // Instanzen zählen
    instance_counter = 1;
    EnumWindows( EnumWindowsProc, 0 );

    //StringCbPrintf( windowname, sizeof(windowname),  APPLICATION_NAME " No.%u", instance_counter );
    sprintf( windowname, APPLICATION_NAME " No.%u", instance_counter );


    //PVICOM_DllMain( hInstance , DLL_PROCESS_ATTACH, 0);
    InitCommonControls();

    //PlotInit(hInstance); // Initialisieren der Plot- Funktion

    ghInstance = hInstance;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_ICO_MAIN));
    //wc.hCursor    = LoadCursor(NULL, IDC_ARROW);
    wc.hCursor = LoadCursor(NULL, IDC_SIZEWE);	//for splitter
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MNU_MAIN);
    wc.lpszClassName = _T("brwatchClass");

    if (!RegisterClass(&wc))
        return 0;

    hMainWindow = CreateWindow(_T("brwatchClass"), windowname, WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    if (!hMainWindow)
        return 0;
    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);


    // Notify
    memset( &notifyicondata, 0, sizeof(notifyicondata) );
    notifyicondata.cbSize = sizeof(notifyicondata);
    notifyicondata.hWnd = hMainWindow;
    strcpy( notifyicondata.szTip, windowname );
    strcat( notifyicondata.szTip, " - Click here to size up" );
    notifyicondata.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_ICO_MAIN));
    notifyicondata.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    notifyicondata.uCallbackMessage = NOTIFY_ICON_MESSAGE;



    // Callback- Funktionen für PVI- Interface setzen
    pvi_interface_notify.cbobjects_valid = PviObjectsValid;
    pvi_interface_notify.cberror_changed = PviObjectErrorChanged;
    pvi_interface_notify.cbdata_changed = ListViewUpdateValue;



    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    StopPvi();
    //PVICOM_DllMain( hInstance , DLL_PROCESS_DETACH, 0);
    return (int) msg.wParam;
}



/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: PviObjectsValid
 Created  : Mon Feb 20 15:32:24 2006
 Modified : Mon Feb 20 15:32:50 2006

 Synopsys : wird aufgerufen, wenn Verbindung zum PVI- Manager besteht und
            die Geräteobjekte eingerichtet wurden

 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void PviObjectsValid(void)
{
    PVIOBJECT *object;
    char logger_watch_file[MAX_PATH];

    object = GetNextPviObject(TRUE);
    while (object->next != NULL)
    {
        object = object->next;
        if (object->type == POBJ_DEVICE)
        {
            AddItemToMyTreeView(&mytreeviewparam, TVI_ROOT, object->descriptor, (LPARAM)object->name, 0);
        }
    }

    // war der Logger vor dem letzten Beenden aktiv, so wird er automatisch gestartet
    if( GetPrivateProfileInt( "General", "LoggerActive", 0, GetIniFile() ) )  // war Logger aktiv ?
    {
        strcpy( logger_watch_file, GetApplicationPath() );
        strcat( logger_watch_file, "\\_logger.wtc" );
        LoadWatchFile( logger_watch_file );
        Logger();
        SendMessage( GetMainWindow(), NOTIFY_LOGGER_STATUS, 0, 0 );
        Shell_NotifyIcon(NIM_ADD, &notifyicondata );
        ShowWindow( hMainWindow, SW_HIDE );
    }
}

/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: PviObjectChangedErrorState
 Created  : Tue Feb 21 14:21:01 2006
 Modified : Tue Feb 21 14:21:01 2006

 Synopsys : wird aufgerufen, wenn sich der Fehlerstatus des Objektes geä
            ndert hat
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void PviObjectErrorChanged( PVIOBJECT *object, int errcode )
{
    LVFINDINFO lvfindinfo;
    int index;
    char errtext[80];

    memset( &lvfindinfo, 0, sizeof(lvfindinfo) );

    lvfindinfo.flags = LVFI_PARAM;
    lvfindinfo.lParam = (LPARAM) object->name;

    index = ListView_FindItem( mylistviewparam.hwndLV, -1, &lvfindinfo );
    if( index >= 0 )
    {
        switch( errcode )
        {
        case 0:
            strcpy( errtext, "" );
            break;

        case 4177: // dynamische Variable nicht belegt
            strcpy( errtext, "(NULL)" );
            break;

        case 4808: // keine Verbindung zur SPS
            strcpy( errtext, "(OFFLINE)" );
            break;

        case 4813: // Identifizierungsfehler
            strcpy( errtext, "(Object not found)");
            break;

        default:
            sprintf( errtext, "Error:%u", errcode );
            break;
        }
        ListView_SetItemText( mylistviewparam.hwndLV, index, 3, errtext );
    }




}




/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: TreeViewInsertChildObjects
 Created  : Thu Mar  9 11:53:17 2006
 Modified : Thu Mar  9 11:53:17 2006

 Synopsys : holt die Kinder des Objektes aus der Liste und trägt sie im
            TreeView ein
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void TreeViewInsertChildObjects( HTREEITEM hti, PVIOBJECT *parent )
{
    PVIOBJECT *child;
    int imageindex;
    TVITEM tvitem;

    if( parent != NULL )
    {

        ExpandPviObject(parent);

        // Nach dem Expandieren stehen evtl erweiterte Informationen zum Elternobject zur Verfügung
        if( parent->type == POBJ_CPU )
        {
            char tempstring[256];
            sprintf( tempstring, "%s %s %s", parent->ex.cpu.cputype, parent->ex.cpu.arversion, parent->descriptor );
            memset( &tvitem, 0, sizeof(tvitem) );
            tvitem.hItem = hti;
            tvitem.mask = TVIF_TEXT;
            tvitem.pszText = tempstring;
            TreeView_SetItem( mytreeviewparam.hwndTV, &tvitem );
        }
        else if( parent->type == POBJ_PVAR )
        {
            char tempstring[256];
            if( parent->ex.pv.type == BR_STRUCT )
            {
                if( parent->ex.pv.dimension > 1 )   // Struktur- Arrays
                {
                    sprintf( tempstring, "%s   :%s[%u]",parent->descriptor, parent->ex.pv.pdatatype, parent->ex.pv.dimension );
                }
                else
                {
                    sprintf( tempstring, "%s   :%s",parent->descriptor, parent->ex.pv.pdatatype );
                }
                memset( &tvitem, 0, sizeof(tvitem) );
                tvitem.hItem = hti;
                tvitem.mask = TVIF_TEXT;
                tvitem.pszText = tempstring;
                TreeView_SetItem( mytreeviewparam.hwndTV, &tvitem );
            }
            else
            {
                if( parent->ex.pv.dimension > 1 )   // Arrays von Basistypen
                {
                    sprintf( tempstring, "%s   :%s[%u]",parent->descriptor, parent->ex.pv.pdatatype, parent->ex.pv.dimension );
                    memset( &tvitem, 0, sizeof(tvitem) );
                    tvitem.hItem = hti;
                    tvitem.mask = TVIF_TEXT;
                    tvitem.pszText = tempstring;
                    TreeView_SetItem( mytreeviewparam.hwndTV, &tvitem );
                }
            }
        }

        child = FindPviChildObject( parent, TRUE );
        while( child != NULL )
        {
            char tempstring[256];

            switch( parent->type )
            {
            case POBJ_PVAR:
                if( parent->ex.pv.type == BR_STRUCT && parent->ex.pv.dimension == 1)
                {
                    strcpy( tempstring, child->descriptor + strlen(parent->descriptor) + 1);
                }
                else
                {
                    strcpy( tempstring, child->descriptor );
                }
                break;

            default:
                strcpy( tempstring, child->descriptor );
                break;

            }

            imageindex = GetPviObjectImage( child );

            AddItemToMyTreeView( &mytreeviewparam, hti, tempstring, (LPARAM) child->name, imageindex );
            child = FindPviChildObject( parent, FALSE );
        } /* End while()*/
    }

}

/* alle Kinder löschen */
static void TreeViewDeleteAllChilds( HTREEITEM hti )
{
    HTREEITEM hChild;
    HTREEITEM hSibling;

    hChild = TreeView_GetChild( mytreeviewparam.hwndTV, hti );
    while( hChild != NULL )
    {
        hSibling = TreeView_GetNextSibling( mytreeviewparam.hwndTV, hChild );
        TreeView_DeleteItem( mytreeviewparam.hwndTV, hChild );
        hChild = hSibling;
    }
}

/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: TreeViewItemSelected
 Created  : Mon Feb 20 15:35:09 2006
 Modified : Mon Feb 20 15:35:09 2006

 Synopsys : ein Element im TreeView wurde angeklickt
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/


static void TreeViewItemSelected( HTREEITEM hti, LPARAM lParam )
{
    PVIOBJECT *parent;  // das angeklickte Objekte ist parent
    HCURSOR oldcursor;

    if( TreeView_GetChild( mytreeviewparam.hwndTV, hti ) == NULL )  // noch nix eingelesen
    {
        parent = FindPviObjectByName( (char*) lParam );
        if( parent != NULL )
        {
            if( parent->error == 0 )
            {
                oldcursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
                TreeViewInsertChildObjects( hti, parent );
                SetCursor(oldcursor);
            }
        }
    }
}
/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: TreeViewBeginDragging
 Created  : Mon Feb 20 15:35:40 2006
 Modified : Mon Feb 20 15:35:40 2006

 Synopsys : noch nicht :-(
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void TreeViewBeginDragging( HTREEITEM hti, LPARAM lParam )
{
    HIMAGELIST himl;

    TreeView_Select( mytreeviewparam.hwndTV, hti, TVGN_DROPHILITE ); // nur wg. Optik

    himl = TreeView_CreateDragImage( mytreeviewparam.hwndTV, hti);
    ImageList_BeginDrag(himl, 0, 0, 0);
    // start drag effect
    ImageList_DragEnter(hMainWindow,0,0);
    SetCapture(hMainWindow);

    dragged_object = FindPviObjectByName( (char*) lParam);
}



/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: TreeViewDblClick
 Created  : Mon Feb 20 15:36:29 2006
 Modified : Mon Feb 20 15:36:29 2006

 Synopsys : ein Element im TreeView wurde doppelt geklickt

 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/
static void TreeViewDblClick(HTREEITEM hti, LPARAM lParam)
{
    PVIOBJECT *object;
    object = FindPviObjectByName( (char*) lParam );
    if( object != NULL )
    {
        object->watchsort = ListView_GetItemCount( mylistviewparam.hwndLV );
        InsertInWatchList( object);
    }
}


/* TreeView, rechte Maustaste */
static void TreeViewRClick(HTREEITEM hti, LPARAM lParam)
{
    PVIOBJECT *object;
    RECT rect_window;
    RECT rect_item;
    LPRECT lprect_item = &rect_item;
    HCURSOR hcursor;
    unsigned long broadcast;

    if( GetWindowRect( mytreeviewparam.hwndTV, &rect_window ) )
    {
        if( TreeView_GetItemRect( mytreeviewparam.hwndTV, hti, lprect_item, TRUE ) )
        {

            object = FindPviObjectByName( (char*) lParam );
            if( object != NULL )
            {
                switch( object->type )
                {
                case POBJ_DEVICE:
                    if( strstr( strupr(object->descriptor), "TCPIP" ) )
                    {
                        selected_object = object;
                        hselected_treeitem = hti;

                        DialogBoxParam( ghInstance, MAKEINTRESOURCE(DLG_ETH_ADAPTERS), GetMainWindow(), (DLGPROC) EthAdaptersDlgProc, (LPARAM) &broadcast );
                        if( broadcast == 0 )
                            break;
                        else
                            object->ex.dev.broadcast = broadcast;

                        hcursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
                        selected_object->ex.dev.allow_icmp = 1;
                        TreeViewDeleteAllChilds( hselected_treeitem );
                        UpdateWindow( mytreeviewparam.hwndTV );
                        TreeViewInsertChildObjects( hselected_treeitem, selected_object );
                        TreeView_Expand( mytreeviewparam.hwndTV, hselected_treeitem, TVE_EXPAND );
                        SetCursor(hcursor);
                        //hPopup = CreatePopupMenu();
                        //AppendMenu( hPopup, MF_ENABLED | MF_STRING, IDM_SEND_ICMP, "Search CPU via ICMP" );
                        //TrackPopupMenu( hPopup, TPM_VERTICAL, rect_window.left + rect_item.left ,
                        //		rect_window.top +rect_item.bottom,
                        //		0, hMainWindow, NULL );
                        //}
                        //DestroyMenu(hPopup);
                    }

                    break;

                case POBJ_PVAR:
                    switch( object->ex.pv.type )
                    {
                    case BR_STRUCT:
                        TreeViewItemSelected( hti, lParam );
                        if( object->ex.pv.pdatatype != NULL )
                        {
                            // Einsprungpunkt für W&H - Tracebaustein...
                            if( !strcmp( object->ex.pv.pdatatype, "wh_acptrace") )
                            {
                                //MessageBox( GetMainWindow(), "hier", "hier", MB_OK );
                                DialogBoxParam( ghInstance, MAKEINTRESOURCE(DLG_AXCONFIG), GetMainWindow(), (DLGPROC) ConfigAxTraceDlg, (LPARAM) object );
                            }
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


/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: InsertInWatchList
 Created  : Thu Mar  9 10:17:07 2006
 Modified : Thu Mar  9 10:17:07 2006

 Synopsys : nimmt ein Objekt in das ListView auf
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void InsertInWatchList( PVIOBJECT *object )
{

    LVITEM lvitem;
    char *text;
    BOOL found;
    int i;

    if( object != NULL )
    {

        // nicht eintragen, wenn es schon existiert
        found = FALSE;
        i= ListView_GetNextItem( mylistviewparam.hwndLV, -1, LVNI_ALL );
        while( i!= -1 )
        {
            memset( &lvitem, 0, sizeof(lvitem) );
            lvitem.mask = LVIF_PARAM;
            lvitem.iItem = i;
            ListView_GetItem( mylistviewparam.hwndLV, &lvitem );
            if( lvitem.lParam != 0 )
            {
                if( strcmp( object->name, (char*) lvitem.lParam ) == 0 )
                {
                    found = TRUE;
                }
            }
            i = ListView_GetNextItem( mylistviewparam.hwndLV, i, LVNI_ALL );
        }

        if( found == TRUE )
            return;

        if( object->watchsort < 0 )   // Object wird nicht gelistet
        {
            return;
        }

        if( object->type == POBJ_PVAR)
        {
            if( object->ex.pv.type == BR_STRUCT || object->ex.pv.dimension > 1 )
                return;  // keine Strukturen oder Arrays...
            if( WatchPviObject( object, TRUE ) != NULL )
            {
                char tempstring[256];

                if( object->ex.pv.task != NULL )
                {
                    PVIOBJECT *task = object->ex.pv.task;

                    strcpy( tempstring, task->descriptor );
                    strcat( tempstring, ":" );
                }
                else
                {
                    strcpy( tempstring, "" ); // sollte eigentlich nie der Fall sein !
                }
                strcat( tempstring, object->descriptor );

                memset(&lvitem, 0, sizeof(lvitem));
                lvitem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
                lvitem.pszText = tempstring;
                lvitem.lParam = (LPARAM) object->name;
                lvitem.iImage = GetImageIndex(IDR_ICO_VARIABLE);
                lvitem.iItem = object->watchsort;

                /* Name */
                ListView_InsertItem(mylistviewparam.hwndLV, &lvitem);
                ListView_SetColumnWidth( mylistviewparam.hwndLV, 0, LVSCW_AUTOSIZE );


                /* Datentyp */
                if( object->ex.pv.type == BR_STRING )
                {
                    sprintf( tempstring, "%1s%s(%lu)", object->ex.pv.scope[0] == 'd' ? "*" : "", object->ex.pv.pdatatype, object->ex.pv.length-1 );
                }
                else
                {
                    sprintf( tempstring, "%1s%s", object->ex.pv.scope[0] == 'd' ? "*" : "", object->ex.pv.pdatatype );
                }
                text = tempstring;
                ListView_SetItemText(mylistviewparam.hwndLV, lvitem.iItem, 1, text ); // Datentyp
                memset(&lvitem, 0, sizeof(lvitem));
                lvitem.mask = LVIF_IMAGE;
                lvitem.iImage = GetPviObjectImage(object);
                lvitem.iSubItem = 1;
                lvitem.iItem = object->watchsort;
                ListView_SetItem( mylistviewparam.hwndLV, &lvitem ); // Icon setzen
                ListView_SetColumnWidth( mylistviewparam.hwndLV, 1, LVSCW_AUTOSIZE );

                /* Gültigkeitsbereich */
                switch( object->ex.pv.scope[0] )
                {
                case 'g':
                    text = "<GLOBAL>";
                    break;
                case 'l':
                    text = "<LOCAL >";
                    break;
                case 'd':
                    text = "<DYNAM.>";
                    break;
                }
                ListView_SetItemText(mylistviewparam.hwndLV, lvitem.iItem, 2, text ); // Scope
                ListView_SetColumnWidth( mylistviewparam.hwndLV, 2, LVSCW_AUTOSIZE );

                /* Wert */
                text = " ";
                ListView_SetItemText(mylistviewparam.hwndLV, lvitem.iItem, 3, text );	// Wert
                //
                ListView_EnsureVisible( mylistviewparam.hwndLV, lvitem.iItem, TRUE );
            }

        }
        else if( object->type == POBJ_CPU )
        {
            char tempstring[256];

            sprintf( tempstring, "%s %s %s", object->ex.cpu.cputype, object->ex.cpu.arversion, object->descriptor );
            memset(&lvitem, 0, sizeof(lvitem));
            lvitem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
            lvitem.pszText = tempstring;
            lvitem.lParam = (LPARAM) object->name;
            lvitem.iImage = GetPviObjectImage(object);
            lvitem.iItem = object->watchsort;

            /* Name */
            ListView_InsertItem(mylistviewparam.hwndLV, &lvitem);
            ListView_SetColumnWidth( mylistviewparam.hwndLV, 0, LVSCW_AUTOSIZE );

            /* Type */
            ListView_SetItemText(mylistviewparam.hwndLV, lvitem.iItem, 1, "CPU " );
            ListView_SetColumnWidth( mylistviewparam.hwndLV, 1, LVSCW_AUTOSIZE );

            /* Wert */
            switch( object->error )
            {
            case 0:
                strcpy( tempstring, "" );
                break;

            case 4808: // keine Verbindung zur SPS
                sprintf( tempstring, "(OFFLINE)" );
                break;

            default:
                sprintf( tempstring, "Err:%i", object->error );
                break;
            }
            ListView_SetItemText(mylistviewparam.hwndLV, lvitem.iItem, 3, tempstring );	// Wert
            ListView_SetColumnWidth( mylistviewparam.hwndLV, 3, LVSCW_AUTOSIZE );

        }
        else if( object->type == POBJ_TASK )
        {
            char tempstring[256];

            strcpy( tempstring, object->descriptor );

            memset(&lvitem, 0, sizeof(lvitem));
            lvitem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
            lvitem.pszText = tempstring;
            lvitem.lParam = (LPARAM) object->name;
            lvitem.iImage = GetPviObjectImage(object);
            lvitem.iItem = object->watchsort;

            /* Name */
            ListView_InsertItem(mylistviewparam.hwndLV, &lvitem);
            ListView_SetColumnWidth( mylistviewparam.hwndLV, 0, LVSCW_AUTOSIZE );

            /* Type */
            ListView_SetItemText(mylistviewparam.hwndLV, lvitem.iItem, 1, "TASK" );
            ListView_SetColumnWidth( mylistviewparam.hwndLV, 1, LVSCW_AUTOSIZE );

            /* Scope */
            // strcpy( tempstring, ((PVIOBJECT*) object->ex.task.cpu)->descriptor );
            // ListView_SetItemText(mylistviewparam.hwndLV, lvitem.iItem, 2, tempstring );
            // ListView_SetColumnWidth( mylistviewparam.hwndLV, 1, LVSCW_AUTOSIZE);
        }


        // Indes des Elementes in der Liste in den Objektinformationen speichern
        i = ListView_GetNextItem( mylistviewparam.hwndLV, -1, LVNI_ALL );
        while( i != -1 )
        {
            memset( &lvitem, 0, sizeof(lvitem) );
            lvitem.mask = LVIF_PARAM;
            lvitem.iItem = i;
            ListView_GetItem( mylistviewparam.hwndLV, &lvitem );
            if( lvitem.lParam != 0 )
            {
                object = FindPviObjectByName( (char*) lvitem.lParam );
                if( object != NULL )
                {
                    object->watchsort = lvitem.iItem;
                }
            }
            i = ListView_GetNextItem( mylistviewparam.hwndLV, i, LVNI_ALL );
        }


    }
}


/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: SaveWatchFile
 Created  : Tue Mar 14 10:54:20 2006
 Modified : Tue Mar 14 10:54:20 2006

 Synopsys : speichert ein Watchfile

 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void SaveWatchFile( char *filename )
{
    int index;
    LVITEM lvitem;
    PVIOBJECT *object;

    remove(filename);
    index = ListView_GetNextItem( mylistviewparam.hwndLV, -1, LVNI_ALL );
    while( index != -1 )
    {
        memset( &lvitem, 0, sizeof(lvitem) );
        lvitem.mask = LVIF_PARAM;
        lvitem.iItem = index;
        ListView_GetItem( mylistviewparam.hwndLV, &lvitem );
        if( lvitem.lParam != 0 )
        {
            object = FindPviObjectByName( (char*) lvitem.lParam );
            if( object != NULL )
            {
                object->watchsort = lvitem.iItem;
                AddToPviWatchFile( object, filename );
            }
        }
        index = ListView_GetNextItem( mylistviewparam.hwndLV, index, LVNI_ALL );
    }

}



/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: LoadWatchFile
 Created  : Tue Mar 14 10:54:32 2006
 Modified : Tue Mar 14 10:54:32 2006

 Synopsys : Lädt ein WatchFile

 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void LoadWatchFile( char *filename )
{
    int objecttype[] = { POBJ_DEVICE, POBJ_CPU, POBJ_TASK, POBJ_PVAR, -1 };
    PVIOBJECT *object;
    int t=0;
    HCURSOR oldcursor;
    int result, n;
    int sort = 0;

    oldcursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

    // Objekte aus Datei laden
    n = 0;
    while( objecttype[t] != -1 )
    {
        result = LoadPviObjectsFromWatchFile( objecttype[t], filename );
        if( result > 0 )
        {
            n += result;
        }
        ++t;
    }

    // Objecte in der Liste suchen und im Baum eintragen


    while( n )
    {
        object = GetNextPviObject(TRUE);
        while ( (object = GetNextPviObject(FALSE)) != NULL )
        {
            if(object->gui_info.loaded_from_file && object->watchsort == sort )
            {
                object->gui_info.loaded_from_file = 0;
                ++sort;
                --n;
                InsertInWatchList( object );
            }
        }
    }

    UpdateWindow(mylistviewparam.hwndLV);
    SetCursor( oldcursor );
}

/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: ListViewUpdateValue
 Created  : Wed Apr  5 13:28:45 2006
 Modified : Wed Apr  5 13:28:45 2006

 Synopsys : Übersetzt die Daten eines Objectes in ASCII und trägt diese ins
            Listview ein

 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void ListViewUpdateValue( PVIOBJECT *object )
{
    LVFINDINFO lvfindinfo;
    int index;
    char *format_decimal=NULL;
    char *format_hex=NULL;
    char *format_binary=NULL;
    char *format_char=NULL;
    char *text_binary=NULL;
    long long intval=0;
    int days, hours, minutes, seconds, milliseconds;
    char value[256];
    char tempstring[256];

    memset( &lvfindinfo, 0, sizeof(lvfindinfo) );

    lvfindinfo.flags = LVFI_PARAM;
    lvfindinfo.lParam = (LPARAM) object->name;

    index = ListView_FindItem( mylistviewparam.hwndLV, -1, &lvfindinfo );
    if( index >= 0 )  	// Objekt in Listview gefunden
    {
        switch( object->type )
        {
        case POBJ_PVAR:
            switch( object->ex.pv.type )
            {
            case BR_USINT:
                if( object->ex.pv.pvalue != NULL )
                {
                    unsigned char x;
                    memcpy( &x, object->ex.pv.pvalue, 1 );
                    intval = x;
                }
                format_decimal 		= "10#%+3llu  ";
                format_hex 			= "16#%2.2llX  ";
                format_binary 		= "2#%s  ";
                format_char 		= "'%c'";
                text_binary 		= int2bin(intval,8);
                break;

            case BR_SINT:
                if( object->ex.pv.pvalue != NULL )
                {
                    signed char x;
                    memcpy( &x, object->ex.pv.pvalue, 1 );
                    intval = x;
                }
                format_decimal 		= "10#%+3lli  ";
                format_hex 			= "16#%2.2llX  ";
                format_binary 		= "2#%s  ";
                format_char 		= "'%c'";
                text_binary 		= int2bin(intval,8);
                break;

            case BR_UINT:
                if( object->ex.pv.pvalue != NULL )
                {
                    unsigned short x;
                    memcpy( &x, object->ex.pv.pvalue, 2 );
                    intval = x;
                }
                format_decimal 		= "10#%+5llu  ";
                format_hex 			= "16#%4.4llX  ";
                format_binary 		= "2#%s";
                format_char 		= "";
                text_binary 		= int2bin(intval,16);
                break;

            case BR_INT:
                if( object->ex.pv.pvalue != NULL )
                {
                    signed short x;
                    memcpy( &x, object->ex.pv.pvalue, 2 );
                    intval = x;
                }
                format_decimal 		= "10#%+5lli  ";
                format_hex 			= "16#%4.4llX  ";
                format_binary 		= "2#%s";
                format_char 		= "";
                text_binary 		= int2bin(intval,16);
                break;

            case BR_UDINT:
                if( object->ex.pv.pvalue != NULL )
                {
                    unsigned int x;
                    memcpy( &x, object->ex.pv.pvalue, 4 );
                    intval = x;
                }
                format_decimal 		= "10#%+10llu  ";
                format_hex 			= "16#%8.8llX  ";
                format_binary 		= "2#%s";
                format_char			= "";
                text_binary 		= int2bin(intval,32);
                break;

            case BR_DINT:
                if( object->ex.pv.pvalue != NULL )
                {
                    signed int x;
                    memcpy( &x, object->ex.pv.pvalue, 4 );
                    intval = x;
                }
                format_decimal 		= "10#%+10lli  ";
                format_hex 			= "16#%8.8llX  ";
                format_binary 		= "2#%s";
                format_char			= "";
                text_binary 		= int2bin(intval,32);
                break;

            case BR_BOOL:
                if( object->ex.pv.pvalue != NULL )
                {
                    unsigned char x;
                    memcpy( &x, object->ex.pv.pvalue, 1 );
                    intval = x;
                }
                format_decimal 		= "%1.1i    ";
                format_hex 			= "";
                format_char			= "";
                format_binary 		= intval ? "TRUE" : "FALSE";
                break;


            case BR_REAL:
                if( object->ex.pv.pvalue != NULL )
                {
                    sprintf( value, "%g", *((float*) object->ex.pv.pvalue) );
                }
                break;

            case BR_LREAL:
                if( object->ex.pv.pvalue != NULL )
                {
                    sprintf( value, "%g", *((double*) object->ex.pv.pvalue) );
                }
                break;

            case BR_STRING:
                if( object->ex.pv.pvalue != NULL )
                {
                    char *s;
                    char *d;
                    s = (char*) object->ex.pv.pvalue;
                    d = value;
                    while( *s )
                    {
                        if( (*s & 0xff) < ' ' )
                        {
                            sprintf( d, "\\x%3.3x", (*s & 0xff) );
                            d+=5;
                            ++s;
                        }
                        else
                        {
                            *d++=*s++;
                        }
                    }
                    *d = 0;
                    if( object->gui_info.interpret_as_oem )
                    {
                        strcpy(tempstring, value);
                        OemToChar( tempstring, value );
                    }
                }
                break;


            case BR_DATI:
                if( object->ex.pv.pvalue != NULL )
                {
                    struct tm* ptst;
                    __time64_t t=0;

                    memset( value, 0, sizeof(value) );

                    memcpy( &t, object->ex.pv.pvalue, sizeof(time_t) );
                    ptst = _gmtime64( &t );
                    if( ptst != NULL )
                        strncpy( value, asctime( ptst ), 24 );
                    else
                        strcpy( value, "(illegal time!)" );

                    if( object->gui_info.display_as_decimal )
                    {
                        char tempstring[20];
                        sprintf( tempstring, "  (10#%.5u)", *((unsigned int*) object->ex.pv.pvalue) );
                        strcat( value, tempstring );
                    }
                }
                break;

            case BR_TIME:
                if( object->ex.pv.pvalue != NULL )
                {
                    memcpy( &intval, object->ex.pv.pvalue, 4 );
                    days = (int) intval / (24*3600*1000);
                    intval -= days * (24*3600*1000);
                    hours = (int) intval / (3600*1000);
                    intval -= hours * (3600*1000);
                    minutes = (int) intval / (60*1000);
                    intval -= minutes * (60*1000);
                    seconds = (int) intval / 1000;
                    intval -= seconds * 1000;
                    milliseconds = (int) intval;
                    sprintf( value, "%2.1ud %2.1uh %2.1um %2.1us %3.3ums", days, hours, minutes, seconds, milliseconds );
                    if( object->gui_info.display_as_decimal )
                    {
                        char tempstring[20];
                        sprintf( tempstring, "  (10#%.5u)", *((unsigned int*) object->ex.pv.pvalue) );
                        strcat( value, tempstring );
                    }
                }
                break;

            default:
                strcpy( value, "???" );
                break;

            }


            // die Formate von Integer- Datentypen kann man umschalten
            if( object->ex.pv.type == BR_USINT || object->ex.pv.type == BR_SINT ||
                    object->ex.pv.type == BR_UINT || object->ex.pv.type == BR_INT ||
                    object->ex.pv.type == BR_UDINT || object->ex.pv.type == BR_DINT ||
                    object->ex.pv.type == BR_BOOL  )
            {
                strcpy( value, "" );
                if( object->gui_info.display_as_decimal )
                {
                    sprintf( tempstring, format_decimal, intval );
                    strcat( value, tempstring );
                }
                if( object->gui_info.display_as_hex )
                {
                    sprintf( tempstring, format_hex, intval );
                    strcat( value, tempstring );
                }
                if( object->gui_info.display_as_binary )
                {
                    sprintf( tempstring, format_binary, text_binary );
                    strcat( value, tempstring );
                }
                if( object->gui_info.display_as_char )
                {
                    sprintf( tempstring, format_char, intval );
                    strcat( value, tempstring );
                }
            }


            break;

        case POBJ_CPU:
            sprintf( value, "%10s(%s)  RTC:%4.4u/%2.2u/%2.2u  %2.2u:%2.2u:%2.2u",
                     object->ex.cpu.status,
                     object->ex.cpu.running ? "RUN":"STOP",
                     object->ex.cpu.rtc_time.tm_year + 1900,
                     object->ex.cpu.rtc_time.tm_mon + 1,
                     object->ex.cpu.rtc_time.tm_mday,
                     object->ex.cpu.rtc_time.tm_hour,
                     object->ex.cpu.rtc_time.tm_min,
                     object->ex.cpu.rtc_time.tm_sec			 );
            break;

        case POBJ_TASK:
            if( ((PVIOBJECT*)object->ex.task.cpu)->ex.cpu.running == FALSE )
            {
                strcpy( value, "(CPU not running)");
            }
            else
            {
                strcpy( value, object->ex.task.status );
            }
            break;

        default:
            strcpy( value, "" );
            break;
        }

        ListView_SetItemText( mylistviewparam.hwndLV, index, 3, value );

    }

}


/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: ListViewKeydown
 Created  : Tue Feb 21 13:16:36 2006
 Modified : Tue Feb 21 13:16:36 2006

 Synopsys : wid aufgerufen, wenn im Listview Tasten betätigt werden
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void ListViewKeydown( WORD vkey )
{
    int index;
    LVITEM lvitem;
    PVIOBJECT *object;


    switch( vkey )
    {
    case VK_DELETE:
        index = ListView_GetNextItem( mylistviewparam.hwndLV, -1, LVNI_SELECTED );
        while( index != -1 )
        {
            memset(&lvitem, 0, sizeof(lvitem) );
            lvitem.iItem = index;
            lvitem.mask = LVIF_PARAM;
            ListView_GetItem( mylistviewparam.hwndLV, &lvitem );
            object = FindPviObjectByName( (char*) lvitem.lParam );
            WatchPviObject( object, FALSE ); // aus dem Watch entfernen
            ListView_DeleteItem( mylistviewparam.hwndLV, index ); // aus der Liste entfernen
            index = ListView_GetNextItem( mylistviewparam.hwndLV, index, LVNI_SELECTED );
        }
        break;


    case VK_RETURN:
        index = ListView_GetNextItem( mylistviewparam.hwndLV, -1, LVNI_SELECTED );
        if( index != -1 )
        {
            memset(&lvitem, 0, sizeof(lvitem) );
            lvitem.iItem = index;
            lvitem.mask = LVIF_PARAM;
            ListView_GetItem( mylistviewparam.hwndLV, &lvitem );

            if( lvitem.lParam != 0 )
            {
                object = FindPviObjectByName((char*)lvitem.lParam );
                if( object->type == POBJ_PVAR )
                {
                    DialogBoxParam( ghInstance, MAKEINTRESOURCE(DLG_WRITE_PVAR), hMainWindow,
                                    (DLGPROC) WritePvarDlgProc, (LPARAM) object );
                }
            }
        }
        break;
    }
}



/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: ListViewDblClick
 Created  : Wed Feb 22 16:10:05 2006
 Modified : Wed Feb 22 16:10:05 2006

 Synopsys : wird aufgerufen, wenn der Benutzer eine Zeile im Listview
            doppelclickt
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/

static void ListViewDblClick( LPNMLISTVIEW lpnm )
{
    LVITEM lvitem;
    PVIOBJECT *object;
    BOOL write_allowed;

    // Ini- Datei lesen
    write_allowed = GetPrivateProfileInt( "General", "WriteAllowed", 0, GetIniFile() );

    if( !write_allowed )
        return;

    memset( &lvitem, 0, sizeof(lvitem) );
    lvitem.mask = LVIF_PARAM;
    lvitem.iItem = lpnm->iItem;
    ListView_GetItem( mylistviewparam.hwndLV, &lvitem );


    if( lvitem.lParam != 0 )
    {
        object = FindPviObjectByName((char*)lvitem.lParam );
        if( object->type == POBJ_PVAR )
        {
            DialogBoxParam( ghInstance, MAKEINTRESOURCE(DLG_WRITE_PVAR), hMainWindow,
                            (DLGPROC) WritePvarDlgProc, (LPARAM) object );
        }
    }

}


static void ListViewRClick( LPNMLISTVIEW lpnm )
{
    HMENU hmenu;
    //RECT offset;
    LVITEM lvitem;
    UINT enabled;
    char *entry;
    POINT pt;


    memset( &lvitem, 0, sizeof(lvitem) );
    lvitem.mask = LVIF_PARAM ;
    lvitem.iItem = lpnm->iItem;
    ListView_GetItem( mylistviewparam.hwndLV, &lvitem );

    if( lvitem.lParam == 0 )
        return; // sollte eigentlich nicht vorkommen :-)

    selected_object = FindPviObjectByName( (char*) lvitem.lParam );

    if( selected_object == NULL )
        return; // sollte auch nicht vorkommen...

    //GetWindowRect( mylistviewparam.hwndLV, &offset );
    GetCursorPos(&pt);

    hmenu = CreatePopupMenu();

    switch( selected_object->type )
    {
    case POBJ_PVAR:
        switch( selected_object->ex.pv.type )
        {
        case BR_USINT:
        case BR_SINT:
            enabled = selected_object->gui_info.display_as_char ? MF_CHECKED : MF_UNCHECKED;
            AppendMenu( hmenu, MF_ENABLED | MF_STRING | enabled, IDM_CHANGE_VIEW_CHAR, "char" );
        case BR_UINT:
        case BR_INT:
        case BR_UDINT:
        case BR_DINT:
            enabled = selected_object->gui_info.display_as_decimal ? MF_CHECKED : MF_UNCHECKED;
            AppendMenu( hmenu, MF_ENABLED | MF_STRING | enabled, IDM_CHANGE_VIEW_DECIMAL, "dec" );
            enabled = selected_object->gui_info.display_as_hex ? MF_CHECKED : MF_UNCHECKED;
            AppendMenu( hmenu, MF_ENABLED | MF_STRING | enabled, IDM_CHANGE_VIEW_HEX, "hex" );
            enabled = selected_object->gui_info.display_as_binary ? MF_CHECKED : MF_UNCHECKED;
            AppendMenu( hmenu,  MF_ENABLED | MF_STRING | enabled, IDM_CHANGE_VIEW_BINARY, "bin" );
            break;

        case BR_REAL:
        case BR_LREAL:
            AppendMenu( hmenu, MF_ENABLED | MF_STRING, 0, "no option" );
            break;

        case BR_STRING:
            entry = selected_object->gui_info.interpret_as_oem ? "interpret as ANSI" : "interpret as OEM";
            AppendMenu( hmenu, MF_ENABLED | MF_STRING, IDM_CHANGE_VIEW_OEMCHAR, entry );
            break;

        case BR_TIME:
        case BR_DATI:
            entry = selected_object->gui_info.display_as_decimal ? "hide decimal value" : "show decimal value";
            AppendMenu( hmenu, MF_ENABLED | MF_STRING, IDM_CHANGE_VIEW_DECIMAL, entry );
            break;

        default:
            break;
        }

        break;

    default:
        break;

    }

    AppendMenu( hmenu, MF_ENABLED | MF_STRING, IDM_DELETE_OBJECT_FROM_LIST, "remove this object" );
    //TrackPopupMenu( hmenu, TPM_VERTICAL, offset.left + lpnm->ptAction.x , offset.top + lpnm->ptAction.y, 0, hMainWindow, NULL );
    TrackPopupMenu( hmenu, TPM_VERTICAL, pt.x, pt.y, 0, hMainWindow, NULL );

}



static void ListViewActivate( LPNMLISTVIEW lpnm )
{
    LVITEM lvitem;

    memset( &lvitem, 0, sizeof(lvitem) );
    lvitem.mask = LVIF_PARAM ;
    lvitem.iItem = lpnm->iItem;
    ListView_GetItem( mylistviewparam.hwndLV, &lvitem );

}


static void ListViewBeginDragging( int iItem )
{
    HIMAGELIST himl;
    LVITEM lvitem;
    POINT p;
    RECT rect_mainwindow, rect_listview;

    ZeroMemory( &rect_mainwindow, sizeof(rect_mainwindow) );
    ZeroMemory( &rect_listview, sizeof(rect_listview) );

    GetWindowRect( mylistviewparam.hwndLV, &rect_listview );
    GetWindowRect( hMainWindow, &rect_mainwindow );

    memset( &lvitem, 0, sizeof(lvitem) );
    lvitem.mask = LVIF_PARAM ;
    lvitem.iItem = iItem;
    ListView_GetItem( mylistviewparam.hwndLV, &lvitem );
    dragged_object = FindPviObjectByName( (char*) lvitem.lParam);

    if( dragged_object == NULL )
        return;

    himl = ListView_CreateDragImage( mylistviewparam.hwndLV, iItem, &p );
    ImageList_BeginDrag(himl, 0, 0, 0);
    // start drag effect
    ImageList_DragEnter(hMainWindow, rect_listview.left - rect_mainwindow.left,
                        rect_listview.top - rect_mainwindow.top );
    SetCapture(hMainWindow);

    WatchPviObject( dragged_object, FALSE ); // aus dem Watch entfernen
    ListView_DeleteItem( mylistviewparam.hwndLV, iItem );
    ListView_Update( mylistviewparam.hwndLV, iItem );

}


/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: WndProc
 Created  : Mon Feb 20 15:36:47 2006
 Modified : Mon Feb 20 15:36:47 2006

 Synopsys : die Fensterfunktion
 Input    :
 Output   :
 Errors   :
 ------------------------------------------------------------------@@-@@-*/
static LRESULT CALLBACK WndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static BOOL drag_splitter = FALSE;	//for splitter
    static UINT splitter_xpos = 250;	//for splitter
    static UINT	writewnd_height = 0;   // Höhe des Schreibfensters
//	static HWND hwndSettings=NULL; // settings- Fenster
    static HFONT hfont_watchlist = NULL; // Font für ListView
    RECT rect;
    HIMAGELIST himl;


    HANDLE_MYTREEVIEW_MSG(&mytreeviewparam, wMsg, wParam, lParam);
    HANDLE_MYLISTVIEW_MSG(&mylistviewparam, wMsg, wParam, lParam);

    switch (wMsg)
    {

    case WM_COMMAND:
        switch (wParam)
        {
        case IDM_ABOUT:
            DialogBox(ghInstance, MAKEINTRESOURCE(DLG_ABOUT), hWnd, (DLGPROC)AboutDlgProc);
            break;

        case IDM_STATUSPVIOBJEKTE:
            //DialogBox(ghInstance, MAKEINTRESOURCE(DLG_SHOWPVIOBJECTS), hWnd, (DLGPROC)ShowPviObjectsDlgProc);
            ShowPviObjects(  );
            break;
        case IDM_MNU_REFRESH:
            if( MessageBox( hWnd, "Delete all entries.\n\nAre you sure ?", "Clear Objects", MB_YESNO | MB_ICONWARNING ) == IDYES )
            {
                HCURSOR oldcursor;

                oldcursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
                TreeView_DeleteAllItems(mytreeviewparam.hwndTV);
                ListView_DeleteAllItems(mylistviewparam.hwndLV);
                StopPvi();
                Sleep(1000);
                StartPvi();
                SetCursor(oldcursor);
            }
            break;
        case IDM_SAVE_WATCH_FILE:
        {
            char filename[MAX_PATH];
            OPENFILENAME ofn;
            strcpy(filename, "default");
            memset(&ofn, 0, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = "WatchFile\0*.wtc\0\0";
            ofn.lpstrFile = filename;
            ofn.lpstrDefExt = "wtc";
            ofn.nMaxFile = sizeof(filename);
            ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
            if (GetSaveFileName(&ofn))
            {
                SaveWatchFile(filename);
            }
        }
        break;

        case IDM_LOAD_WATCH_FILE:
        {
            char filename[MAX_PATH];
            OPENFILENAME ofn;
            memset(&ofn, 0, sizeof(ofn));
            strcpy(filename, "default");
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = "WatchFile\0*.wtc\0\0";
            ofn.lpstrFile = filename;
            ofn.lpstrDefExt = "wtc";
            ofn.nMaxFile = sizeof(filename);
            ofn.Flags = OFN_HIDEREADONLY;
            if (GetOpenFileName(&ofn))
            {
                LoadWatchFile(filename);
            }
        }
        break;


        case IDM_LOGGER_CONFIG:
            DialogBox(ghInstance, MAKEINTRESOURCE(DLG_LOGGER_CONFIG), hWnd, (DLGPROC)LoggerConfigDlgProc);
            break;
            break;

        case IDM_LOGGER_START:
            Logger();
            SendMessage( hWnd, NOTIFY_LOGGER_STATUS, 0, 0 );
            break;

        case IDM_LOGGER_UNZIPLOGGERFILE:
        {
            char filename[MAX_PATH];
            int result;
            char tempstring[80];

            OPENFILENAME ofn;
            memset(&ofn, 0, sizeof(ofn));
            strcpy(filename, "");
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = "ZippedLoggerFile(*.gz)\0*.gz\0\0";
            ofn.lpstrFile = filename;
            ofn.lpstrDefExt = "gz";
            ofn.nMaxFile = sizeof(filename);
            ofn.Flags = OFN_HIDEREADONLY;
            if (GetOpenFileName(&ofn))
            {
                result = DecompressFile(filename);
                if( result == 0 )
                {
                    MessageBox( hWnd, "File was successfully unzipped !", "Unzip Logger file", MB_OK );
                }
                else
                {
                    sprintf( tempstring, "Error unzipping file: %i", result );
                    MessageBox( hWnd, tempstring, "Unzip Logger file", MB_OK | MB_ICONERROR );
                }
            }
        }
        break;



        /*
        case ID_LOGGER_OPENLOGGERFILE:
        {
            char filename[MAX_PATH];

            OPENFILENAME ofn;
            memset(&ofn, 0, sizeof(ofn));
            strcpy(filename, "logger");
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = "Logger File\0*.csv\0\0";
            ofn.lpstrFile = filename;
            ofn.lpstrDefExt = "csv";
            ofn.nMaxFile = sizeof(filename);
            ofn.Flags = OFN_HIDEREADONLY;
            if (GetOpenFileName(&ofn))
            {
                Plot(filename);
            }
        }
        break;
*/


        case IDM_CHANGE_VIEW_DECIMAL:
            if (selected_object != NULL)
            {
                selected_object->gui_info.display_as_decimal ^= 1;
                ListViewUpdateValue( selected_object );
            }
            break;

        case IDM_CHANGE_VIEW_HEX:
            if (selected_object != NULL)
            {
                selected_object->gui_info.display_as_hex ^= 1;
                ListViewUpdateValue( selected_object );
            }
            break;

        case IDM_CHANGE_VIEW_BINARY:
            if (selected_object != NULL)
            {
                selected_object->gui_info.display_as_binary ^= 1;
                ListViewUpdateValue( selected_object );
            }
            break;

        case IDM_CHANGE_VIEW_CHAR:
            if (selected_object != NULL)
            {
                selected_object->gui_info.display_as_char ^= 1;
                ListViewUpdateValue( selected_object );
            }
            break;

        case IDM_CHANGE_VIEW_OEMCHAR:
            if (selected_object != NULL)
            {
                selected_object->gui_info.interpret_as_oem ^= 1;
                ListViewUpdateValue( selected_object );
            }
            break;


        case IDM_DELETE_OBJECT_FROM_LIST:
        {
            LVITEM lvitem;
            int i;
            //char *temp;

            i= ListView_GetNextItem( mylistviewparam.hwndLV, -1, LVNI_ALL );
            while( i!= -1 )
            {
                memset( &lvitem, 0, sizeof(lvitem) );
                lvitem.mask = LVIF_PARAM;
                lvitem.iItem = i;
                ListView_GetItem( mylistviewparam.hwndLV, &lvitem );
                if( lvitem.lParam != 0 )
                {
                    //temp = (char*) lvitem.lParam;
                    if( strcmp( selected_object->name, (char*) lvitem.lParam ) == 0 )
                    {
                        WatchPviObject( selected_object, FALSE ); // aus dem Watch entfernen
                        ListView_DeleteItem( mylistviewparam.hwndLV, i ); // aus der Liste entfernen
                        break;
                    }
                }
                i = ListView_GetNextItem( mylistviewparam.hwndLV, i, LVNI_ALL );
            }
        }
        break;


        case IDM_VIEW_SETDEFAULTFONT:
        {
            HFONT hfont = GetStockObject( DEFAULT_GUI_FONT );
            SendMessage( mylistviewparam.hwndLV, WM_SETFONT, (WPARAM) hfont, TRUE );
            if( hfont_watchlist != NULL )
            {
                DeleteObject( hfont_watchlist );
                hfont_watchlist = NULL;
            }
        }
        break;

        case IDM_VIEW_SELECT_FONT:
        {
            CHOOSEFONT cf;
            LOGFONT font;


            if( hfont_watchlist != NULL )
            {
                DeleteObject( hfont_watchlist );
                hfont_watchlist = NULL;
            }

            ZeroMemory(&font, sizeof(font) );
            font.lfCharSet = ANSI_CHARSET;
            font.lfWeight = FW_THIN;
            strncpy( font.lfFaceName,"System", LF_FACESIZE );

            ZeroMemory(&cf, sizeof(cf) );
            cf.lStructSize = sizeof(cf);
            cf.hInstance = ghInstance;
            cf.hwndOwner = hWnd;
            cf.Flags = CF_SCREENFONTS | CF_NOVERTFONTS | CF_INITTOLOGFONTSTRUCT ;
            cf.lpLogFont = &font;
            if( ChooseFont(&cf) )
            {
                hfont_watchlist = CreateFontIndirect( &font );
                SendMessage( mylistviewparam.hwndLV, WM_SETFONT, (WPARAM) hfont_watchlist, 1 );
            }
        }
        break;

        case IDM_OPTIONS_SETTINGS:
            DialogBox( ghInstance, MAKEINTRESOURCE(DLG_SETTINGS), hWnd, (DLGPROC) SettingsDlg );
            break;
        }

        return 0;


    case WM_SIZE:
    {
        int nWidth = LOWORD(lParam);  // width of client area
        int nHeight = HIWORD(lParam); // height of client area

        // für Splitter
        MoveWindow(mytreeviewparam.hwndTV, 0, 0, splitter_xpos - 1, nHeight, TRUE);
        MoveWindow(mylistviewparam.hwndLV, splitter_xpos + 1, 0, nWidth - 101, nHeight -writewnd_height, TRUE);
//		MoveWindow(mywritelistviewparam.hwndLV, splitter_xpos + 1, nHeight-writewnd_height, nWidth - 101, writewnd_height, TRUE);
    }
    return 0;

    case WM_LBUTTONDOWN:
        // für Splitter
        SetCapture(hWnd);
        drag_splitter = TRUE;
        return 0;

    case WM_LBUTTONUP:
        // für Splitter
        ReleaseCapture();
        drag_splitter = FALSE;

        // Droppen vom Object
        if (dragged_object != NULL)
        {
            ImageList_EndDrag();
            ImageList_DragLeave(hMainWindow);
            ReleaseCapture();
            dragged_object->watchsort = drop_target_index;
            InsertInWatchList(dragged_object);
            dragged_object = NULL;
            TreeView_Select(mytreeviewparam.hwndTV, 0, TVGN_DROPHILITE);	// nur wg. Optik
        }
        return 0;

    case WM_KEYDOWN:
        switch( wParam )
        {
        case VK_TAB:
            SetFocus( mytreeviewparam.hwndTV );
            break;
        }
        return 0;

    case WM_MOUSEMOVE:
        if (drag_splitter)
        {
            GetClientRect(hWnd, &rect);
            if ((LOWORD(lParam) > 50) && (LOWORD(lParam) < (rect.right) - 50))
            {
                MoveWindow(mytreeviewparam.hwndTV, 0, 0, LOWORD(lParam) - 1, rect.bottom, TRUE);
                MoveWindow(mylistviewparam.hwndLV, LOWORD(lParam) + 1, 0, rect.right - LOWORD(lParam) + 1, rect.bottom - writewnd_height, TRUE);
//				MoveWindow(mywritelistviewparam.hwndLV, LOWORD(lParam) + 1, rect.bottom - writewnd_height, rect.right - LOWORD(lParam) + 1, writewnd_height, TRUE);
                splitter_xpos = LOWORD(lParam);
            }
        }
        if (dragged_object != NULL)  	// Dragging eines Ojbektes vom TreeView zum ListView
        {
            POINTS Pos;
            LVHITTESTINFO lvht;
            Pos.x = GET_X_LPARAM(lParam);
            Pos.y = GET_Y_LPARAM(lParam);
            ImageList_DragMove(Pos.x, Pos.y+40);
            //
            memset(&lvht, 0, sizeof(lvht));
            lvht.pt.x = Pos.x;
            lvht.pt.y = Pos.y;
            ListView_HitTest(mylistviewparam.hwndLV, &lvht);
            if (lvht.flags & LVHT_NOWHERE)  	// innerhalb Listview, aber über keinem Eintrag ?
            {
                drop_target_index = ListView_GetItemCount(mylistviewparam.hwndLV); // ans Ende der Liste
            }
            else if (lvht.flags & LVHT_ONITEM)
            {
                drop_target_index = lvht.iItem;
            }
            else
                drop_target_index = -1; // nicht einfügen
            ListView_SetHotItem(mylistviewparam.hwndLV, lvht.iItem);
        }
        return 0;

    case WM_SYSCOMMAND:
        if( wParam == SC_MINIMIZE )
        {
            Shell_NotifyIcon(NIM_ADD, &notifyicondata );
            ShowWindow( hMainWindow, SW_HIDE );
            return 0;
        }
        break;


    case NOTIFY_ICON_MESSAGE:
        switch( LOWORD( lParam ) )
        {
        case WM_LBUTTONDOWN:
            ShowWindow( hMainWindow, SW_RESTORE );
            Shell_NotifyIcon(NIM_DELETE, &notifyicondata );
            break;
        }
        break;


    case NOTIFY_LOGGER_STATUS:
        if( IsLoggerRunning() )
        {
            char logger_watch_file[MAX_PATH];

            // Logger gestartet...
            ModifyMenu( GetMenu(hWnd), IDM_LOGGER_START, MF_BYCOMMAND | MF_STRING, IDM_LOGGER_START, "Logger &Stop" );
            EnableWindow( mylistviewparam.hwndLV, FALSE );
            EnableWindow( mytreeviewparam.hwndTV, FALSE );
            WritePrivateProfileString( "General", "LoggerActive", "1", GetIniFile() );
            strcpy( logger_watch_file, GetApplicationPath() );
            strcat( logger_watch_file, "\\_logger.wtc" );
            SaveWatchFile( logger_watch_file );
        }
        else
        {
            // Logger gestoppt ...
            ModifyMenu( GetMenu(hWnd), IDM_LOGGER_START, MF_BYCOMMAND | MF_STRING, IDM_LOGGER_START, "Logger &Start" );
            EnableWindow( mylistviewparam.hwndLV, TRUE );
            EnableWindow( mytreeviewparam.hwndTV, TRUE );
            WritePrivateProfileString( "General", "LoggerActive", "0", GetIniFile() );
        }
        break;

    case WM_CREATE:
        InitCommonControls();
        /* Imagelist */
        himl = CreateImageList();

        dragged_object = NULL;

        /* TreeView */
        memset(&mytreeviewparam, 0, sizeof(mytreeviewparam));
        mytreeviewparam.hinstance = ghInstance;
        mytreeviewparam.hwndParent = hWnd;
        mytreeviewparam.cbselected = TreeViewItemSelected;	// Callback
        mytreeviewparam.cbbegindrag = TreeViewBeginDragging;
        mytreeviewparam.cbdblclick = TreeViewDblClick;
        mytreeviewparam.cbrclick = TreeViewRClick;

        CreateMyTreeView(&mytreeviewparam);
        TreeView_SetImageList(mytreeviewparam.hwndTV, himl, TVSIL_NORMAL);
        //TreeView_SetImageList(mytreeviewparam.hwndTV, himl, TVSIL_STATE );

        /* Listview */
        memset(&mylistviewparam, 0, sizeof(mylistviewparam));
        mylistviewparam.hinstance = ghInstance;
        mylistviewparam.hwndParent = hWnd;
        mylistviewparam.column[0].name = "Name";
        mylistviewparam.column[0].width = 100;
        mylistviewparam.column[1].name = "Type";
        mylistviewparam.column[1].width = 100;
        mylistviewparam.column[2].name = "Scope";
        mylistviewparam.column[2].width = 100;
        mylistviewparam.column[3].name = "Value";
        mylistviewparam.column[3].width = 600;
        mylistviewparam.cbkeydown = ListViewKeydown;	// Callbacks
        mylistviewparam.cbdblclick = ListViewDblClick;
        mylistviewparam.cbrclick = ListViewRClick;
        mylistviewparam.cbbegindrag = ListViewBeginDragging;
        mylistviewparam.cbactivate = ListViewActivate;

        CreateMyListView(&mylistviewparam);
        ListView_SetImageList(mylistviewparam.hwndLV, himl, LVSIL_SMALL);

        /* Schreibfenster */
        writewnd_height = 0;
        // if( write_enabled ){
        // writewnd_height = 20;
        // mywritelistviewparam.hwndLV = CreateWindowEx( 0, "EDIT", "", WS_CHILD | WS_VISIBLE,
        // 0, 0, 0, 0, hWnd, 0, ghInstance, NULL);
        // }

        if (StartPvi())
            MessageBox(hMainWindow, "Beim Initialisieren von PVI ist ein Fehler aufgetreten.\nsiehe Logdatei !", "Fehler", MB_OK);
        if( ZipDllFound() )
        {
            EnableMenuItem( GetMenu(hWnd), IDM_LOGGER_UNZIPLOGGERFILE, MF_ENABLED );
        }
        else
        {
            EnableMenuItem( GetMenu(hWnd), IDM_LOGGER_UNZIPLOGGERFILE, MF_GRAYED );
        }
        return (0);

    case WM_DESTROY:
        if( hfont_watchlist != NULL )
        {
            DeleteObject( hfont_watchlist );
            hfont_watchlist = NULL;
        }
        PostQuitMessage(0);
        return (0);


    }

    return DefWindowProc(hWnd, wMsg, wParam, lParam);

}







