#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "pvi_interface.h"
#include "resource.h"



static LRESULT CALLBACK  DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static PVIOBJECT *object;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        if( lParam != 0 )
        {
            char tempstring[256];
            object = (PVIOBJECT *) lParam;
            sprintf( tempstring, "%s %s %s", object->ex.cpu.cputype, object->ex.cpu.arversion, object->descriptor );
            SetDlgItemText( hDlg, IDC_STATIC_CPU, tempstring );
            SetDlgItemText( hDlg, IDR_STATIC_CPU_STATUS, object->ex.cpu.status );
            return TRUE;
        }
        else
            object = NULL;
        return FALSE;



    case WM_COMMAND:

        switch (wParam)
        {
         case IDR_BUTTON_STOP:
            if(  object != NULL )
            {
                char command[] = {"ST=Reset"};
                PviWrite( object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0 ) ;
                EndDialog(hDlg, TRUE );
                return TRUE;
            }
            return FALSE;


        case IDR_BUTTON_WARMSTART:
            if(  object != NULL )
            {
                char command[] = {"ST=Warmstart"};
                PviWrite( object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0 ) ;
                EndDialog(hDlg, TRUE );
                return TRUE;
            }
            return FALSE;


        case IDR_BUTTON_COLDSTART:
            if(  object != NULL )
            {
                char command[] = {"ST=Coldstart"};
                PviWrite( object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0 ) ;
                EndDialog(hDlg, TRUE );
                return TRUE;
            }
            return FALSE;


        case IDR_BUTTON_DIAGNOSIS:
            if(  object != NULL )
            {
                char command[] = {"ST=Diagnose"};
                PviWrite( object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0 ) ;
                EndDialog(hDlg, TRUE );
                return TRUE;
            }
            return FALSE;

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




void DlgEditCpuShowDialog( PVIOBJECT *object )
{
    if( object->type == POBJ_CPU )
    {
        if( object->error == 0 )
        {

            DialogBoxParam( g_hInstance, MAKEINTRESOURCE(DLG_EDIT_CPU), g_hwndMainWindow,
                            (DLGPROC) DlgProc, (LPARAM) object );
        }
    }
}
