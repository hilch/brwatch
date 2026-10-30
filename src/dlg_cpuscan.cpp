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

#define WIN32_LEAN_AND_MEAN
#include "main.h"
#include "dlg_cpuscan.h"
#include <string>

static const unsigned hostChoices[] = {256, 512, 1024, 2048, 16384};

static std::string AddressText(std::uint32_t address)
{
	char text[20];
	snprintf(text, sizeof(text), "%u.%u.%u.%u",
		(address >> 24) & 255, (address >> 16) & 255, (address >> 8) & 255, address & 255);
	return text;
}

static void UpdateScanPreview(HWND dialog, CpuScanDialogData &data)
{
	std::string details;
	std::uint64_t timeoutMs = 0;
	unsigned totalTargets = 0;
	for (const auto &adapter : data.adapters)
	{
		CpuPortScanPlan plan;
		int error;
		char row[400];
		if (!GetCpuPortScanPlan(adapter, data.hosts, plan, error, data.allowBeyondSubnet))
		{
			snprintf(row, sizeof(row), "ERROR: %s (%s / %s): invalid address or subnet mask (error %d).\r\n\r\n",
				adapter.description, adapter.ipAddress, adapter.subnetMask, error);
			details += row;
			continue;
		}
		std::uint64_t adapterTimeout = CpuPortScanTimeoutBudgetMs(plan);
		timeoutMs += adapterTimeout;
		totalTargets += plan.targetCount;
		snprintf(row, sizeof(row), "%s (%s / %s)\r\n",
			adapter.description, adapter.ipAddress, adapter.subnetMask);
		details += row;
		if (plan.targetCount != 0)
		{
			snprintf(row, sizeof(row), "%s - %s: %u TCP targets; timeout budget %.1f s.\r\n",
				AddressText(plan.firstAddress).c_str(), AddressText(plan.lastAddress).c_str(),
				plan.targetCount, adapterTimeout / 1000.0);
			details += row;
		}
		else
			details += "No other usable addresses in this subnet.\r\n";
		if (plan.limitedBySubnet)
			details += "WARNING: selected block exceeds this adapter's subnet; only in-subnet addresses will be scanned.\r\n";
		if (plan.extendsBeyondSubnet)
			details += "WARNING: scanning beyond this adapter's subnet requires a suitable route/gateway and firewall permissions.\r\n";
		details += "\r\n";
	}
	SetDlgItemTextA(dialog, IDC_CPU_SCAN_DETAILS, details.c_str());
	char timeout[160];
	snprintf(timeout, sizeof(timeout), "Maximum TCP timeout budget: %.1f seconds (%u targets, %u parallel connections, %u ms per batch).",
		timeoutMs / 1000.0, totalTargets, CpuScanBatchSize, CpuScanBatchTimeoutMs);
	SetDlgItemTextA(dialog, IDC_CPU_SCAN_TIMEOUT, timeout);
	EnableWindow(GetDlgItem(dialog, IDOK), totalTargets != 0);
}

INT_PTR CALLBACK CpuScanDlgProc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto data = reinterpret_cast<CpuScanDialogData *>(GetWindowLongPtrA(dialog, DWLP_USER));
	switch (message)
	{
	case WM_INITDIALOG:
	{
		data = reinterpret_cast<CpuScanDialogData *>(lParam);
		if (data == nullptr)
		{
			MessageBoxA(dialog, "Missing CPU scan configuration.", "CPU scan", MB_OK | MB_ICONERROR);
			EndDialog(dialog, IDCANCEL);
			return TRUE;
		}
		SetWindowLongPtrA(dialog, DWLP_USER, reinterpret_cast<LONG_PTR>(data));
		int selected = -1;
		for (unsigned i = 0; i < sizeof(hostChoices) / sizeof(hostChoices[0]); ++i)
		{
			char text[16];
			snprintf(text, sizeof(text), "%u", hostChoices[i]);
			LRESULT index = SendDlgItemMessageA(dialog, IDC_CPU_SCAN_HOSTS, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(text));
			if (index == CB_ERR || index == CB_ERRSPACE)
			{
				MessageBoxA(dialog, "Could not populate the CPU scan choices.", "CPU scan", MB_OK | MB_ICONERROR);
				EndDialog(dialog, IDCANCEL);
				return TRUE;
			}
			if (hostChoices[i] == data->hosts)
				selected = static_cast<int>(i);
		}
		if (selected < 0)
		{
			MessageBoxA(dialog, "Invalid CPU scan address-block size.", "CPU scan", MB_OK | MB_ICONERROR);
			EndDialog(dialog, IDCANCEL);
			return TRUE;
		}
		data->hosts = hostChoices[selected];
		SendDlgItemMessageA(dialog, IDC_CPU_SCAN_HOSTS, CB_SETCURSEL, selected, 0);
		CheckDlgButton(dialog, IDC_CPU_SCAN_BEYOND_SUBNET, data->allowBeyondSubnet ? BST_CHECKED : BST_UNCHECKED);
		UpdateScanPreview(dialog, *data);
		return TRUE;
	}
	case WM_COMMAND:
		if (data == nullptr)
			break;
		if (LOWORD(wParam) == IDC_CPU_SCAN_BEYOND_SUBNET && HIWORD(wParam) == BN_CLICKED)
		{
			data->allowBeyondSubnet = IsDlgButtonChecked(dialog, IDC_CPU_SCAN_BEYOND_SUBNET) == BST_CHECKED;
			UpdateScanPreview(dialog, *data);
			return TRUE;
		}
		if (LOWORD(wParam) == IDC_CPU_SCAN_HOSTS && HIWORD(wParam) == CBN_SELCHANGE)
		{
			LRESULT selection = SendDlgItemMessageA(dialog, IDC_CPU_SCAN_HOSTS, CB_GETCURSEL, 0, 0);
			if (selection >= 0 && selection < static_cast<LRESULT>(sizeof(hostChoices) / sizeof(hostChoices[0])))
			{
				data->hosts = hostChoices[selection];
				UpdateScanPreview(dialog, *data);
			}
			return TRUE;
		}
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(dialog, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

bool DlgCpuScanShowDialog(HWND parent, unsigned &hosts, bool &allowBeyondSubnet)
{
	CpuScanDialogData data;
	data.hosts = hosts;
	data.allowBeyondSubnet = allowBeyondSubnet;
	unsigned long error = ListCpuScanAdapters(data.adapters);
	if (error != ERROR_SUCCESS)
	{
		char message[160];
		snprintf(message, sizeof(message), "Could not enumerate Ethernet adapters (error %lu).", error);
		MessageBoxA(parent, message, "CPU scan", MB_OK | MB_ICONERROR);
		return false;
	}
	INT_PTR result = DialogBoxParamA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(DLG_CPU_SCAN),
		parent, CpuScanDlgProc, reinterpret_cast<LPARAM>(&data));
	if (result == -1)
	{
		char message[160];
		snprintf(message, sizeof(message), "Could not create the CPU scan dialog (error %lu).", GetLastError());
		MessageBoxA(parent, message, "CPU scan", MB_OK | MB_ICONERROR);
		return false;
	}
	if (result != IDOK)
		return false;
	hosts = data.hosts;
	allowBeyondSubnet = data.allowBeyondSubnet;
	return true;
}
