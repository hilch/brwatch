/*
 * brwatch - PVI diagnosis tool for B&R PLCs
 *
 * Repository: https://github.com/hilch/brwatch
 *
 * SPDX-License-Identifier: GPL-3.0-only
 * Licensed under the GNU General Public License version 3.
 * See LICENSE for details.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#define WIN32_LEAN_AND_MEAN /* speed up compilations */
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

static INT_PTR CALLBACK AddCpuDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
	{
		SetWindowLongPtr(hDlg, DWLP_USER, lParam);
		SendDlgItemMessageA(hDlg, IDR_EDIT_IPADDRESS, EM_SETLIMITTEXT, 15, 0);
		SendDlgItemMessageA(hDlg, IDC_EDIT_CPU_TIMEOUT, EM_SETLIMITTEXT, 6, 0);
		SetDlgItemInt(hDlg, IDC_EDIT_CPU_TIMEOUT, 2500, FALSE);
		return TRUE;
	}
	if (uMsg == WM_COMMAND)
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			char ipAddress[17];
			unsigned octets[4];
			char trailing;
			GetDlgItemTextA(hDlg, IDR_EDIT_IPADDRESS, ipAddress, sizeof(ipAddress));
			if (strspn(ipAddress, "0123456789.") != strlen(ipAddress) ||
				sscanf(ipAddress, "%u.%u.%u.%u%c", &octets[0], &octets[1], &octets[2], &octets[3], &trailing) != 4 ||
				octets[0] > 255 || octets[1] > 255 || octets[2] > 255 || octets[3] > 255)
			{
				MessageBoxA(hDlg, "Enter a valid IPv4 address.", "Add Cpu", MB_OK | MB_ICONWARNING);
				SetFocus(GetDlgItem(hDlg, IDR_EDIT_IPADDRESS));
				return TRUE;
			}
			snprintf(ipAddress, sizeof(ipAddress), "%u.%u.%u.%u", octets[0], octets[1], octets[2], octets[3]);
			char timeoutText[32];
			GetDlgItemTextA(hDlg, IDC_EDIT_CPU_TIMEOUT, timeoutText, sizeof(timeoutText));
			BOOL validTimeout = FALSE;
			UINT timeoutMs = GetDlgItemInt(hDlg, IDC_EDIT_CPU_TIMEOUT, &validTimeout, FALSE);
			if (!validTimeout || strspn(timeoutText, "0123456789") != strlen(timeoutText) ||
				timeoutMs < 500 || timeoutMs > 360000)
			{
				MessageBoxA(hDlg, "Enter a timeout from 500 to 360000 ms.", "Add Cpu", MB_OK | MB_ICONWARNING);
				SetFocus(GetDlgItem(hDlg, IDC_EDIT_CPU_TIMEOUT));
				return TRUE;
			}
			PviObject *cpuObject = PviObject::AddCpuByIp(ipAddress, timeoutMs);
			if (cpuObject != nullptr)
			{
				PviObject **result = reinterpret_cast<PviObject **>(GetWindowLongPtr(hDlg, DWLP_USER));
				*result = cpuObject;
				EndDialog(hDlg, IDOK);
			}
			return TRUE;
		}
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
	}
	return FALSE;
}

PviObject *DlgAddCpuShowDialog(void)
{
	PviObject *cpuObject = nullptr;
	DialogBoxParamA(g_hInstance, MAKEINTRESOURCEA(DLG_ADD_CPU), g_hwndMainWindow,
		AddCpuDlgProc, reinterpret_cast<LPARAM>(&cpuObject));
	return cpuObject;
}

