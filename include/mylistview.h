
#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include <windows.h>
#include <windowsx.h>
#ifndef _WIN32_IE
	#define _WIN32_IE 0x0401
#endif
#include <commctrl.h>
#include "pvi_interface.h"
#define MAX_MYLISTVIEW_COLS		10

typedef struct {
	char 					*name;
	int						width;
} MYLISTVIEWCOLUMN;

typedef struct {
	HINSTANCE				hinstance;									// Instanz der Anwendung
	HWND					hwndParent;									// Handle des Elternfenster
	HWND					hwndLV;										// Handle auf Listview-Fenster
	MYLISTVIEWCOLUMN		column[MAX_MYLISTVIEW_COLS];
	void					(* cbkeydown)(	WORD vkey );				// Zeiger auf Callback "Taste betätigt"
	void					(* cbbegindrag) (int iItem);				// Zeiger auf Callback "Begin Dragging"
	void					(* cbdblclick)( LPNMLISTVIEW );				// Zeiger auf Callback "Doppelclick"
	void					(* cbrclick) ( LPNMLISTVIEW );				// Zeiger auf Callback "Rechtsclick"
	void					(* cbactivate) (LPNMLISTVIEW );				// Zeiger auf Callback "aktiviert"
} MYLISTVIEWPARAM;

MYLISTVIEWPARAM mylistviewparam;

HWND MyListViewCreateWindow( MYLISTVIEWPARAM * );
void MyListViewKeydown( WORD vkey );
void MyListViewDblClick( LPNMLISTVIEW lpnm );
void MyListViewRClick( LPNMLISTVIEW lpnm );
void MyListViewActivate( LPNMLISTVIEW lpnm );
void MyListViewBeginDragging( int iItem );
void MyListViewUpdateValue( PVIOBJECT *object );
LRESULT CALLBACK MyListViewHandleMessages(MYLISTVIEWPARAM* , UINT, WPARAM , LPARAM );
void MyListViewInsertPVIObjects( PVIOBJECT *object);





