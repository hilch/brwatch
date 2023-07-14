#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include <windows.h>



BOOL CALLBACK  BusyDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_INITDIALOG:
			/*
			 * Nothing special to initialize.
			 */
			return TRUE;

		case WM_COMMAND:
			switch (wParam) {
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
