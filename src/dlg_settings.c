#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "main.h"



LRESULT CALLBACK SettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_INITDIALOG:
			/*
			 * Nothing special to initialize.
			 */
			SendDlgItemMessage( hDlg, IDR_BUTTON_DEVICE1, BM_SETCHECK, BST_CHECKED, 0 );
			return TRUE;

		case WM_COMMAND:
			switch (wParam) {
				case IDR_BUTTON_DEVICE1:
					MessageBox( hDlg, "SERIAL", "Click", MB_OK );
					return TRUE;

				case IDR_BUTTON_DEVICE2:
					MessageBox( hDlg, "SERIAL", "Click", MB_OK );
					return TRUE;

				case IDOK:
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
