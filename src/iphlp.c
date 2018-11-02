
#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include "main.h"
#include <Iphlpapi.h>
#include <Winsock2.h>



LRESULT CALLBACK EthAdaptersDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LVCOLUMN lvcolumn;
    LVITEM lvitem;
    POINT pt;
    //
    static HWND hwndListView;
    TEXTMETRIC  tm ;
    int	cxChar, cxCaps;						// Font- Abmessungen
    HDC hdc;
    HCURSOR holdcursor;
    //
    IP_ADAPTER_INFO *buffer;
    DWORD bufsize;
    IP_ADAPTER_INFO *next;
    IP_ADDR_STRING *ipaddr;
    DWORD result;
    //
    unsigned long ip_address, mask;
    static unsigned long *pbroadcast;
    //
    int i;


    switch (uMsg)
    {
    case WM_INITDIALOG:
        SetWindowText( hDlg, "Search CPU on the following network adapters" );
        holdcursor = SetCursor(LoadCursor(NULL, IDC_WAIT));

        pbroadcast = (unsigned long*) lParam;
        /* Font- Abmessungen holen */
        hdc = GetDC( hDlg );
        GetTextMetrics (hdc, &tm) ;
        ReleaseDC( hDlg, hdc );
        cxChar = tm.tmAveCharWidth ;
        cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2 ;
//			cyChar = tm.tmHeight + tm.tmExternalLeading ;
        /* Window- Handle des ListView */
        hwndListView = GetDlgItem( hDlg, IDC_LIST );
        /* Spalte 0 */
        lvcolumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        lvcolumn.fmt = LVCFMT_LEFT;
        lvcolumn.cx = cxCaps*15;
        lvcolumn.pszText = "IP-Address";
        ListView_InsertColumn(hwndListView, 0, &lvcolumn);
        /* Spalte 1 */
        lvcolumn.cx = cxCaps*15;
        lvcolumn.pszText = "Mask";
        ListView_InsertColumn(hwndListView, 1, &lvcolumn);
        /* Spalte 2 */
        lvcolumn.cx = cxCaps*20;
        lvcolumn.pszText = "Name";
        ListView_InsertColumn(hwndListView, 2, &lvcolumn);
        /* Spalte 3 */
        lvcolumn.cx = cxCaps*15;
        lvcolumn.pszText = "Type";
        ListView_InsertColumn(hwndListView, 3, &lvcolumn);
        ListView_DeleteAllItems(hwndListView);


        // Adapter auflisten
        bufsize = 20 * sizeof(IP_ADAPTER_INFO);
        buffer = malloc( bufsize );

        if( buffer != NULL )
        {

            result = GetAdaptersInfo( buffer, &bufsize );

            if( result == ERROR_SUCCESS )
                next = buffer;
            else
                next = NULL;

            i = 0;
            while( next != NULL )
            {

                ipaddr = &next->IpAddressList;

                while( ipaddr != NULL )
                {
                    memset(&lvitem, 0, sizeof(lvitem));
                    lvitem.mask = LVIF_TEXT;
                    lvitem.iItem = i;
                    lvitem.iSubItem = 0;
                    lvitem.state = LVIS_FOCUSED;
                    lvitem.stateMask = (UINT) - 1;
                    lvitem.pszText = ipaddr->IpAddress.String;
                    lvitem.cchTextMax = 0;
                    ListView_InsertItem(hwndListView, &lvitem);
                    // Mask
                    ListView_SetItemText(hwndListView, i, 1, ipaddr->IpMask.String );
                    // Name
                    ListView_SetItemText(hwndListView, i, 2, next->Description );
                    // Type
                    switch( next->Type )
                    {
                    case MIB_IF_TYPE_ETHERNET:
                        ListView_SetItemText(hwndListView, i, 3, _T("Ethernet") );
                        break;
                    case MIB_IF_TYPE_LOOPBACK:
                        ListView_SetItemText(hwndListView, i, 3, _T("Loopback") );
                        break;
                    case MIB_IF_TYPE_SLIP:
                    case MIB_IF_TYPE_PPP:
                        ListView_SetItemText(hwndListView, i, 3, _T("SLIP/PPP") );
                        break;
                    default:
                        ListView_SetItemText(hwndListView, i, 3, _T("Other") );
                        break;
                    }
                    ipaddr = ipaddr->Next;
                    ++i;
                }
                next = next->Next;
            }

            free(buffer);
        }
        SetCursor( holdcursor );
        GetCursorPos( &pt );
        SetWindowPos( hDlg, HWND_TOP, pt.x, pt.y, 0, 0, SWP_NOSIZE );
        return TRUE;



    case WM_COMMAND:
        switch (wParam)
        {
        case IDOK:
            i = ListView_GetNextItem( hwndListView, -1, LVNI_SELECTED );
            if( i != -1 )
            {
                char tempstring[20];

                memset(&lvitem, 0, sizeof(lvitem) );
                lvitem.iItem = i;
                lvitem.mask = LVIF_PARAM;
                ListView_GetItemText( hwndListView,i, 0, tempstring, sizeof(tempstring) );
                ip_address = inet_addr(tempstring);
                ListView_GetItemText( hwndListView,i, 1, tempstring, sizeof(tempstring) );
                mask = inet_addr(tempstring);
                *pbroadcast = (ip_address & mask) | ~mask; // Netzwerkadresse
                EndDialog(hDlg, TRUE);
            }
            else
            {
                MessageBox( hDlg, "Select one adapter, please !", "No network adapter selected", MB_OK |MB_ICONASTERISK );
            }
            return TRUE;


        case IDCANCEL:
            /*
             * OK or Cancel was clicked, close the dialog.
             */
            *pbroadcast = 0;
            EndDialog(hDlg, TRUE);
            return TRUE;
        }
        break;
    }

    return FALSE;
}


