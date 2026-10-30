#ifndef BRWATCH_OBJECT_TREE_VIEW_H
#define BRWATCH_OBJECT_TREE_VIEW_H

#define WIN32_LEAN_AND_MEAN /* speed up compilations */
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

class PviObject;

class ObjectTreeView {
public:
	ObjectTreeView() = default;
	ObjectTreeView(const ObjectTreeView &) = delete;
	ObjectTreeView &operator=(const ObjectTreeView &) = delete;

	HWND Create(HINSTANCE instance, HWND parent);
	HWND GetHandle() const { return window; }
	HTREEITEM AddItem(HTREEITEM parent, LPSTR text, LPARAM id, int iconIndex);
	void ClearAll();
	LRESULT HandleMessages(UINT message, WPARAM wParam, LPARAM lParam);
	void UpdateCpuSettingsMenu(HMENU menu);
	void CpuSettingsCommand(UINT command);

private:
	void InsertChildObjects(HTREEITEM item, PviObject *parent);
	void ItemSelected(HTREEITEM item, LPARAM data);
	void BeginDragging(HTREEITEM item, LPARAM data);
	void DblClick(HTREEITEM item, LPARAM data);
	void RClick(HTREEITEM item, LPARAM data);
	PviObject *GetSelectedCpu() const;
	void UpdateCpuSettings(PviObject *object, UINT command);

	HWND window = nullptr;
	HWND parentWindow = nullptr;
};

extern ObjectTreeView objectTreeView;

#endif
