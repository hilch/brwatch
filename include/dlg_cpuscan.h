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
