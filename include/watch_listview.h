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

private:
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
