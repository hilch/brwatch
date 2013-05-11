
#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>


#define MAX_MYTREEVIEW_LEVELS			4


typedef struct {
	HINSTANCE				hinstance;									// Instanz der Anwendung
	HWND					hwndParent;									// Handle des Elternfenster
	HWND					hwndTV;										// Handle auf Treeview-Fenster
	void 					(* cbselected)( HTREEITEM, LPARAM );		// Zeiger auf Callback "Element ausgewaehlt"
	void					(* cbbegindrag) (HTREEITEM, LPARAM );		// Zeiger auf Callback "Begin Dragging"
	void					(* cbdblclick) (HTREEITEM, LPARAM );		// Zeiger auf Callback "Doppel-click"
	void					(* cbrclick) (HTREEITEM, LPARAM );			// Zeiger auf Callback "Rechts-click"
} MYTREEVIEWPARAM;


HTREEITEM AddItemToMyTreeView(MYTREEVIEWPARAM* tv, HTREEITEM parent, LPSTR text, LPARAM id, int icon_index );
HWND CreateMyTreeView( MYTREEVIEWPARAM * );
LRESULT CALLBACK HandleMyTreeViewMessages(MYTREEVIEWPARAM* , UINT, WPARAM , LPARAM );
void ClearMyTreeView( MYTREEVIEWPARAM* );

#define HANDLE_MYTREEVIEW_MSG( param , wMsg, wParam, lParam ) 		if( HandleMyTreeViewMessages( param, wMsg, wParam, lParam ) == 0 ) return(0);




