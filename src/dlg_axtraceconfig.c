#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include "main.h"
#include "pvi_interface.h"



LRESULT CALLBACK  ConfigAxTraceDlg(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	//static PVIOBJECT *obj_fub;		// FUB
	//static PVIOBJECT *obj_axname;	// FUB.axname
	//static PVIOBJECT *obj_modname;	// FUB.modname

	//PVIOBJECT tempobject;

//	static UINT_PTR timer_id;

	switch (uMsg) {
		case WM_INITDIALOG:

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

		case WM_TIMER:

			break;
	}

	return FALSE;
}
