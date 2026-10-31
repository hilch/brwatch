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

#ifndef BRWATCH_WATCH_LIST_VIEW_H
#define BRWATCH_WATCH_LIST_VIEW_H

#define WIN32_LEAN_AND_MEAN /* speed up compilations */
#include <windows.h>
#include <windowsx.h>
#ifndef _WIN32_IE
	#define _WIN32_IE 0x0401
#endif
#include <commctrl.h>
#include "pvi_interface.h"
class WatchListView {
public:
	WatchListView() = default;
	WatchListView(const WatchListView &) = delete;
	WatchListView &operator=(const WatchListView &) = delete;

	HWND Create(HINSTANCE instance, HWND parent);
	HWND GetHandle() const { return window; }
	LRESULT HandleMessages(UINT message, WPARAM wParam, LPARAM lParam);
	void InsertPVIObjects(PviObject *object);
	void UpdateValue(PviObject *object);
	void UpdateMenu(HMENU menu) const;
	void EditSelected();
	void RemoveSelected();
	void RemoveObject(PviObject *object);

private:
	PviObject *GetItemObject(int index) const;
	bool CanModify() const;
	bool CanEdit(const PviObject *object) const;
	void EditItem(int index);
	bool RemoveItem(int index);
	void SetItemText(int index, int subItem, char *text);
	void Keydown(WORD key);
	void DblClick(LPNMLISTVIEW notification);
	void RClick(LPNMLISTVIEW notification);
	void Activate(LPNMLISTVIEW notification);
	void BeginDragging(int index);

	HWND window = nullptr;
	HWND parentWindow = nullptr;
};

extern WatchListView watchListView;

#endif
