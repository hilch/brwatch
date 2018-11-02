#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include "main.h"
#include <math.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>
#include "plot.h"
#include "logger.h"


//#define NUM    1000
//#define TWOPI  (2 * 3.14159)


#define WM_USER_DISPLAY_STATUS			(WM_USER+1)
#define WM_USER_REDRAWPLOT				(WM_USER+2)
#define WM_USER_COPYTOCLIPBOARD			(WM_USER+3)
#define WM_USER_DATAREAD				(WM_USER+4)
#define WM_USER_PLOTCURSORMOVED			(WM_USER+5)
#define WM_USER_HIDEPLOT				(WM_USER+7)
#define WM_USER_UPDATEMENU				(WM_USER+8)
#define WM_USER_ERROR_CLOSE				(WM_USER+9)


#define MAX_TRACES					20
//#define BORDER_Y					5   /* Rand für Y- Achse */
//#define BORDER_RIGHT				100  /* Rand rechte Seite (X) */
#define ID_MENU_TRACE				100
#define GREY						RGB(0xcc, 0xcc, 0xcc)
#define CURSOR_SIZE					10


const COLORREF colors[MAX_TRACES] =
{
    RGB( 0xff, 0x00, 0x00 ),		// Knatschrot
    RGB( 0, 0xff, 0 ),				// Grün
    RGB( 0x1c, 0x86, 0xee ),		// Blau
    RGB( 0, 0xff, 0xff ),			// Cyan
    RGB( 0x8b, 0x45, 0x13 ),		// Braun
    RGB( 0xff, 0x8c, 0x00 ),		// Orange
    RGB( 0xff, 0x14, 0x93 ),		// Pink
    RGB( 0x94, 0x00, 0xd3 ),	// Violett
    RGB( 0xff, 0x63, 0x47 ),	// "Tomato"
    RGB( 0xf0, 0x80, 0x80 ),  // Pink
    RGB( 0xf0, 0xff, 0xf0 ),  //
    RGB( 0xff, 0x6a, 0x6a ),  //
    RGB( 255, 255, 255 ),  // Weiss
    RGB( 255, 255, 255 ),  // Weiss
    RGB( 255, 255, 255 ),  // Weiss
    RGB( 255, 255, 255 ),  // Weiss
    RGB( 255, 255, 255 ),  // Weiss
    RGB( 255, 255, 255 ),  // Weiss
    RGB( 255, 255, 255 ),  // Weiss
    RGB( 255, 255, 255 )  // Weiss
};

/* Strukturen */

typedef struct
{
    HWND		hwnd;										// Plot- Window- Handle
    HWND		hwnd_info;									// Info- Window- Handle
    RECT		rcPlot;										// Größe des Plot- Bereiches
    char		name[256];									// Name der Spur
    double		maxvalue;									// maximaler Wert
    double		minvalue;									// minimaler Wert
    double		scale;										// Skalierungsfaktor für Anzeige
    BOOL		draw;										// 1 = Trace soll gezeichnet werden
    POINT		cursor_old;									// "alte" Cursorposition
    POINT		refcursor_old;								// "alte" Referenzcursorposition
    double		*value;
    COLORREF	color;
} PLOT_TRACE_typ;


typedef struct
{
    int				no_of_entries;						// Anzahl der Einträge
    int				no_of_visible_pixels;
    int				first_visible_entry;				// Index des ersten sichtbaren Eintrags;
    int				entry_under_cursor;					// Index des ersten Eintrags unter Mouse- Cursor
    int				entry_under_refcursor;				// Index des ersten Eintrags unter Referenz- Cursor
    int				cursor_x_old;						// "alte" Cursorposition
    int				scrollpos;							// Position des Scrollbars in %
    int				zoomx;								// Zoom in X- Richtung
    BOOL			drag_splitter;
    HMENU			hmenu;								// Plot- Haupt- Menü
    HMENU			hsubmenu_traces;						//
    HWND			hwnd_status;						// Handle Statusleiste
    int				no_of_traces;						// Anzahl der Spuren
    int				no_of_visible_traces;				// Anzahl der sichtbaren Spuren
    __time64_t		starttime;							// Startzeit des Trace
    PLOT_TRACE_typ	trace[MAX_TRACES];
    int				*timestamp;							// Zeitstempel
} PLOT_DATA_typ;

