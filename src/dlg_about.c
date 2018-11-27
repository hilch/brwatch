
#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "main.h"
#include "resource.h"
#include "pvicom.h"
#include "zip.h"

LRESULT WINAPI AboutDlgProc(HWND, UINT, WPARAM, LPARAM);



LRESULT CALLBACK AboutDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch (uMsg)
    {
    case WM_INITDIALOG:
        /*
         * Nothing special to initialize.
         */
#ifdef _DEBUG
#define VERSION "Debug!!!"
#else
#define VERSION ""
#endif
        SetDlgItemText( hDlg, IDC_STATIC_VERSION, "BRWATCH V1.3 (BUILD " __DATE__ "  " __TIME__ "  " VERSION ")" );

       // PviGetVersion( versiontext, sizeof(versiontext) );
        SetDlgItemText( hDlg, IDC_STATIC_PVIVERSION, g_PVIVersionString );
        SetDlgItemText( hDlg, IDS_ZLIB1, GetZibLibVersion() );
        return TRUE;

    case WM_COMMAND:
        switch (wParam)
        {
        case IDOK:
        case IDCANCEL:
            /*
             * OK or Cancel was clicked, close the dialog.
             */
            EndDialog(hDlg, TRUE);
            return TRUE;
        }
        break;
    }

    return FALSE;
}
