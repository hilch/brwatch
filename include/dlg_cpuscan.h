#ifndef BRWATCH_DLG_CPU_SCAN_H
#define BRWATCH_DLG_CPU_SCAN_H

#include <windows.h>
#include "cpusearch.h"

struct CpuScanDialogData {
	unsigned hosts = CpuScanDefaultHosts;
	bool allowBeyondSubnet = false;
	std::vector<stAdapterInfo> adapters;
};

INT_PTR CALLBACK CpuScanDlgProc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam);
bool DlgCpuScanShowDialog(HWND parent, unsigned &hosts, bool &allowBeyondSubnet);

#endif
