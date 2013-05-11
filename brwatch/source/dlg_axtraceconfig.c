#include "main.h"
#include "pvi_interface.h"



LRESULT CALLBACK  ConfigAxTraceDlg(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	//static PVIOBJECT *obj_fub;		// FUB
	//static PVIOBJECT *obj_axname;	// FUB.axname
	//static PVIOBJECT *obj_modname;	// FUB.modname

	//PVIOBJECT tempobject;

	static UINT_PTR timer_id;

    switch (uMsg)
    {
        case WM_INITDIALOG:
			//if( lParam != 0 ) {
			//	char tempstring[256];

			//	obj_fub = (PVIOBJECT *) lParam;  // Object des FUBs
			//	// die Elemente des FUBs...
			//	sprintf( tempstring, "%s.axname", obj_fub->name );
			//	obj_axname = FindPviObjectByName( tempstring );

			//	// neues Object erstellen
			//	if( obj_axname != NULL ){
			//		ZeroMemory( &tempobject, sizeof(tempobject) );
			//		sprintf( tempobject.name, "%s_trace", obj_axname->name );
			//		strcpy( tempobject.descriptor, obj_axname->descriptor );
			//		tempobject.type = obj_axname->type;
			//		AddPviObject( &tempobject, FALSE );
			//	}

			//	sprintf( tempstring, "%s.modname", obj_fub->name );
			//	obj_modname = FindPviObjectByName( tempstring );


			//	if( obj_axname != NULL && obj_modname != NULL ) {
			//		WatchPviObject( obj_axname, TRUE );
			//		WatchPviObject( obj_modname, TRUE );
			//		SetDlgItemText( hDlg, IDR_STATIC_NAME, obj_axname->descriptor );
			//		timer_id = SetTimer( hDlg, 1, 200, NULL );
			//	}
			//	else {
			//		MessageBox( GetMainWindow(), "Error loading trace-settings", "Error!", MB_OK | MB_ICONERROR );
			//		return FALSE;
			//	}

			//}	
            return TRUE;

        case WM_COMMAND:
            switch (wParam)
            {
                case IDOK:
                case IDCANCEL:
                    /*
                     * OK or Cancel was clicked, close the dialog.
                     */
					//KillTimer( hDlg, 1 );
					//WatchPviObject( obj_axname, FALSE );
					//WatchPviObject( obj_axname, FALSE );
                    EndDialog(hDlg, TRUE);
                    return TRUE;
            }
            break;

		case WM_TIMER:
			//if( obj_axname->ex.pv.pvalue != NULL ){
			//	SetDlgItemText( hDlg, IDR_STATIC_NAME, obj_axname->ex.pv.pvalue );
			//}
			//if( obj_modname->ex.pv.pvalue != NULL ){
			//	SetDlgItemText( hDlg, IDC_STATIC_MODULENAME, obj_modname->ex.pv.pvalue );
			//}
			break;
    }

    return FALSE;
}
