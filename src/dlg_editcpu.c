#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "pvi_interface.h"
#include "resource.h"
#include "cpusearch.h"
#include "dlg_editcpu.h"
#include "stringtools.h"

static LRESULT CALLBACK  DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static PVIOBJECT *object;

	switch (uMsg) {
		case WM_INITDIALOG:
			if( lParam != 0 ) {
				char tempstring[512];
				object = (PVIOBJECT *) lParam;
				sprintf( tempstring, "%s %s %s", object->ex.cpu.cputype, object->ex.cpu.arversion, object->descriptor );
				SetDlgItemText( hDlg, IDC_STATIC_CPU, tempstring );
				sprintf( tempstring, "%s", object->ex.cpu.ethernetCpuInfo.macAddress );
				SetDlgItemText( hDlg, IDR_MAC_ADDRESS, tempstring );
				SetDlgItemText( hDlg, IDR_STATIC_CPU_STATUS, object->ex.cpu.ethernetCpuInfo.arState );
				sprintf( tempstring, "%s", object->ex.cpu.ethernetCpuInfo.ipAddress );
				SetDlgItemText( hDlg, IDR_EDIT_IPADDRESS, tempstring );
				sprintf( tempstring, "%s", object->ex.cpu.ethernetCpuInfo.subnetMask );
				SetDlgItemText( hDlg, IDR_EDIT_SUBNETMASK, tempstring );
				sprintf( tempstring, "%s", object->ex.cpu.ethernetCpuInfo.gateway );
				SetDlgItemText( hDlg, IDR_EDIT_GATEWAY, tempstring );				
				sprintf( tempstring, "INA-node:%u  INA-port:%u", object->ex.cpu.ethernetCpuInfo.INA_nodeNumber, object->ex.cpu.ethernetCpuInfo.INA_portNumber );
				SetDlgItemText( hDlg, IDR_INA_SETTINGS, tempstring );

				SendMessage( GetDlgItem( hDlg, IDR_CHECK_DHCP_CLIENT ), BM_SETCHECK, object->ex.cpu.ethernetCpuInfo.ipMethod ? BST_CHECKED : BST_UNCHECKED, 0 );
				SendMessage( GetDlgItem( hDlg, IDR_INA_ACTIVATED ), BM_SETCHECK, object->ex.cpu.ethernetCpuInfo.INA_activated ? BST_CHECKED : BST_UNCHECKED, 0 );

				int haveMAC = strlen( object->ex.cpu.ethernetCpuInfo.macAddress );
				EnableWindow( GetDlgItem(hDlg, IDR_CHECK_DHCP_CLIENT ), haveMAC );

				object->ex.cpu.ethernetCpuInfo.ipMethod = Button_GetCheck(GetDlgItem(hDlg, IDR_CHECK_DHCP_CLIENT ));
				if( object->ex.cpu.ethernetCpuInfo.ipMethod || (haveMAC == 0) ) {
					EnableWindow( GetDlgItem(hDlg, IDR_EDIT_IPADDRESS), FALSE );
					EnableWindow( GetDlgItem(hDlg, IDR_EDIT_SUBNETMASK), FALSE );
					EnableWindow( GetDlgItem(hDlg, IDR_EDIT_GATEWAY), FALSE );
				} else {
					EnableWindow( GetDlgItem(hDlg, IDR_EDIT_IPADDRESS), TRUE );
					EnableWindow( GetDlgItem(hDlg, IDR_EDIT_SUBNETMASK), TRUE );
					EnableWindow( GetDlgItem(hDlg, IDR_EDIT_GATEWAY), TRUE );					
				}
				EnableWindow( GetDlgItem(hDlg, IDR_BUTTON_WARMSTART), object->error == 0 );
				EnableWindow( GetDlgItem(hDlg, IDR_BUTTON_COLDSTART), object->error == 0 );
				EnableWindow( GetDlgItem(hDlg, IDR_BUTTON_STOP), object->error == 0 );
				EnableWindow( GetDlgItem(hDlg, IDR_BUTTON_DIAGNOSIS), object->error == 0 );

				EnableWindow( GetDlgItem(hDlg, IDR_INA_ACTIVATED), FALSE );


				return TRUE;
			} else
				object = NULL;
			return FALSE;



		case WM_COMMAND:

			switch (LOWORD(wParam)) {

				case IDR_BUTTON_CHANGEIP:
					if(  object != NULL ) {
						struct stEthernetCpuInfo cpuInfo;
						int result;
						GetDlgItemText( hDlg, IDR_EDIT_IPADDRESS, cpuInfo.ipAddress, sizeof(cpuInfo.ipAddress) );
						GetDlgItemText( hDlg, IDR_EDIT_SUBNETMASK, cpuInfo.subnetMask, sizeof(cpuInfo.subnetMask) );
						GetDlgItemText( hDlg, IDR_EDIT_GATEWAY, cpuInfo.gateway, sizeof(cpuInfo.gateway) );						
						cpuInfo.ipMethod = Button_GetCheck(GetDlgItem(hDlg, IDR_CHECK_DHCP_CLIENT ));
						strcpy( cpuInfo.macAddress, object->ex.cpu.ethernetCpuInfo.macAddress );
						if( validate_ip4(cpuInfo.ipAddress) && validate_ip4(cpuInfo.subnetMask)) {
							result = SetCPUIpParameters( &cpuInfo );
							if( result == 1 ) {
								EndDialog(hDlg, TRUE );
								return TRUE;
							}
						}
					}
					return FALSE;

				case IDR_BUTTON_STOP:
					if(  object != NULL ) {
						char command[] = {"ST=Reset"};
						PviWrite( object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0 ) ;
						EndDialog(hDlg, TRUE );
						return TRUE;
					}
					return FALSE;


				case IDR_BUTTON_WARMSTART:
					if(  object != NULL ) {
						char command[] = {"ST=Warmstart"};
						PviWrite( object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0 ) ;
						EndDialog(hDlg, TRUE );
						return TRUE;
					}
					return FALSE;


				case IDR_BUTTON_COLDSTART:
					if(  object != NULL ) {
						char command[] = {"ST=Coldstart"};
						PviWrite( object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0 ) ;
						EndDialog(hDlg, TRUE );
						return TRUE;
					}
					return FALSE;


				case IDR_BUTTON_DIAGNOSIS:
					if(  object != NULL ) {
						char command[] = {"ST=Diagnose"};
						PviWrite( object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0 ) ;
						EndDialog(hDlg, TRUE );
						return TRUE;
					}
					return FALSE;

				case IDR_CHECK_DHCP_CLIENT:
					if(  object != NULL ) {
						HWND hwndButton = (HWND) lParam;
						if( HIWORD(wParam) == BN_CLICKED ) {
							if( Button_GetCheck(hwndButton) ) {
								EnableWindow( GetDlgItem(hDlg, IDR_EDIT_IPADDRESS), FALSE );
								EnableWindow( GetDlgItem(hDlg, IDR_EDIT_SUBNETMASK), FALSE );
								EnableWindow( GetDlgItem(hDlg, IDR_EDIT_GATEWAY), FALSE );								
								object->ex.cpu.ethernetCpuInfo.ipMethod = 1;
							} else {
								EnableWindow( GetDlgItem(hDlg, IDR_EDIT_IPADDRESS), TRUE );
								EnableWindow( GetDlgItem(hDlg, IDR_EDIT_SUBNETMASK), TRUE );
								EnableWindow( GetDlgItem(hDlg, IDR_EDIT_GATEWAY), TRUE );								
								object->ex.cpu.ethernetCpuInfo.ipMethod = 0;
							}
						}
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




void DlgEditCpuShowDialog( PVIOBJECT *object ) {
	if( object->type == POBJ_CPU ) {
		DialogBoxParam( g_hInstance, MAKEINTRESOURCE(DLG_EDIT_CPU), g_hwndMainWindow,
		                (DLGPROC) DlgProc, (LPARAM) object );
	}
}



/* -----------------------------------------------------------------------------------------
    PVI Callback for SNMP line
    ----------------------------------------------------------------------------------------
*/
static void WINAPI PviSnmpProc (WPARAM wParam, LPARAM lParam, LPVOID pData, DWORD DataLen, T_RESPONSE_INFO* pInfo) {

}


/* -----------------------------------------------------------------------------------------
    set ip parameters
    ----------------------------------------------------------------------------------------
*/

int SetCPUIpParameters( struct stEthernetCpuInfo *cpuInfo ) {
	DWORD result;
	DWORD linkIdSnmpLine;
	DWORD linkIdDevice;
	DWORD linkIdStation;
	int error = 0;

	result = PviCreate( &linkIdSnmpLine, "@Pvi/LnSNMP", POBJ_LINE, "CD=\"LNSNMP\"", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
	if( result == 0 ) {
		result = PviCreate( &linkIdDevice, "@Pvi/LnSNMP/Device", POBJ_DEVICE, "CD=\"/IF=snmp /RT=3000\"", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
		if( result == 0 ) {
			char tempstring[256];
			sprintf( tempstring, "CD=\"/CN=%s\"", cpuInfo->macAddress );
			result = PviCreate( &linkIdStation, "@Pvi/LnSNMP/Device/Station", POBJ_STATION, tempstring, PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
			if( result == 0 ) {
				DWORD linkIdPvar;
				result = PviCreate( &linkIdPvar, "@Pvi/LnSNMP/Device/Station/ipMethod", POBJ_PVAR, "CD=ipMethod", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
				result = PviWrite( linkIdPvar, POBJ_ACC_DATA, (void*) &cpuInfo->ipMethod, sizeof(cpuInfo->ipMethod), NULL, 0);
				if( result != 0 )
					error = 1;
				PviUnlink( linkIdPvar);
				if( cpuInfo->ipMethod == 0 ) { /* fixed IP ? */
					/* set gateway */
					result = PviCreate( &linkIdPvar, "@Pvi/LnSNMP/Device/Station/Gateway", POBJ_PVAR, "CD=defaultGateway", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
					result = PviWrite( linkIdPvar, POBJ_ACC_DATA, (void*) cpuInfo->gateway, strlen(cpuInfo->gateway), NULL, 0);
					if( result != 0 )
						error = 1;
					PviUnlink( linkIdPvar);
									
					/* set subnet mask */
					result = PviCreate( &linkIdPvar, "@Pvi/LnSNMP/Device/Station/Subnet", POBJ_PVAR, "CD=subnetMask", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
					result = PviWrite( linkIdPvar, POBJ_ACC_DATA, (void*) cpuInfo->subnetMask, strlen(cpuInfo->subnetMask), NULL, 0);
					if( result != 0 )
						error = 1;
					PviUnlink( linkIdPvar);

					/* set ip address */
					result = PviCreate( &linkIdPvar, "@Pvi/LnSNMP/Device/Station/Ip", POBJ_PVAR, "CD=ipAddress", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds" );
					result = PviWrite( linkIdPvar, POBJ_ACC_DATA, (void*) cpuInfo->ipAddress, strlen(cpuInfo->ipAddress), NULL, 0);
					if( result != 0 )
						error = 1;
					PviUnlink( linkIdPvar);
				}
				PviUnlink(linkIdStation);
			} else
				error = 1;
			PviUnlink( linkIdDevice );
		} else
			error = 1;
		result = PviUnlink(linkIdSnmpLine);
	} else
		error = 1;

	return( !error );
}



