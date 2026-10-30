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

#ifndef BRWATCH_OBJECT_TREE_VIEW_H
#define BRWATCH_OBJECT_TREE_VIEW_H

#define WIN32_LEAN_AND_MEAN /* speed up compilations */
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <memory>

class PviObject;

class ObjectTreeView {
public:
	ObjectTreeView();
	~ObjectTreeView();
	ObjectTreeView(const ObjectTreeView &) = delete;
	ObjectTreeView &operator=(const ObjectTreeView &) = delete;

	HWND Create(HINSTANCE instance, HWND parent);
	HWND GetHandle() const { return window; }
	HWND GetCpuScanDialogHandle() const;
	void StopCpuScan();
	HTREEITEM AddItem(HTREEITEM parent, LPSTR text, LPARAM id, int iconIndex);
	void ClearAll();
	LRESULT HandleMessages(UINT message, WPARAM wParam, LPARAM lParam);
	void UpdateCpuSettingsMenu(HMENU menu);
	void CpuSettingsCommand(UINT command);

private:
	struct CpuScanTask;
	static unsigned __stdcall CpuScanThread(void *data);
	static INT_PTR CALLBACK CpuScanProgressDlgProc(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam);
	void StartCpuScan(HTREEITEM item, PviObject *parent, unsigned hosts, bool allowBeyondSubnet);
	void FinishCpuScan();
	void UpdateCpuScanProgress();
	void SetCpuScanBusy(bool busy);
	void InsertExistingChildObjects(HTREEITEM item, PviObject *parent);
	void InsertChildObjects(HTREEITEM item, PviObject *parent, unsigned hostsPerAdapter, bool allowBeyondSubnet);
	void ItemSelected(HTREEITEM item, LPARAM data);
	void BeginDragging(HTREEITEM item, LPARAM data);
	void DblClick(HTREEITEM item, LPARAM data);
	void RClick(HTREEITEM item, LPARAM data);
	PviObject *GetSelectedCpu() const;
	void UpdateCpuSettings(PviObject *object, UINT command);

	HWND window = nullptr;
	HWND parentWindow = nullptr;
	bool cpuScanBusy = false;
	HCURSOR previousCursor = nullptr;
	std::unique_ptr<CpuScanTask> scanTask;
};

extern ObjectTreeView objectTreeView;

#endif