/* Prototypen */
LRESULT CALLBACK PlotMainWndProc (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK PlotTraceWndProc (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK PlotInfoWndProc (HWND, UINT, WPARAM, LPARAM) ;


/* globale Variable */

static 	int	cxChar, cxCaps, cyChar;						// Font- Abmessungen


void PlotInit( HINSTANCE hInstance )
{
#ifdef NOT_WORKING
    WNDCLASS wc;
    HBRUSH hbrush;


    hbrush = CreateSolidBrush( RGB( 0xee, 0xe9, 0xe9 ) );
    hbrush = hbrush;

    memset(&wc, 0, sizeof(wc));
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)PlotTraceWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(PLOT_DATA_typ*);
    wc.hInstance = hInstance;
    wc.hIcon = NULL; //LoadIcon(hInstance, MAKEINTRESOURCE(IDR_ICO_MAIN));
    wc.hCursor    = LoadCursor(NULL, IDC_CROSS);
    wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
    wc.lpszMenuName = NULL; //MAKEINTRESOURCE(IDR_MNU_PLOT);;
    wc.lpszClassName = _T("brwatchPlotTraceClass");


    if (!RegisterClass(&wc))
    {
        MessageBox( GetMainWindow(), "Error Initializing Plot-Window", "Plotting Loggerfile", MB_OK | MB_ICONERROR );
    }

    memset(&wc, 0, sizeof(wc));
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)PlotMainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(PLOT_DATA_typ*);
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_ICO_PLOT));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); //LoadCursor(NULL, IDC_SIZENS);	//for splitter
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW; //(HBRUSH) GetStockObject (BLACK_BRUSH) ;
    wc.lpszMenuName = NULL; //MAKEINTRESOURCE(IDR_MNU_PLOT);;
    wc.lpszClassName = _T("brwatchPlotMainClass");


    if (!RegisterClass(&wc))
    {
        MessageBox( GetMainWindow(), "Error Initializing Plot-MainWindow", "Plotting Loggerfile", MB_OK | MB_ICONERROR );
    }

    memset(&wc, 0, sizeof(wc));
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)PlotInfoWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(PLOT_DATA_typ*);
    wc.hInstance = hInstance;
    wc.hIcon = NULL; //LoadIcon(hInstance, MAKEINTRESOURCE(IDR_ICO_MAIN));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
    wc.lpszMenuName = NULL; //MAKEINTRESOURCE(IDR_MNU_PLOT);;
    wc.lpszClassName = _T("brwatchPlotInfoClass");


    if (!RegisterClass(&wc))
    {
        MessageBox( GetMainWindow(), "Error Initializing Plot-InfoWindow", "Plotting Loggerfile", MB_OK | MB_ICONERROR );
    }
    //DeleteObject( hbrush );
#endif // NOT_WORKING
}




/* -----------------------------------------------------------------------------------------
   file mit den Tracedaten lesen und darstellen
   -----------------------------------------------------------------------------------------*/

