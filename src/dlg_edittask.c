
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
            sprintf( tempstring, object->descriptor );
            SetDlgItemText( hDlg, IDC_STATIC_TASK, tempstring );
            SetDlgItemText( hDlg, IDR_STATIC_TASK_STATUS, object->ex.task.status );
            SetDlgItemText( hDlg, IDR_EDIT_NUMBER_OF_CYCLES, "1");
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
                char command[] = {"ST=Stop"};
                PviWrite( object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0 ) ;
                return TRUE;
            }
            return FALSE;

        case IDR_BUTTON_RESUME:
            if(  object != NULL )
            {
                char command[100];
                sprintf( command, "%s", "ST=Cycle" );
                PviWrite( object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0 ) ;
                sprintf( command,"%s","ST=Resume");
                PviWrite( object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0 ) ;
                return TRUE;
            }
            return FALSE;


        case IDR_BUTTON_START:
            if(  object != NULL )
            {
                char tempstring[20];
                tempstring[0] = 0;
                GetDlgItemText( hDlg, IDR_EDIT_NUMBER_OF_CYCLES, tempstring, sizeof(tempstring) );
                char command[100];
                sprintf( command, "ST=Cycle(%s)", tempstring );
                PviWrite( object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0 ) ;
                sprintf( command, "%s", "ST=Start" );
                PviWrite( object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0 ) ;
                return TRUE;
            }
            return FALSE;



        case IDOK:
        case IDCANCEL:

            EndDialog(hDlg, TRUE);
            return TRUE;
        }
        break;
    }

    return FALSE;
}




void DlgEditTaskShowDialog( PVIOBJECT *object )
{
    if( object->type == POBJ_TASK )
    {
        if( object->error == 0 )
        {
            DialogBoxParam( g_hInstance, MAKEINTRESOURCE(DLG_EDIT_TASK), g_hwndMainWindow,
                            (DLGPROC) DlgProc, (LPARAM) object );
        }
    }
}
