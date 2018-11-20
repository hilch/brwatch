
#define WIN32_LEAN_AND_MEAN  /* speed up compilations */

#include "main.h"
#include "mytreeview.h"
#include "mylistview.h"
#include "pvi_interface.h"
#include "stringtools.h"
#include "logger.h"
#include "zip.h"
#include "settings.h"
#include "dlg_writepar.h"
#include "dlg_about.h"
#include "dlg_showpviobjects.h"
#include "resource.h"
#include <wingdi.h>


/** Prototypes **************************************************************/

static LRESULT WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
//static void Main_OnDestroy(HWND);
static void PviObjectsValid(void);
static void PviObjectErrorChanged( PVIOBJECT *object, int errcode );
//static void PviObjectDataChanged(PVIOBJECT *object, char* value );
//static void PviObjectStatusChanged( PVIOBJECT *object, char* status );

static void LoadWatchFile( char *filename );
static void SaveWatchFile( char *filename );


/** Global variables ********************************************************/


static char application_path[MAX_PATH];






//static HTREEITEM hselected_treeitem;

static int	drop_target_index;
static int instance_counter;

static NOTIFYICONDATA notifyicondata;





char *GetApplicationPath(void)
{
    return application_path;
}

HWND MainWindowGetHandle( void )
{
    return( g_hwndMainWindow );
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


    HMODULE m_user32Dll = LoadLibrary("User32.dll");
    if(m_user32Dll) {
        FARPROC pFunc = GetProcAddress(m_user32Dll, "SetProcessDPIAware");
        if(pFunc) {
            pFunc();
        }
        FreeLibrary(m_user32Dll);
        m_user32Dll = NULL;
    }

    // Pfad der Applikation speichern
    if (GetCurrentDirectory(sizeof(application_path), application_path))
    {
    }
    else
    {
        MessageBox( NULL, "GetCurrentDirectory", "Error", MB_OK );
        return -1;
    }

    SettingsInitialize(); // ggf. Default- Ini kreieren

    // instance counter
    instance_counter = 1;
    EnumWindows( EnumWindowsProc, 0 );

    sprintf( windowname, APPLICATION_NAME " {%u. instance}", instance_counter );

    InitCommonControls();

    g_hInstance = hInstance;
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

    g_hwndMainWindow = CreateWindow(_T("brwatchClass"), windowname, WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    if (!g_hwndMainWindow)
        return 0;
    ShowWindow(g_hwndMainWindow, nCmdShow);
    UpdateWindow(g_hwndMainWindow);


    // Notify area
    memset( &notifyicondata, 0, sizeof(notifyicondata) );
    notifyicondata.cbSize = sizeof(notifyicondata);
    notifyicondata.hWnd = g_hwndMainWindow;
    strcpy( notifyicondata.szTip, windowname );
    strcat( notifyicondata.szTip, " - Click here to size up" );
    notifyicondata.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_ICO_MAIN));
    notifyicondata.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    notifyicondata.uCallbackMessage = NOTIFY_ICON_MESSAGE;

    // set callback functions for PVI
    pvi_interface_notify.cbobjects_valid = PviObjectsValid;
    pvi_interface_notify.cberror_changed = PviObjectErrorChanged;
    pvi_interface_notify.cbdata_changed = MyListViewUpdateValue;


    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    StopPvi();
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
            MyTreeViewAddItem(&mytreeviewparam, TVI_ROOT, object->descriptor, (LPARAM)object->name, 0);
        }
    }

    // if logger was active when program ended it will be started again.
    if( GetPrivateProfileInt( "General", "LoggerActive", 0, SettingsGetFileName() ) )  // was logger active ?
    {
        strcpy( logger_watch_file, GetApplicationPath() );
        strcat( logger_watch_file, "\\_logger.wtc" );
        LoadWatchFile( logger_watch_file );
        Logger();
        SendMessage( MainWindowGetHandle(), NOTIFY_LOGGER_STATUS, 0, 0 );
        Shell_NotifyIcon(NIM_ADD, &notifyicondata );
        ShowWindow( g_hwndMainWindow, SW_HIDE );
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
                MyListViewInsertPVIObjects( object );
            }
        }
    }

    UpdateWindow(mylistviewparam.hwndLV);
    SetCursor( oldcursor );
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

    if( MyListViewHandleMessages( &mylistviewparam, wMsg, wParam, lParam ) == 0 )
        return(0);

    if( MyTreeViewHandleMessages(&mytreeviewparam, wMsg, wParam, lParam) == 0 )
        return(0);

    switch (wMsg)
    {

    case WM_COMMAND:
        switch (wParam)
        {
        case IDM_ABOUT:
            DialogBox(g_hInstance, MAKEINTRESOURCE(DLG_ABOUT), hWnd, (DLGPROC)AboutDlgProc);
            break;

        case IDM_STATUSPVIOBJEKTE:
            //DialogBox(ghInstance, MAKEINTRESOURCE(DLG_SHOWPVIOBJECTS), hWnd, (DLGPROC)ShowPviObjectsDlgProc);
            ShowWindowPviObjects(  );
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
            DialogBox(g_hInstance, MAKEINTRESOURCE(DLG_LOGGER_CONFIG), hWnd, (DLGPROC)LoggerConfigDlgProc);
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


        case IDM_CHANGE_VIEW_DECIMAL:
            if (g_selectedPVIObject != NULL)
            {
                g_selectedPVIObject->gui_info.display_as_decimal ^= 1;
                MyListViewUpdateValue( g_selectedPVIObject );
            }
            break;

        case IDM_CHANGE_VIEW_HEX:
            if (g_selectedPVIObject != NULL)
            {
                g_selectedPVIObject->gui_info.display_as_hex ^= 1;
                MyListViewUpdateValue( g_selectedPVIObject );
            }
            break;

        case IDM_CHANGE_VIEW_BINARY:
            if (g_selectedPVIObject != NULL)
            {
                g_selectedPVIObject->gui_info.display_as_binary ^= 1;
                MyListViewUpdateValue( g_selectedPVIObject );
            }
            break;

        case IDM_CHANGE_VIEW_CHAR:
            if (g_selectedPVIObject != NULL)
            {
                g_selectedPVIObject->gui_info.display_as_char ^= 1;
                MyListViewUpdateValue( g_selectedPVIObject );
            }
            break;

        case IDM_CHANGE_VIEW_OEMCHAR:
            if (g_selectedPVIObject != NULL)
            {
                g_selectedPVIObject->gui_info.interpret_as_oem ^= 1;
                MyListViewUpdateValue( g_selectedPVIObject );
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
                    if( strcmp( g_selectedPVIObject->name, (char*) lvitem.lParam ) == 0 )
                    {
                        WatchPviObject( g_selectedPVIObject, FALSE ); // aus dem Watch entfernen
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
            cf.hInstance = g_hInstance;
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
            DialogBox( g_hInstance, MAKEINTRESOURCE(DLG_SETTINGS), hWnd, (DLGPROC) SettingsDlg );
            break;

        case IDM_QUIT:
            DestroyWindow( hWnd );
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
        if (g_draggedPVIObject != NULL)
        {
            ImageList_EndDrag();
            ImageList_DragLeave(g_hwndMainWindow);
            ReleaseCapture();
            g_draggedPVIObject->watchsort = drop_target_index;
            MyListViewInsertPVIObjects(g_draggedPVIObject);
            g_draggedPVIObject = NULL;
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
        if (g_draggedPVIObject != NULL)  	// Dragging eines Ojbektes vom TreeView zum ListView
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
            ShowWindow( g_hwndMainWindow, SW_HIDE );
            return 0;
        }
        break;


    case NOTIFY_ICON_MESSAGE:
        switch( LOWORD( lParam ) )
        {
        case WM_LBUTTONDOWN:
            ShowWindow( g_hwndMainWindow, SW_RESTORE );
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
            WritePrivateProfileString( "General", "LoggerActive", "1", SettingsGetFileName() );
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
            WritePrivateProfileString( "General", "LoggerActive", "0", SettingsGetFileName() );
        }
        break;

    case WM_CREATE:
        InitCommonControls();
        /* Imagelist */
        himl = ResourcesCreateImageList();

        g_draggedPVIObject = NULL;

        /* TreeView */
        memset(&mytreeviewparam, 0, sizeof(mytreeviewparam));
        mytreeviewparam.hinstance = g_hInstance;
        mytreeviewparam.hwndParent = hWnd;
        mytreeviewparam.cbselected = MyTreeViewItemSelected;	// Callback
        mytreeviewparam.cbbegindrag = MyTreeViewBeginDragging;
        mytreeviewparam.cbdblclick = MyTreeViewDblClick;
        mytreeviewparam.cbrclick = MyTreeViewRClick;

        MyTreeViewCreateWindow(&mytreeviewparam);
        TreeView_SetImageList(mytreeviewparam.hwndTV, himl, TVSIL_NORMAL);
        //TreeView_SetImageList(mytreeviewparam.hwndTV, himl, TVSIL_STATE );

        /* Listview */
        memset(&mylistviewparam, 0, sizeof(mylistviewparam));
        mylistviewparam.hinstance = g_hInstance;
        mylistviewparam.hwndParent = hWnd;
        mylistviewparam.column[0].name = "Name";
        mylistviewparam.column[0].width = 100;
        mylistviewparam.column[1].name = "Type";
        mylistviewparam.column[1].width = 100;
        mylistviewparam.column[2].name = "Scope";
        mylistviewparam.column[2].width = 100;
        mylistviewparam.column[3].name = "Value";
        mylistviewparam.column[3].width = 600;
        mylistviewparam.cbkeydown = MyListViewKeydown;	// Callbacks
        mylistviewparam.cbdblclick = MyListViewDblClick;
        mylistviewparam.cbrclick = MyListViewRClick;
        mylistviewparam.cbbegindrag = MyListViewBeginDragging;
        mylistviewparam.cbactivate = MyListViewActivate;

        MyListViewCreateWindow(&mylistviewparam);
        ListView_SetImageList(mylistviewparam.hwndLV, himl, LVSIL_SMALL);

        /* Schreibfenster */
        writewnd_height = 0;
        // if( write_enabled ){
        // writewnd_height = 20;
        // mywritelistviewparam.hwndLV = CreateWindowEx( 0, "EDIT", "", WS_CHILD | WS_VISIBLE,
        // 0, 0, 0, 0, hWnd, 0, ghInstance, NULL);
        // }

        if (StartPvi())
            MessageBox(g_hwndMainWindow, "Beim Initialisieren von PVI ist ein Fehler aufgetreten.\nsiehe Logdatei !", "Fehler", MB_OK);
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