void Plot( char *csvfile )
{
#ifdef NOT_WORKING
    HWND hplotwnd;
    MSG msg;
    HCURSOR oldcursor;
    HINSTANCE hInstance = (HINSTANCE)((LONG_PTR) GetWindowLongPtr( GetMainWindow(), GWLP_HINSTANCE));
    FILE *stream;
    PLOT_DATA_typ *pplotdata;
    char buffer[2048];
    BOOL header_found;
    BOOL starttime_found;
    BOOL memory_allocated;
    int i;
    char *s;
    int entry=0;
    int ticks = 0, ticks_old = 0;
    __time64_t ti;
    struct tm ts;


    pplotdata = (PLOT_DATA_typ*) malloc( sizeof(PLOT_DATA_typ) );
    memset( pplotdata, 0, sizeof(PLOT_DATA_typ) );

    if( pplotdata == NULL || buffer == NULL )
    {
        MessageBox( GetMainWindow(), "no memory", "Plotting Loggerfile", MB_OK | MB_ICONERROR );
        goto freememory;
    }

    hplotwnd = CreateWindow( _T("brwatchPlotMainClass"), _T("Plot"), WS_OVERLAPPEDWINDOW | WS_HSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, GetMainWindow(), NULL, hInstance, pplotdata );
    ShowWindow( hplotwnd, SW_SHOW  );



    /* Menü erstellen */
    pplotdata->hmenu = CreateMenu();
    pplotdata->hsubmenu_traces = CreateMenu();

    /* Warte- Meldung */
    sprintf( buffer, "Loading %s...", csvfile );
    SetWindowText( hplotwnd, buffer );
    oldcursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    // Datei einlesen
    stream = fopen( csvfile, "rt" );

    header_found = 0;
    starttime_found = 0;
    memory_allocated = 0;

    if( stream != NULL )
    {
        while( !feof(stream) )   /* PASS 1 */
        {
            memset( buffer, 0, sizeof(buffer) );
            fgets( buffer, sizeof(buffer) -1, stream );   // Zeile einlesen

            if( !header_found )  // Header parsen
            {
                header_found = 1;
                if( strncmp( buffer, LOGFILE_HEADER, strlen(LOGFILE_HEADER) ) )
                {
                    SendMessage( hplotwnd, WM_USER_ERROR_CLOSE, 0, (LPARAM) "Illegal Loggerfile !" );
                    fclose(stream);
                    goto freememory;
                }
                else
                {
                    char seps[]   = ";\n";
                    char *token;
                    pplotdata->no_of_traces = 0; // Anzahl der Spalten zaehlen
                    s = buffer+strlen(LOGFILE_HEADER)+1;


                    token = strtok( s, seps );
                    while( token != NULL )
                    {
                        memset( &pplotdata->trace[pplotdata->no_of_traces], 0, sizeof(PLOT_TRACE_typ) );
                        strcpy( pplotdata->trace[pplotdata->no_of_traces].name, token );  // Name der Spur speichern
                        pplotdata->trace[pplotdata->no_of_traces].color = colors[pplotdata->no_of_traces];
                        AppendMenu( pplotdata->hsubmenu_traces, MF_CHECKED | MF_ENABLED | MF_STRING, ID_MENU_TRACE + pplotdata->no_of_traces, token );
                        pplotdata->trace[pplotdata->no_of_traces].draw = 1;
                        pplotdata->trace[pplotdata->no_of_traces].maxvalue = -1e40;		// Extremwerte initialisieren
                        pplotdata->trace[pplotdata->no_of_traces].minvalue = +1e40;
                        if( pplotdata->no_of_traces++ >= MAX_TRACES )  // max. Anzahl Traces erreicht
                            break;
                        token = strtok( NULL, seps );
                    }
                    AppendMenu( pplotdata->hmenu, MF_POPUP | MF_STRING, (int) pplotdata->hsubmenu_traces, "Trace" );
                    entry = 0;
                }
            }
            else    // alle anderen Zeilen zaehlen
            {
                char seps[]   = ";\n";
                char *token;
                __int64 filelength;

                // Speicher allokieren, wenn noch nicht geschehen
                if( !memory_allocated )
                {
                    filelength = _filelengthi64( _fileno(stream) );
                    if( strlen(buffer) != 0 )
                    {
                        pplotdata->no_of_entries = (int) filelength / strlen(buffer); // Näherungswert ;-)
                    }
                    else
                        pplotdata->no_of_entries = 0;

                    if( pplotdata->no_of_entries <= 1 )
                    {
                        SendMessage( hplotwnd, WM_USER_ERROR_CLOSE, 0, (LPARAM) "To less information in Loggerfile !" );
                        fclose(stream);
                        goto freememory;
                    }


                    pplotdata->timestamp = (int*) malloc( (size_t) pplotdata->no_of_entries * sizeof(int) );
                    if( pplotdata->timestamp != NULL )
                    {
                        for( i = 0; i < pplotdata->no_of_traces; ++i )
                        {
                            pplotdata->trace[i].value = (double*) malloc( pplotdata->no_of_entries * sizeof(double) );
                            if( pplotdata->trace[i].value != NULL )
                            {
                                memory_allocated = 1;
                            }
                            else
                            {
                                sprintf( buffer, "no memory for %u rows", pplotdata->no_of_entries );
                                SendMessage( hplotwnd, WM_USER_ERROR_CLOSE, 0, (LPARAM) buffer );
                                fclose(stream);
                                goto freememory;


                            }
                        }
                    }
                }


                /* Zeilen parsen... */
                s = buffer;
                token = strtok( s, seps );

                // Zeitstempel
                if( token != NULL )
                {
                    // rtc
                    if( sscanf( token, "%4u/%2u/%2u-%2u:%2u:%2u", &ts.tm_year, &ts.tm_mon, &ts.tm_mday, &ts.tm_hour, &ts.tm_min, &ts.tm_sec ) )
                    {
                        ts.tm_year -= 1900;
                        ts.tm_mon -=1;
                        ts.tm_isdst = 0;
                        ti = _mktime64( &ts );

                        // Ticks
                        token = strtok( NULL, seps );
                        if( token != NULL )
                        {
                            if( sscanf( token, "%4u", &ticks ) )
                            {
                                if( !starttime_found )  // erster Eintrag ?
                                {
                                    pplotdata->starttime = ti;
                                    ticks_old = ticks;
                                    starttime_found = 1;
                                    pplotdata->timestamp[0] = 0;
                                }
                                else
                                {
                                    int diff = ticks - ticks_old;
                                    if( diff < 0 ) // Überlauf des Tickzählers
                                        diff += 10000;
                                    pplotdata->timestamp[entry] = pplotdata->timestamp[entry-1] + diff;
                                    ticks_old = ticks;
                                }
                            }
                        }

                    }
                }


                // Werte
                i = 0; // Zähler für den aktuellen Trace
                token = strtok( NULL, seps );


                while( token != NULL )
                {
                    pplotdata->trace[i].value[entry] = atof( token ); // Werte speichern
                    // Extrem werte ermitteln
                    pplotdata->trace[i].maxvalue = __max( pplotdata->trace[i].maxvalue, pplotdata->trace[i].value[entry] );
                    pplotdata->trace[i].minvalue = __min( pplotdata->trace[i].minvalue, pplotdata->trace[i].value[entry] );
                    token = strtok( NULL, seps );
                    if( ++i >= MAX_TRACES )
                        break;
                }

                if( ++entry >= pplotdata->no_of_entries )  // Anzahl der Einträge zählen
                    break; // Sollte eigentlich nie auftreten

            }
        } /* end of while( !feof(stream) )... */

        fclose(stream);
        pplotdata->no_of_entries = entry-1; // jetzt sollte dies genau stimmen !
    }
    else
    {
        MessageBox( GetMainWindow(), "Error Loading Loggerfile", "Plotting Loggerfile", MB_OK | MB_ICONERROR );
    }

    // Ende Datei einlesen

    SetMenu( hplotwnd, pplotdata->hmenu );
    SetCursor(oldcursor); // Cursor wiederherstellen
    sprintf( buffer, "Plot of %s", csvfile );
    SetWindowText( hplotwnd, buffer );
    SendMessage( hplotwnd, WM_USER_DATAREAD, 0, 0 );

freememory:

    // Nachrichtenschleife für das Plotfenster
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    /* Speicher freigeben */

    free( pplotdata->timestamp );
    for( i = 0; i < pplotdata->no_of_traces; ++i )
    {
        if( pplotdata->trace[i].value )
        {
            free(pplotdata->trace[i].value );
            pplotdata->trace[i].value = NULL;
        }
    }

    free(pplotdata);
#endif
}