static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static PviObject *object;

	switch (uMsg)
	{
	case WM_UPDATEUISTATE:
		// Keep shortcut underlines visible when Windows reinitializes dialog UI state.
		DefWindowProc(hDlg, uMsg, MAKEWPARAM(LOWORD(wParam), HIWORD(wParam) & ~UISF_HIDEACCEL), lParam);
		DefWindowProc(hDlg, uMsg, MAKEWPARAM(UIS_CLEAR, UISF_HIDEACCEL), 0);
		return TRUE;
	case WM_INITDIALOG:
		if (lParam != 0)
		{
			SendMessage(hDlg, WM_UPDATEUISTATE, MAKEWPARAM(UIS_CLEAR, UISF_HIDEACCEL), 0);
			char tempstring[512];
			object = (PviObject *)lParam;
			sprintf(tempstring, "%s %s %s", object->ex.cpu.cputype, object->ex.cpu.arversion, object->descriptor);
			SetDlgItemTextA(hDlg, IDC_STATIC_CPU, tempstring);
			if (strlen(object->ex.cpu.ethernetCpuInfo.macAddress) == 0)
				strcpy(tempstring, "00-00-00-00-00-00-00");
			else
				strcpy(tempstring, object->ex.cpu.ethernetCpuInfo.macAddress);
			SetDlgItemTextA(hDlg, IDR_MAC_ADDRESS, tempstring);

			if (strlen(object->ex.cpu.ethernetCpuInfo.arState) == 0)
				strcpy(tempstring, object->ex.cpu.status);
			else
				strcpy(tempstring, object->ex.cpu.ethernetCpuInfo.arState);
			SetDlgItemTextA(hDlg, IDR_STATIC_CPU_STATUS, tempstring);

			if (object->ex.cpu.ethernetCpuInfo.SNMP_mode == 0)
				strcpy(tempstring, "deactivated");
			else if (object->ex.cpu.ethernetCpuInfo.SNMP_mode == 1)
				strcpy(tempstring, "read only");
			else if (object->ex.cpu.ethernetCpuInfo.SNMP_mode == 2)
				strcpy(tempstring, "activated");
			SetDlgItemTextA(hDlg, IDR_STATIC_SNMP_MODE, tempstring);

			sprintf(tempstring, "%s", object->ex.cpu.ethernetCpuInfo.ipAddress);
			SetDlgItemTextA(hDlg, IDR_EDIT_IPADDRESS, tempstring);
			sprintf(tempstring, "%s", object->ex.cpu.ethernetCpuInfo.subnetMask);
			SetDlgItemTextA(hDlg, IDR_EDIT_SUBNETMASK, tempstring);
			sprintf(tempstring, "%s", object->ex.cpu.ethernetCpuInfo.gateway);
			SetDlgItemTextA(hDlg, IDR_EDIT_GATEWAY, tempstring);
			sprintf(tempstring, "INA-node:%u  INA-port:%u", object->ex.cpu.ethernetCpuInfo.INA_nodeNumber, object->ex.cpu.ethernetCpuInfo.INA_portNumber);
			SetDlgItemTextA(hDlg, IDR_INA_SETTINGS, tempstring);

			SendMessage(GetDlgItem(hDlg, IDR_CHECK_DHCP_CLIENT), BM_SETCHECK, object->ex.cpu.ethernetCpuInfo.ipMethod ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hDlg, IDR_INA_ACTIVATED), BM_SETCHECK, object->ex.cpu.ethernetCpuInfo.INA_activated ? BST_CHECKED : BST_UNCHECKED, 0);

			int haveMAC = strlen(object->ex.cpu.ethernetCpuInfo.macAddress);
			EnableWindow(GetDlgItem(hDlg, IDR_CHECK_DHCP_CLIENT), haveMAC);

			object->ex.cpu.ethernetCpuInfo.ipMethod = Button_GetCheck(GetDlgItem(hDlg, IDR_CHECK_DHCP_CLIENT));
			if (object->ex.cpu.ethernetCpuInfo.ipMethod || (haveMAC == 0))
			{
				EnableWindow(GetDlgItem(hDlg, IDR_EDIT_IPADDRESS), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDR_EDIT_SUBNETMASK), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDR_EDIT_GATEWAY), FALSE);
			}
			else
			{
				EnableWindow(GetDlgItem(hDlg, IDR_EDIT_IPADDRESS), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDR_EDIT_SUBNETMASK), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDR_EDIT_GATEWAY), TRUE);
			}
			EnableWindow(GetDlgItem(hDlg, IDR_BUTTON_WARMSTART), object->error == 0);
			EnableWindow(GetDlgItem(hDlg, IDR_BUTTON_COLDSTART), object->error == 0);
			EnableWindow(GetDlgItem(hDlg, IDR_BUTTON_STOP), object->error == 0);
			EnableWindow(GetDlgItem(hDlg, IDR_BUTTON_DIAGNOSIS), object->error == 0);
			EnableWindow(GetDlgItem(hDlg, IDR_INA_ACTIVATED), FALSE);

			return TRUE;
		}
		else
			object = nullptr;
		return FALSE;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{

		case IDR_BUTTON_CHANGEIP:
			if (object != nullptr)
			{
				struct stEthernetCpuInfo cpuInfo;
				int result;
				GetDlgItemTextA(hDlg, IDR_EDIT_IPADDRESS, cpuInfo.ipAddress, sizeof(cpuInfo.ipAddress));
				GetDlgItemTextA(hDlg, IDR_EDIT_SUBNETMASK, cpuInfo.subnetMask, sizeof(cpuInfo.subnetMask));
				GetDlgItemTextA(hDlg, IDR_EDIT_GATEWAY, cpuInfo.gateway, sizeof(cpuInfo.gateway));
				cpuInfo.ipMethod = Button_GetCheck(GetDlgItem(hDlg, IDR_CHECK_DHCP_CLIENT));
				strcpy(cpuInfo.macAddress, object->ex.cpu.ethernetCpuInfo.macAddress);
				if (validate_ip4(cpuInfo.ipAddress) && validate_ip4(cpuInfo.subnetMask))
				{
					result = SetCPUIpParameters(&cpuInfo);
					if (result == 1)
					{
						EndDialog(hDlg, TRUE);
						return TRUE;
					}
				}
			}
			return FALSE;

		case IDR_BUTTON_STOP:
			if (object != nullptr)
			{
				char command[] = {"ST=Reset"};
				PviWrite(object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0);
				EndDialog(hDlg, TRUE);
				return TRUE;
			}
			return FALSE;

		case IDR_BUTTON_WARMSTART:
			if (object != nullptr)
			{
				char command[] = {"ST=Warmstart"};
				PviWrite(object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0);
				EndDialog(hDlg, TRUE);
				return TRUE;
			}
			return FALSE;

		case IDR_BUTTON_COLDSTART:
			if (object != nullptr)
			{
				char command[] = {"ST=Coldstart"};
				PviWrite(object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0);
				EndDialog(hDlg, TRUE);
				return TRUE;
			}
			return FALSE;

		case IDR_BUTTON_DIAGNOSIS:
			if (object != nullptr)
			{
				char command[] = {"ST=Diagnose"};
				PviWrite(object->linkid, POBJ_ACC_STATUS, command, strlen(command), 0, 0);
				EndDialog(hDlg, TRUE);
				return TRUE;
			}
			return FALSE;

		case IDR_CHECK_DHCP_CLIENT:
			if (object != nullptr)
			{
				HWND hwndButton = (HWND)lParam;
				if (HIWORD(wParam) == BN_CLICKED)
				{
					if (Button_GetCheck(hwndButton))
					{
						EnableWindow(GetDlgItem(hDlg, IDR_EDIT_IPADDRESS), FALSE);
						EnableWindow(GetDlgItem(hDlg, IDR_EDIT_SUBNETMASK), FALSE);
						EnableWindow(GetDlgItem(hDlg, IDR_EDIT_GATEWAY), FALSE);
						object->ex.cpu.ethernetCpuInfo.ipMethod = 1;
					}
					else
					{
						EnableWindow(GetDlgItem(hDlg, IDR_EDIT_IPADDRESS), TRUE);
						EnableWindow(GetDlgItem(hDlg, IDR_EDIT_SUBNETMASK), TRUE);
						EnableWindow(GetDlgItem(hDlg, IDR_EDIT_GATEWAY), TRUE);
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

void DlgEditCpuShowDialog(PviObject *object)
{
	if (object->type == POBJ_CPU)
	{
		DialogBoxParam(g_hInstance, MAKEINTRESOURCE(DLG_EDIT_CPU), g_hwndMainWindow,
					   DlgProc, (LPARAM)object);
	}
}

/* -----------------------------------------------------------------------------------------
	PVI Callback for SNMP line
	----------------------------------------------------------------------------------------
*/
static void WINAPI PviSnmpProc(WPARAM /* wParam */, LPARAM /*lParam*/, LPVOID /* pData */, DWORD /* DataLen */, T_RESPONSE_INFO * /* pInfo */)
{
}

/* -----------------------------------------------------------------------------------------
	set ip parameters
	----------------------------------------------------------------------------------------
*/

int SetCPUIpParameters(struct stEthernetCpuInfo *cpuInfo)
{
	DWORD result;
	DWORD linkIdSnmpLine;
	DWORD linkIdDevice;
	DWORD linkIdStation;
	int error = 0;

	result = PviCreate(&linkIdSnmpLine, "@Pvi/LnSNMP", POBJ_LINE, "CD=\"LNSNMP\"", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds");
	if (result == 0)
	{
		result = PviCreate(&linkIdDevice, "@Pvi/LnSNMP/Device", POBJ_DEVICE, "CD=\"/IF=snmp /RT=3000\"", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds");
		if (result == 0)
		{
			char tempstring[256];
			sprintf(tempstring, "CD=\"/CN=%s\"", cpuInfo->macAddress);
			result = PviCreate(&linkIdStation, "@Pvi/LnSNMP/Device/Station", POBJ_STATION, tempstring, PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds");
			if (result == 0)
			{
				DWORD linkIdPvar;
				result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/ipMethod", POBJ_PVAR, "CD=ipMethod", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds");
				result = PviWrite(linkIdPvar, POBJ_ACC_DATA, (void *)&cpuInfo->ipMethod, sizeof(cpuInfo->ipMethod), nullptr, 0);
				if (result != 0)
					error = 1;
				PviUnlink(linkIdPvar);
				if (cpuInfo->ipMethod == 0)
				{ /* fixed IP ? */
					/* set gateway */
					result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/Gateway", POBJ_PVAR, "CD=defaultGateway", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds");
					result = PviWrite(linkIdPvar, POBJ_ACC_DATA, (void *)cpuInfo->gateway, strlen(cpuInfo->gateway), nullptr, 0);
					if (result != 0)
						error = 1;
					PviUnlink(linkIdPvar);

					/* set subnet mask */
					result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/Subnet", POBJ_PVAR, "CD=subnetMask", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds");
					result = PviWrite(linkIdPvar, POBJ_ACC_DATA, (void *)cpuInfo->subnetMask, strlen(cpuInfo->subnetMask), nullptr, 0);
					if (result != 0)
						error = 1;
					PviUnlink(linkIdPvar);

					/* set ip address */
					result = PviCreate(&linkIdPvar, "@Pvi/LnSNMP/Device/Station/Ip", POBJ_PVAR, "CD=ipAddress", PviSnmpProc, SET_PVICALLBACK_DATA, 0, "Ev=eds");
					result = PviWrite(linkIdPvar, POBJ_ACC_DATA, (void *)cpuInfo->ipAddress, strlen(cpuInfo->ipAddress), nullptr, 0);
					if (result != 0)
						error = 1;
					PviUnlink(linkIdPvar);
				}
				PviUnlink(linkIdStation);
			}
			else
				error = 1;
			PviUnlink(linkIdDevice);
		}
		else
			error = 1;
		result = PviUnlink(linkIdSnmpLine);
	}
	else
		error = 1;

	return (!error);
}
