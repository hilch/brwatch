#include "mylistview.h"
#include "main.h"


HWND CreateMyListView( MYLISTVIEWPARAM * param )
{
	HFONT hfont;
	LV_COLUMN lvcolumn;
	int i;

	param->hwndLV = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, WS_CHILD | WS_VISIBLE 
									| WS_TABSTOP | LVS_REPORT,
									0, 0, 0, 0, param->hwndParent, 0, param->hinstance, NULL);

	for (i = 0; i < MAX_MYLISTVIEW_COLS; ++i) {
		if (param->column[i].name == NULL)
			break;
		if (param->column[i].width == 0) {
			param->column[i].width = 50;
		}
		memset(&lvcolumn, 0, sizeof(lvcolumn));
		lvcolumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvcolumn.fmt = LVCF_TEXT;
		lvcolumn.cx = param->column[i].width;
		lvcolumn.cchTextMax = (int) strlen(param->column[i].name);
		lvcolumn.iSubItem = i;
		lvcolumn.pszText = param->column[i].name;
		ListView_InsertColumn(param->hwndLV, i, &lvcolumn);
	}


	ListView_SetExtendedListViewStyleEx( param->hwndLV, 0, 
		LVS_EX_FULLROWSELECT 				// ganze Zeile kann ausgewählt werden
		| LVS_EX_GRIDLINES 					// Tabellenmarkierung
		| LVS_EX_HEADERDRAGDROP				// Spalten können verschoben werden
		| LVS_EX_SUBITEMIMAGES				// auch Subitems können Images haben
		| LVS_EX_TRACKSELECT				// Element auswählen, wenn Cursor längere Zeit darüber ist
		);

	hfont = GetStockObject( DEFAULT_GUI_FONT );
	SendMessage( param->hwndLV, WM_SETFONT, (WPARAM) hfont, TRUE );

	return param->hwndLV;
}


LRESULT CALLBACK HandleMyListViewMessages(MYLISTVIEWPARAM * lv, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	LPNMLISTVIEW pnmlv;
	LPNMLVKEYDOWN pnkd;


	switch (wMsg) {


	case WM_NOTIFY:
		pnmlv = (LPNMLISTVIEW)lParam;

		if (pnmlv->hdr.hwndFrom != lv->hwndLV) {
			return 1;
		}

		switch (pnmlv->hdr.code) {
		case NM_DBLCLK:
			if (lv->cbdblclick != NULL) {
				lv->cbdblclick(pnmlv);
			}
			return 0;
			break;


		case NM_RCLICK:
			if (lv->cbrclick != NULL) {
				lv->cbrclick(pnmlv);
			}
			return 0;
			break;


		case LVN_KEYDOWN:
			pnkd = (LPNMLVKEYDOWN)lParam;
			if (lv->cbkeydown != NULL) {
				lv->cbkeydown(pnkd->wVKey);
			}
			return 0;
			break;

		case LVN_ITEMACTIVATE:
			if (lv->cbactivate != NULL) {
				lv->cbactivate(pnmlv);
			}
			return 0;
			break;

		case LVN_BEGINDRAG:	// Begin Draggin
			if (lv->cbbegindrag != NULL) {
				(lv->cbbegindrag) (pnmlv->iItem);
			}
			return 0;

		}

		break;


	default:

		break;


	}

	return 1;
}