/* -----------------------------------------------------------------------------------------
   Plot- Main-Window
   -----------------------------------------------------------------------------------------*/
LRESULT CALLBACK PlotMainWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    const int splitter_size = 6;
    HINSTANCE hInstance;
    HDC hdc;
    PLOT_DATA_typ *inst;
    int i,j, k;
    RECT rc, rc2;
    SCROLLINFO  si ;
    POINT pt;
    HFONT hfont_old;
    TEXTMETRIC  tm ;
    PAINTSTRUCT ps;
    static char *errormessage;

    switch (message)
    {

    case WM_CREATE:
        SetWindowLong( hwnd, 0, (LONG)((LPCREATESTRUCT) lParam)->lpCreateParams ); // Übergabewert von CreateWindow() speichern
        inst = (PLOT_DATA_typ*) GetWindowLongPtr( hwnd, 0 );
        inst->hwnd_status = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hwnd, NULL, GetModuleHandle(NULL), NULL);
        SendMessage( inst->hwnd_status, WM_SIZE, 0, 0 );
        inst->entry_under_cursor = 0;
        inst->entry_under_refcursor = 0;
        /* Abmessungen des Font */
        hdc = GetDC (hwnd) ;
        hfont_old = SelectObject( hdc, GetStockObject(DEFAULT_GUI_FONT) );
        GetTextMetrics (hdc, &tm) ;
        SelectObject( hdc, hfont_old );
        ReleaseDC (hwnd, hdc) ;
        cxChar = tm.tmAveCharWidth ;
        cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2 ;
        cyChar = tm.tmHeight + tm.tmExternalLeading ;
        errormessage = 0;
        return 0;


    case WM_USER_DATAREAD: // Trace- Daten wurden eingelesen
        inst = (PLOT_DATA_typ*) GetWindowLongPtr( hwnd, 0 );
        hInstance = (HINSTANCE)((LONG_PTR) GetWindowLongPtr( GetMainWindow(), GWLP_HINSTANCE));
        inst->no_of_visible_traces = inst->no_of_traces;
        for( i = 0; i< inst->no_of_traces; ++i )
        {
            inst->trace[i].draw = 1;  // beim Starten werden alle Traces dargestellt
            inst->trace[i].hwnd = CreateWindow( _T("brwatchPlotTraceClass"), _T("Plot"), WS_CHILD | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, hInstance, &inst->trace[i] );
            inst->trace[i].hwnd_info = CreateWindow( _T("brwatchPlotInfoClass"), _T("Plot"), WS_CHILD | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd, NULL, hInstance, &inst->trace[i] );
        }
        GetClientRect( hwnd, &rc );
        SendMessage( hwnd, WM_SIZE, SIZE_MINIMIZED, MAKELPARAM( rc.right, rc.bottom ) );
        SendMessage( hwnd, WM_USER_DISPLAY_STATUS, 0, 0 );
        return 0;


    case WM_SIZE:
        inst = (PLOT_DATA_typ*) GetWindowLongPtr( hwnd, 0 );
        if( inst->no_of_visible_traces < 1 )
            return 0;

        /* Statusleiste anpassen */
        SendMessage( inst->hwnd_status, WM_SIZE, 0, 0 );

        GetClientRect( hwnd, &rc );

        /* Die Statusleiste nimmt uns noch etwas vom Client-Bereich weg */
        GetWindowRect( inst->hwnd_status, &rc2);
        if( ( rc.bottom-= (rc2.bottom - rc2.top)) < 0 )
            rc.bottom = 0;


        inst->no_of_visible_traces = inst->no_of_traces;
        for( i = 0; i < inst->no_of_traces; ++i )
        {
            if( !inst->trace[i].draw )
            {
                --inst->no_of_visible_traces;

            }
        }

        // Höhe der Plot-Windows berechnen
        if( inst->no_of_visible_traces == 0 )
            k = rc.bottom;
        else
            k = (rc.bottom- (inst->no_of_visible_traces-1)* splitter_size)/inst->no_of_visible_traces;

        j = 0;
        for( i = 0; i< inst->no_of_traces; ++i )
        {
            if( inst->trace[i].draw )
            {
                MoveWindow( inst->trace[i].hwnd, 0, j, rc.right - cxCaps * 15, k, TRUE );  // Plot- Fenster
                MoveWindow( inst->trace[i].hwnd_info, rc.right - cxCaps * 15, j, cxCaps * 15, k, TRUE ); // Info - Fenster
                j += ( k + splitter_size);
                ShowWindow( inst->trace[i].hwnd, SW_SHOW );
                ShowWindow( inst->trace[i].hwnd_info, SW_SHOW );

            }
            else
            {
                ShowWindow( inst->trace[i].hwnd, SW_HIDE );
                ShowWindow( inst->trace[i].hwnd_info, SW_HIDE );
            }
        }



        return 0;

    case WM_PAINT:
        hdc = BeginPaint( hwnd, &ps );
        if( errormessage != NULL )
        {
            GetClientRect( hwnd, &rc );
            SetTextAlign( hdc, TA_CENTER );
            SetBkMode( hdc, TRANSPARENT );
            TextOut( hdc, rc.right /2, rc.bottom / 2, errormessage, strlen( errormessage ) );
        }
        EndPaint( hwnd, &ps );
        return 0;


    case WM_LBUTTONDOWN:
        inst = (PLOT_DATA_typ*) GetWindowLongPtr( hwnd, 0 );
        SetWindowText( hwnd, "Lbutton" );
        // für Splitter
        SetCapture(hwnd);
        inst->drag_splitter = TRUE;
        return 0;

    case WM_LBUTTONUP:
        inst = (PLOT_DATA_typ*) GetWindowLongPtr( hwnd, 0 );
        // für Splitter
        ReleaseCapture();
        inst->drag_splitter = FALSE;
        return 0;

    case WM_COMMAND:
        inst = (PLOT_DATA_typ*) GetWindowLong( hwnd, 0 );

        inst->no_of_visible_traces = inst->no_of_traces;
        for( i = 0; i < inst->no_of_traces; ++i )
        {
            if( wParam == (ID_MENU_TRACE + i ) )   // Traces sollen ein- oder ausgeschaltet werden
            {
                inst->trace[i].draw = !inst->trace[i].draw;
                //CheckMenuItem( inst->hsubmenu_traces, wParam, inst->trace[i].draw ? MF_CHECKED : MF_UNCHECKED );
            }
        }
        SendMessage( hwnd, WM_USER_UPDATEMENU, 0, 0 );
        SendMessage( hwnd, WM_SIZE, 0, 0 );
        return 0;


    case WM_USER_UPDATEMENU:
        inst = (PLOT_DATA_typ*) GetWindowLong( hwnd, 0 );

        for( i = 0; i < inst->no_of_traces; ++i )
        {
            if( !inst->trace[i].draw )
                CheckMenuItem( inst->hsubmenu_traces, ID_MENU_TRACE + i, MF_BYCOMMAND | MF_UNCHECKED );
            else
                CheckMenuItem( inst->hsubmenu_traces, ID_MENU_TRACE + i, MF_BYCOMMAND | MF_CHECKED );
        }
        return 0;


    case WM_USER_ERROR_CLOSE:
        if( lParam != 0 )
        {
            errormessage = (char*) lParam;
            UpdateWindow( hwnd );
        }
        return 0;


    case WM_HSCROLL:
        inst = (PLOT_DATA_typ*) GetWindowLong( hwnd, 0 );

        // horizontale Bildlaufleiste abfragen
        si.cbSize = sizeof (si) ;
        si.fMask  = SIF_ALL ;  // alle Daten

        // Position für den späteren Vergleich festhalten
        GetScrollInfo (hwnd, SB_HORZ, &si) ;
        i = si.nPos ;

        switch (LOWORD (wParam))
        {
        case SB_LINELEFT:
            si.nPos -= 1 ;
            break ;

        case SB_LINERIGHT:
            si.nPos += 1 ;
            break ;

        case SB_PAGELEFT:
            si.nPos -= si.nPage ;
            break ;

        case SB_PAGERIGHT:
            si.nPos += si.nPage ;
            break ;

        case SB_THUMBPOSITION:
            si.nPos = si.nTrackPos ;
            break ;

        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;

        default :
            break ;
        }

        if( si.nPos > si.nMax )
            si.nPos = si.nMax;
        if( si.nPos < si.nMin )
            si.nPos = si.nMin;

        // erst setzen, dann wieder abfragen
        si.fMask = SIF_POS ;
        SetScrollInfo (hwnd, SB_HORZ, &si, TRUE) ;

        inst->scrollpos = si.nPos;

        // Wenn sich die Position geändert hat: Fenster horizontal rollen

        if (si.nPos != i)
        {
            for( i = 0; i < inst->no_of_traces; ++i )
            {
                SendMessage( inst->trace[i].hwnd, WM_USER_REDRAWPLOT, 0, 0 );
            }
        }
        SendMessage( hwnd, WM_USER_DISPLAY_STATUS, 0, 0 );
        return 0 ;


    case WM_USER_DISPLAY_STATUS:  /* Statustext ausgeben */
    {
        char temp[256];
        __time64_t t;
        int ms;
        struct tm *ts;
        double diff;

        TIME_ZONE_INFORMATION tzi;
        //
        GetTimeZoneInformation( &tzi );
        //
        inst = (PLOT_DATA_typ*) GetWindowLong( hwnd, 0 );

        t = inst->starttime - tzi.Bias*60 + inst->timestamp[inst->entry_under_cursor] / 1000 ;
        ts = _gmtime64( &t );
        ms = inst->timestamp[inst->entry_under_cursor] % 1000;
        diff = (double)(inst->timestamp[inst->entry_under_cursor] - inst->timestamp[inst->entry_under_refcursor])/1000.0;

        if( ts != NULL )
        {
            sprintf( temp, "row: %.9u/%9u     time:%fs     diff-time%fs     clock:%4.4u/%2.2u/%2.2u  %2.2u:%2.2u:%2.2u:%3.3u",
                     inst->entry_under_cursor+1, inst->no_of_entries,
                     (double) inst->timestamp[inst->entry_under_cursor]/1000.0,
                     diff,
                     ts->tm_year + 1900,
                     ts->tm_mon + 1,
                     ts->tm_mday,
                     ts->tm_hour,
                     ts->tm_min,
                     ts->tm_sec,
                     ms
                   );
        }
        SetWindowText( inst->hwnd_status, temp );
    }
    return 0;



    case WM_USER_PLOTCURSORMOVED:	// Mouse- Event vom aktiven Plot- Window
        inst = (PLOT_DATA_typ*) GetWindowLong( hwnd, 0 );
        for( i = 0; i < inst->no_of_traces; ++i )
        {
            SendMessage( inst->trace[i].hwnd, WM_USER_PLOTCURSORMOVED, wParam, lParam ); // an alle Plot- Fenster zurück
            SendMessage( inst->trace[i].hwnd_info, WM_USER_PLOTCURSORMOVED, wParam, lParam ); // an alle Plot- Info Fenster zurück
        }
        SendMessage( hwnd, WM_USER_DISPLAY_STATUS, 0, 0 );
        return 0;


    case WM_KEYDOWN:
        switch( LOWORD(wParam) )
        {
        case VK_LEFT:
            if( GetKeyState(VK_SHIFT)&0x8000  )
            {
                SendMessage( hwnd, WM_HSCROLL, SB_LINELEFT, 0 );
            }
            else
            {
                GetCursorPos( &pt );
                SetCursorPos( pt.x-1, pt.y);
            }
            break;

        case VK_RIGHT:
            if( GetKeyState(VK_SHIFT)&0x8000  )
            {
                SendMessage( hwnd, WM_HSCROLL, SB_LINERIGHT, 0 );
            }
            else
            {
                GetCursorPos( &pt );
                SetCursorPos( pt.x+1, pt.y);
            }
            break;
        }
        return 0;


    case WM_DESTROY:
        DestroyMenu( GetMenu(hwnd) );
        PostQuitMessage (0) ;
        return 0 ;
    }
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}




