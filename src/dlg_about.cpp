
#define WIN32_LEAN_AND_MEAN /* speed up compilations */
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "main.h"
#include "resource.h"
#include "pvicom.h"
#include "git_info.h"

INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM /* lParam */)
{

	switch (uMsg)
	{
	case WM_INITDIALOG:
		/*
		 * Nothing special to initialize.
		 */

#ifdef __DEBUG__
#warning debug version !!!
#define VERSION "Debug!!!"
#else
#define VERSION ""
#endif

		SetDlgItemTextA(hDlg, IDC_STATIC_VERSION, "BRWATCH " VER_PRODUCTVERSION_STR " (BUILD " __DATE__ "  " __TIME__ "  " VERSION ")");
		SetDlgItemTextA(hDlg, IDC_STATIC_GIT_HASH, "Git hash: " BRWATCH_GIT_HASH);
		SetDlgItemTextA(hDlg, IDC_STATIC_GIT_TAG, "Git tag: " BRWATCH_GIT_TAG);

		// PviGetVersion( versiontext, sizeof(versiontext) );
		SetDlgItemTextA(hDlg, IDC_STATIC_PVIVERSION, g_PVIVersionString);
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