/* ------------------------------------------------------------------------------------------
  Callback für das einzelne Plotfenster
  -------------------------------------------------------------------------------------------
  */
LRESULT CALLBACK PlotTraceWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC         hdc ;
    HPEN		hpen, hpen_old;
    HFONT		hfont_old;
    HMENU		hmenu;
    int         i ;
    PAINTSTRUCT ps ;
    POINT		 *ppoints, pt;
    PLOT_TRACE_typ *inst;
    PLOT_DATA_typ *pplotdata;
    int ypos;


    switch (message)
    {

    case WM_CREATE:
        SetWindowLong( hwnd, 0, (LONG)((LPCREATESTRUCT) lParam)->lpCreateParams );
        return 0;


    case WM_SIZE:
        inst = (PLOT_TRACE_typ*) GetWindowLongPtr( hwnd, 0 );

        GetClientRect( hwnd, &inst->rcPlot );
        // Skalierungsfaktoren für die Traces berechnen
        inst->scale = (double) (inst->rcPlot.bottom - (5 + cyChar) - 10) / ( inst->maxvalue - inst->minvalue );
        return 0 ;



    case WM_PAINT:
        inst = (PLOT_TRACE_typ*) GetWindowLong( hwnd, 0 );
        pplotdata = (PLOT_DATA_typ*) GetWindowLong( GetParent(hwnd), 0 );

        pplotdata->no_of_visible_pixels = __min( inst->rcPlot.right-1, pplotdata->no_of_entries );
        pplotdata->first_visible_entry = pplotdata->scrollpos * pplotdata->no_of_entries / 100;
        if( (pplotdata->first_visible_entry + pplotdata->no_of_visible_pixels) >= pplotdata->no_of_entries )
            pplotdata->first_visible_entry = pplotdata->no_of_entries - pplotdata->no_of_visible_pixels;

        //pplotdata->entry_under_refcursor = pplotdata->first_visible_entry;
        // nee, den Referenzcursor lassen wir ggf. im alten Bild
        pplotdata->entry_under_cursor = pplotdata->first_visible_entry;

        ppoints = malloc( sizeof(POINT) * pplotdata->no_of_visible_pixels );

        if( ppoints == NULL )
            return 0;

        hdc = BeginPaint (hwnd, &ps) ;

        if( inst->draw )
        {

            for( i= 0; i< pplotdata->no_of_visible_pixels ; ++i )
            {
                ppoints[i].x = i;
                ppoints[i].y = inst->rcPlot.bottom - ( 5 +  (LONG)( ( inst->value[i + pplotdata->first_visible_entry] - inst->minvalue ) * inst->scale ));
            }

            // Zeichnen
            hpen = CreatePen( PS_SOLID, 0, inst->color );
            hpen_old = SelectObject( hdc, hpen );
            Polyline( hdc, ppoints, pplotdata->no_of_visible_pixels );
            SelectObject( hdc, hpen_old );
            DeletePen(hpen);

            // Überschrift
            hfont_old = SelectObject( hdc, GetStockObject(DEFAULT_GUI_FONT) );
            SetTextColor( hdc, inst->color );
            SetBkMode(hdc, TRANSPARENT );
            TextOut( hdc, 10, 2, inst->name, strlen(inst->name) );
            SelectObject( hdc, hfont_old );
            pplotdata->cursor_x_old = -1;  // Cursor muss neu gezeichnet werden

        }

        EndPaint (hwnd, &ps) ;
        free(ppoints);
        inst->cursor_old.x = inst->cursor_old.y = -1; // Cursor neu zeichnen
        inst->refcursor_old.x = inst->refcursor_old.y = -1;
        return 0 ;


    case WM_USER_REDRAWPLOT:
        inst = (PLOT_TRACE_typ*) GetWindowLong( hwnd, 0 );
        InvalidateRect( hwnd, NULL, TRUE );
        return 0;


    case WM_MOUSEMOVE:
        inst = (PLOT_TRACE_typ*) GetWindowLong( hwnd, 0 );
        pplotdata = (PLOT_DATA_typ*) GetWindowLong( GetParent(hwnd), 0 );
        i = LOWORD(lParam);

        if( wParam & MK_CONTROL )   // Referenzcursor soll bewegt werden
        {
            wParam = 1;
            pplotdata->entry_under_refcursor = pplotdata->first_visible_entry + i;
            if( pplotdata->entry_under_refcursor >= pplotdata->no_of_entries )
            {
                pplotdata->entry_under_refcursor = pplotdata->no_of_entries - 1;
                i = pplotdata->entry_under_refcursor - pplotdata->first_visible_entry;
            }
        }
        else   // Cursor soll bewegt werden
        {
            wParam = 0;
            pplotdata->entry_under_cursor = pplotdata->first_visible_entry + i;
            if( pplotdata->entry_under_cursor >= pplotdata->no_of_entries )
            {
                pplotdata->entry_under_cursor = pplotdata->no_of_entries - 1;
                i = pplotdata->entry_under_cursor - pplotdata->first_visible_entry;
            }
        }
        // Mouse- Event an das Main- Fenster schicken, damit dieses verteilt werden kann
        SendMessage( GetParent(hwnd), WM_USER_PLOTCURSORMOVED, wParam, MAKELPARAM( i, HIWORD(lParam)) );
        return 0;


    case WM_USER_PLOTCURSORMOVED:  // Plot- Cursor muss bewegt werden
        inst = (PLOT_TRACE_typ*) GetWindowLong( hwnd, 0 );
        pplotdata = (PLOT_DATA_typ*) GetWindowLong( GetParent(hwnd), 0 );

        // Xpos des Cursors
        i = LOWORD( lParam );

        hdc = GetDC (hwnd) ;
        SetROP2 (hdc, R2_XORPEN) ;

        if( wParam == 0 )     // Cursor
        {
            hpen = CreatePen( PS_SOLID,1, RGB(255,255,255) );
            hpen_old = SelectObject( hdc, hpen );

            // Ypos des Cursors
            ypos = inst->rcPlot.bottom - ( 5 + (LONG)( ( inst->value[pplotdata->entry_under_cursor] - inst->minvalue ) * inst->scale ));

            // alten Cursor löschen
            if( inst->cursor_old.x > -1 )
            {
                MoveToEx( hdc, inst->cursor_old.x, inst->cursor_old.y - CURSOR_SIZE/2-3, NULL );
                LineTo( hdc, inst->cursor_old.x, inst->cursor_old.y + CURSOR_SIZE);
                MoveToEx( hdc, inst->cursor_old.x - CURSOR_SIZE/2-3, inst->cursor_old.y, NULL );
                LineTo( hdc, inst->cursor_old.x + CURSOR_SIZE, inst->cursor_old.y );
            }

            // Neue Cursor zeichnen
            MoveToEx( hdc, i, ypos - CURSOR_SIZE/2-3, NULL );
            LineTo( hdc, i, ypos + CURSOR_SIZE );
            MoveToEx( hdc, i  - CURSOR_SIZE/2-3, ypos, NULL );
            LineTo( hdc, i + CURSOR_SIZE, ypos );

            inst->cursor_old.x = i;
            inst->cursor_old.y = ypos;

            SelectObject( hdc, hpen_old );
            DeletePen( hpen );
        }
        else    // Referenzcursor
        {
            hpen = CreatePen( PS_SOLID,1, RGB(0xff, 0xb9, 0x0f) );
            hpen_old = SelectObject( hdc, hpen );

            // Ypos des Cursors
            ypos = inst->rcPlot.bottom - ( 5 + (LONG)( ( inst->value[pplotdata->entry_under_refcursor] - inst->minvalue ) * inst->scale ));

            // alten Cursor löschen
            if( inst->refcursor_old.x > -1 )
            {
                MoveToEx( hdc, inst->refcursor_old.x, inst->refcursor_old.y - CURSOR_SIZE/2-3, NULL );
                LineTo( hdc, inst->refcursor_old.x, inst->refcursor_old.y + CURSOR_SIZE);
                MoveToEx( hdc, inst->refcursor_old.x - CURSOR_SIZE/2-3, inst->refcursor_old.y, NULL );
                LineTo( hdc, inst->refcursor_old.x + CURSOR_SIZE, inst->refcursor_old.y );
            }

            // Neue Cursor zeichnen
            MoveToEx( hdc, i, ypos - CURSOR_SIZE/2-3, NULL );
            LineTo( hdc, i, ypos + CURSOR_SIZE );
            MoveToEx( hdc, i  - CURSOR_SIZE/2-3, ypos, NULL );
            LineTo( hdc, i + CURSOR_SIZE, ypos );

            inst->refcursor_old.x = i;
            inst->refcursor_old.y = ypos;

            SelectObject( hdc, hpen_old );
            DeletePen( hpen );
        }


        ReleaseDC (hwnd, hdc) ;
        return 0;



    case WM_RBUTTONDOWN:
        hmenu = CreatePopupMenu();
        GetCursorPos( &pt );
        AppendMenu( hmenu,  MF_ENABLED | MF_STRING, WM_USER_COPYTOCLIPBOARD, "copy to clipboard" );
        AppendMenu( hmenu,  MF_ENABLED | MF_STRING, WM_USER_HIDEPLOT, "hide this trace" );
        TrackPopupMenu( hmenu, TPM_VERTICAL, pt.x, pt.y, 0, hwnd, NULL );
        return 0;


    case WM_COMMAND:
        inst = (PLOT_TRACE_typ*) GetWindowLong( hwnd, 0 );
        pplotdata = (PLOT_DATA_typ*) GetWindowLong( GetParent(hwnd), 0 );

        switch( LOWORD(wParam) )
        {

        case WM_USER_HIDEPLOT:
            inst->draw = FALSE;
            SendMessage( GetParent(hwnd), WM_USER_UPDATEMENU, 0, 0 );
            SendMessage( GetParent(hwnd), WM_SIZE, 0, 0 );
            break;

        case WM_USER_COPYTOCLIPBOARD:
        {
            HDC hdc2;
            HBITMAP hbitmap, hbitmap_old;
            RECT rc;

            inst = (PLOT_TRACE_typ*) GetWindowLong( hwnd, 0 );

            hdc = GetDC(hwnd);
            hdc2 = CreateCompatibleDC( hdc );
            GetClientRect( inst->hwnd_info, &rc );
            hbitmap = CreateCompatibleBitmap( hdc, inst->rcPlot.right + rc.right, inst->rcPlot.bottom);
            hbitmap_old = SelectObject( hdc2, hbitmap );
            //
            BitBlt( hdc2, 0, 0, inst->rcPlot.right, inst->rcPlot.bottom, hdc, 0, 0, SRCCOPY );
            ReleaseDC( hwnd, hdc );
            hdc = GetDC(inst->hwnd_info);
            BitBlt( hdc2, inst->rcPlot.right, 0, rc.right, rc.bottom, hdc, 0, 0, SRCCOPY );
            ReleaseDC( inst->hwnd_info, hdc );
            //
            OpenClipboard( hwnd );
            EmptyClipboard();
            SetClipboardData( CF_BITMAP, hbitmap );
            CloseClipboard();
            //
            SelectObject( hdc2, hbitmap_old );
            DeleteBitmap(hbitmap);
            DeleteDC( hdc2 );

        }
        break;

        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage (0) ;
        return 0 ;
    }

    return DefWindowProc (hwnd, message, wParam, lParam) ;
}




/* ----------------------------------------------------------------------------------------------------
   Fenster für die Informationen jedes Plots
   -----------------------------------------------------------------------------------------------------
*/
LRESULT CALLBACK PlotInfoWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    char tempstring[20];
    HFONT hfont_old;
    PLOT_TRACE_typ *inst;
    PLOT_DATA_typ *pplotdata;
    RECT rc;
    double d;


    switch (message)
    {

    case WM_CREATE:
        SetWindowLong( hwnd, 0, (LONG)((LPCREATESTRUCT) lParam)->lpCreateParams );
        return 0;


    case WM_USER_PLOTCURSORMOVED:
        InvalidateRect( hwnd, NULL, TRUE );
        return 0;

    case WM_PAINT:
        inst = (PLOT_TRACE_typ*) GetWindowLong( hwnd, 0 );
        pplotdata = (PLOT_DATA_typ*) GetWindowLong( GetParent(hwnd), 0 );
        GetClientRect( hwnd, &rc );
        //
        hdc = BeginPaint( hwnd, &ps );

        if( inst->draw )
        {
            hfont_old = SelectObject( hdc, GetStockObject(DEFAULT_GUI_FONT) );



            SetTextColor( hdc, RGB(0xff, 0xff, 0xff) ); // weiss
            SetBkMode(hdc, TRANSPARENT );

            // akt. Wert bei Cursor
            sprintf( tempstring, "y:%6g", inst->value[pplotdata->entry_under_cursor] );
            TextOut( hdc, 10, rc.bottom / 2, tempstring, strlen(tempstring) );

            // Differenz in Y
            d = inst->value[pplotdata->entry_under_cursor] - inst->value[pplotdata->entry_under_refcursor];
            sprintf( tempstring, "diff_y:%6g", d );
            TextOut( hdc, 10, rc.bottom / 2 + cyChar, tempstring, strlen(tempstring) );

            // max. Wert
            sprintf( tempstring, "max:%6g", inst->maxvalue );
            TextOut( hdc, 10, 0, tempstring, strlen(tempstring) );

            // min. Wert
            sprintf( tempstring, "min:%6g", inst->minvalue );
            TextOut( hdc, 10, rc.bottom - ( cyChar), tempstring, strlen(tempstring) );

            SelectObject( hdc, hfont_old );
        }
        EndPaint( hwnd, &ps );
        return 0;


    case WM_DESTROY:
        PostQuitMessage (0) ;
        return 0 ;
    }
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}

