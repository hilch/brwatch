#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include <windows.h>
#include "mylistview.h"
#include "main.h"
#include "stringtools.h"
#include "settings.h"
#include "dlg_writepar.h"
#include "dlg_editcpu.h"
#include "dlg_edittask.h"

HWND MyListViewCreateWindow( MYLISTVIEWPARAM * param ) {
	HFONT hfont;
	LV_COLUMN lvcolumn;
	int i;

	param->hwndLV = CreateWindowExW(WS_EX_CLIENTEDGE, L""WC_LISTVIEW, NULL, WS_CHILD | WS_VISIBLE
	                               | WS_TABSTOP | LVS_REPORT,
	                               0, 0, 0, 0, param->hwndParent, 0, param->hinstance, NULL);
	//ListView_SetUnicodeFormat(param->hwndLV, TRUE );

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
	                                     LVS_EX_FULLROWSELECT 				// select full row
	                                     | LVS_EX_GRIDLINES 				// Tabellenmarkierung
	                                     | LVS_EX_HEADERDRAGDROP			// Spalten können verschoben werden
	                                     | LVS_EX_SUBITEMIMAGES				// auch Subitems können Images haben
	                                     | LVS_EX_TRACKSELECT				// Element auswählen, wenn Cursor längere Zeit darüber ist
	                                   );

	hfont = GetStockObject( DEFAULT_GUI_FONT );
	SendMessage( param->hwndLV, WM_SETFONT, (WPARAM) hfont, TRUE );

	return param->hwndLV;
}


LRESULT CALLBACK MyListViewHandleMessages(MYLISTVIEWPARAM * lv, UINT wMsg, WPARAM wParam, LPARAM lParam) {
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


/* insert object to list view */

void MyListViewInsertPVIObjects( PVIOBJECT *object ) {
	LVITEM lvitem;
	char *text;
	BOOL found = FALSE;
	int i;

	if( object != NULL ) {
		// do not insert if item already exists
		found = FALSE;
		i= ListView_GetNextItem( mylistviewparam.hwndLV, -1, LVNI_ALL );
		while( i!= -1 ) {
			memset( &lvitem, 0, sizeof(lvitem) );
			lvitem.mask = LVIF_PARAM;
			lvitem.iItem = i;
			ListView_GetItem( mylistviewparam.hwndLV, &lvitem );
			if( lvitem.lParam != 0 ) {
				if( strcmp( object->name, (char*) lvitem.lParam ) == 0 ) {
					found = TRUE;
				}
			}
			i = ListView_GetNextItem( mylistviewparam.hwndLV, i, LVNI_ALL );
		}

		if( found == TRUE )
			return;

		if( object->watchsort < 0 ) { // object is not listed
			return;
		}

		/* insert a process variable */
		if( object->type == POBJ_PVAR) {
			if( WatchPviObject( object, TRUE ) != NULL ) {
				char tempstring[256];

				if( object->ex.pv.task != NULL ) {
					PVIOBJECT *task = object->ex.pv.task;

					strcpy( tempstring, task->descriptor );
					strcat( tempstring, ":" );
				} else {
					strcpy( tempstring, "" ); // should never happen
				}
				strcat( tempstring, object->descriptor );

				memset(&lvitem, 0, sizeof(lvitem));
				lvitem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
				lvitem.pszText = tempstring;
				lvitem.lParam = (LPARAM) object->name;
				lvitem.iImage = ResourcesGetImageIndex(IDR_ICO_VARIABLE);
				lvitem.iItem = object->watchsort;

				/* name */
				ListView_InsertItem(mylistviewparam.hwndLV, &lvitem);
				ListView_SetColumnWidth( mylistviewparam.hwndLV, 0, LVSCW_AUTOSIZE );


				/* data type */
				if( object->ex.pv.type == BR_STRING ) {
					sprintf( tempstring, "%1s%s(%lu)", object->ex.pv.scope[0] == 'd' ? "*" : "", object->ex.pv.pdatatype, object->ex.pv.length-1 );
				} else if( object->ex.pv.type == BR_WSTRING ) {
					sprintf( tempstring, "%1s%s(%lu)", object->ex.pv.scope[0] == 'd' ? "*" : "", object->ex.pv.pdatatype, object->ex.pv.length/2 -1 );
				} else {
					sprintf( tempstring, "%1s%s", object->ex.pv.scope[0] == 'd' ? "*" : "", object->ex.pv.pdatatype );
				}
				text = tempstring;
				ListView_SetItemText(mylistviewparam.hwndLV, lvitem.iItem, 1, text ); // data type
				memset(&lvitem, 0, sizeof(lvitem));
				lvitem.mask = LVIF_IMAGE;
				lvitem.iImage = ResourcesGetPviObjectImage(object);
				lvitem.iSubItem = 1;
				lvitem.iItem = object->watchsort;
				ListView_SetItem( mylistviewparam.hwndLV, &lvitem ); // set icon
				ListView_SetColumnWidth( mylistviewparam.hwndLV, 1, LVSCW_AUTOSIZE );

				/* value range */
				switch( object->ex.pv.scope[0] ) {
					case 'g':
						text = "<GLOBAL>";
						break;
					case 'l':
						text = "<LOCAL >";
						break;
					case 'd':
						text = "<DYNAM.>";
						break;
				}
				ListView_SetItemText(mylistviewparam.hwndLV, lvitem.iItem, 2, text ); // Scope
				ListView_SetColumnWidth( mylistviewparam.hwndLV, 2, LVSCW_AUTOSIZE );

				/* value */
				text = " ";
				ListView_SetItemText(mylistviewparam.hwndLV, lvitem.iItem, 3, text );	// value
				//
				ListView_EnsureVisible( mylistviewparam.hwndLV, lvitem.iItem, TRUE );
				ListView_SetColumnWidth( mylistviewparam.hwndLV, 3, LVSCW_AUTOSIZE );
			}

			/* insert a CPU */
		} else if( object->type == POBJ_CPU ) {
			char tempstring[512];

			sprintf( tempstring, "%s %s %s", object->ex.cpu.cputype, object->ex.cpu.arversion, object->descriptor );
			memset(&lvitem, 0, sizeof(lvitem));
			lvitem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
			lvitem.pszText = tempstring;
			lvitem.lParam = (LPARAM) object->name;
			lvitem.iImage = ResourcesGetPviObjectImage(object);
			lvitem.iItem = object->watchsort;

			/* name */
			ListView_InsertItem(mylistviewparam.hwndLV, &lvitem);
			ListView_SetColumnWidth( mylistviewparam.hwndLV, 0, LVSCW_AUTOSIZE_USEHEADER );

			/* type */
			ListView_SetItemText(mylistviewparam.hwndLV, lvitem.iItem, 1, "CPU " );
			ListView_SetColumnWidth( mylistviewparam.hwndLV, 1, LVSCW_AUTOSIZE_USEHEADER );

			/* scope */
			ListView_SetColumnWidth( mylistviewparam.hwndLV, 2, LVSCW_AUTOSIZE_USEHEADER );

			/* value */
			switch( object->error ) {
				case 0:
					strcpy( tempstring, "(running) RTC:0000/00/00-00:00:00" );
					break;

				case 4808: // no connection to plc
				case 11022:
					sprintf( tempstring, "(OFFLINE)" );
					break;

				default:
					sprintf( tempstring, "Err:%i", object->error );
					break;
			}
			ListView_SetItemText(mylistviewparam.hwndLV, lvitem.iItem, 3, tempstring );	// value
			ListView_SetColumnWidth( mylistviewparam.hwndLV, 3, LVSCW_AUTOSIZE );

			/* insert a task */
		} else if( object->type == POBJ_TASK ) {
			char tempstring[256];

			strcpy( tempstring, object->descriptor );

			memset(&lvitem, 0, sizeof(lvitem));
			lvitem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
			lvitem.pszText = tempstring;
			lvitem.lParam = (LPARAM) object->name;
			lvitem.iImage = ResourcesGetPviObjectImage(object);
			lvitem.iItem = object->watchsort;

			/* name */
			ListView_InsertItem(mylistviewparam.hwndLV, &lvitem);
			ListView_SetColumnWidth( mylistviewparam.hwndLV, 0, LVSCW_AUTOSIZE );

			/* type */
			ListView_SetItemText(mylistviewparam.hwndLV, lvitem.iItem, 1, "TASK" );
			ListView_SetColumnWidth( mylistviewparam.hwndLV, 1, LVSCW_AUTOSIZE );

			/* scope */
			// strcpy( tempstring, ((PVIOBJECT*) object->ex.task.cpu)->descriptor );
			// ListView_SetItemText(mylistviewparam.hwndLV, lvitem.iItem, 2, tempstring );
			// ListView_SetColumnWidth( mylistviewparam.hwndLV, 1, LVSCW_AUTOSIZE);
		}


		// save list index of element into object information
		i = ListView_GetNextItem( mylistviewparam.hwndLV, -1, LVNI_ALL );
		while( i != -1 ) {
			memset( &lvitem, 0, sizeof(lvitem) );
			lvitem.mask = LVIF_PARAM;
			lvitem.iItem = i;
			ListView_GetItem( mylistviewparam.hwndLV, &lvitem );
			if( lvitem.lParam != 0 ) {
				object = FindPviObjectByName( (char*) lvitem.lParam );
				if( object != NULL ) {
					object->watchsort = lvitem.iItem;
				}
			}
			i = ListView_GetNextItem( mylistviewparam.hwndLV, i, LVNI_ALL );
		}


	}
}


/* translates ojbect's data into ASCII and enters them into list view */

void MyListViewUpdateValue( PVIOBJECT *object ) {
	LVFINDINFO lvfindinfo;
	int index;
	char *format_decimal=NULL;
	char *format_hex=NULL;
	char *format_binary=NULL;
	char *format_char=NULL;
	char *text_binary=NULL;
	long long intval=0;
	int hours, minutes, seconds, milliseconds;
	char value[2048];
	char tempstring[256];

	memset( &lvfindinfo, 0, sizeof(lvfindinfo) );

	lvfindinfo.flags = LVFI_PARAM;
	lvfindinfo.lParam = (LPARAM) object->name;

	index = ListView_FindItem( mylistviewparam.hwndLV, -1, &lvfindinfo );
	if( index >= 0 ) {	// found object in list view
		switch( object->type ) {
			case POBJ_PVAR:
				switch( object->ex.pv.type ) {
					case BR_USINT:
						if( object->ex.pv.pvalue != NULL ) {
							unsigned char x;
							memcpy( &x, object->ex.pv.pvalue, 1 );
							intval = x;
						}
						format_decimal 		= "10#%+3llu  ";
						format_hex 			= "16#%2.2llX  ";
						format_binary 		= "2#%s  ";
						format_char 		= "'%c'";
						text_binary 		= int2bin(intval,8);
						break;

					case BR_SINT:
						if( object->ex.pv.pvalue != NULL ) {
							signed char x;
							memcpy( &x, object->ex.pv.pvalue, 1 );
							intval = x;
						}
						format_decimal 		= "10#%+3lli  ";
						format_hex 			= "16#%2.2llX  ";
						format_binary 		= "2#%s  ";
						format_char 		= "'%c'";
						text_binary 		= int2bin(intval,8);
						break;

					case BR_UINT:
						if( object->ex.pv.pvalue != NULL ) {
							unsigned short x;
							memcpy( &x, object->ex.pv.pvalue, 2 );
							intval = x;
						}
						format_decimal 		= "10#%+5llu  ";
						format_hex 			= "16#%4.4llX  ";
						format_binary 		= "2#%s";
						format_char 		= "";
						text_binary 		= int2bin(intval,16);
						break;

					case BR_INT:
						if( object->ex.pv.pvalue != NULL ) {
							signed short x;
							memcpy( &x, object->ex.pv.pvalue, 2 );
							intval = x;
						}
						format_decimal 		= "10#%+5lli  ";
						format_hex 			= "16#%4.4llX  ";
						format_binary 		= "2#%s";
						format_char 		= "";
						text_binary 		= int2bin(intval,16);
						break;

					case BR_UDINT:
						if( object->ex.pv.pvalue != NULL ) {
							unsigned int x;
							memcpy( &x, object->ex.pv.pvalue, 4 );
							intval = x;
						}
						format_decimal 		= "10#%+10llu  ";
						format_hex 			= "16#%8.8llX  ";
						format_binary 		= "2#%s";
						format_char			= "";
						text_binary 		= int2bin(intval,32);
						break;

					case BR_DINT:
					case BR_TIME:
						if( object->ex.pv.pvalue != NULL ) {
							signed int x;
							memcpy( &x, object->ex.pv.pvalue, 4 );
							intval = x;
						}
						format_decimal 		= "10#%+10lli  ";
						format_hex 			= "16#%8.8llX  ";
						format_binary 		= "2#%s";
						format_char			= "";
						text_binary 		= int2bin(intval,32);
						break;

					case BR_BOOL:
						if( object->ex.pv.pvalue != NULL ) {
							unsigned char x;
							memcpy( &x, object->ex.pv.pvalue, 1 );
							intval = x;
						}
						format_decimal 		= "%1.1i    ";
						format_hex 			= "";
						format_char			= "";
						format_binary 		= intval ? "TRUE" : "FALSE";
						break;


					case BR_REAL:
						if( object->ex.pv.pvalue != NULL ) {
							sprintf( value, "%g", *((float*) object->ex.pv.pvalue) );
						}
						break;

					case BR_LREAL:
						if( object->ex.pv.pvalue != NULL ) {
							sprintf( value, "%g", *((double*) object->ex.pv.pvalue) );
						}
						break;

					case BR_STRING:
						if( object->ex.pv.pvalue != NULL ) {
							char *s;
							char *d;
							s = (char*) object->ex.pv.pvalue;
							d = value;
							while( *s ) {
								if( (*s & 0xff) < ' ' ) {
									sprintf( d, "\\x%3.3x", (*s & 0xff) );
									d+=5;
									++s;
								} else {
									*d++=*s++;
								}
							}
							*d = 0;
							if( object->gui_info.interpret_as_oem ) {
								strcpy(tempstring, value);
								OemToChar( tempstring, value );
							}
						}
						break;


					case BR_WSTRING:
						if( object->ex.pv.pvalue != NULL ) {
							LV_ITEMW item; 
							item.iSubItem = 3; 
							item.pszText = object->ex.pv.pvalue; 
							SendMessageW( mylistviewparam.hwndLV, LVM_SETITEMTEXTW, index, (LPARAM)(LV_ITEMW *)&item);	
						}
						return;

					case BR_DATI:
					case BR_DATE:
						if( object->ex.pv.pvalue != NULL ) {
							struct tm* ptst;
							__time64_t t=0;

							memset( value, 0, sizeof(value) );

							memcpy( &t, object->ex.pv.pvalue, sizeof(time_t) );
							ptst = _gmtime64( &t );
							if( ptst != NULL )
								strncpy( value, asctime( ptst ), 24 );
							else
								strcpy( value, "(illegal time!)" );

							if( object->gui_info.display_as_decimal ) {
								char tempstring[20];
								sprintf( tempstring, "  (10#%.5u)", *((unsigned int*) object->ex.pv.pvalue) );
								strcat( value, tempstring );
							}
						}
						break;

					case BR_TOD:
						if( object->ex.pv.pvalue != NULL ) {
							memcpy( &intval, object->ex.pv.pvalue, 4 );
							printf("%u", (unsigned int) intval);
							hours = (int) intval / (3600*1000);
							intval -= hours * (3600*1000);
							minutes = (int) intval / (60*1000);
							intval -= minutes * (60*1000);
							seconds = (int) intval / 1000;
							intval -= seconds * 1000;
							milliseconds = (int) intval;
							sprintf( value, "%2.1u:%2.1u:%2.1u:%3.3u", hours, minutes, seconds, milliseconds );
							if( object->gui_info.display_as_decimal ) {
								char tempstring[20];
								sprintf( tempstring, "  (10#%.5u)", *((unsigned int*) object->ex.pv.pvalue) );
								strcat( value, tempstring );
							}
						}
						break;



					default:
						strcpy( value, "???" );
						break;

				}


				// change display format of integers
				if( object->ex.pv.type == BR_USINT || object->ex.pv.type == BR_SINT ||
				        object->ex.pv.type == BR_UINT || object->ex.pv.type == BR_INT ||
				        object->ex.pv.type == BR_UDINT || object->ex.pv.type == BR_DINT ||
				        object->ex.pv.type == BR_TIME ||
				        object->ex.pv.type == BR_BOOL  ) {
					strcpy( value, "" );
					if( object->gui_info.display_as_decimal ) {
						sprintf( tempstring, format_decimal, intval );
						strcat( value, tempstring );
					}
					if( object->gui_info.display_as_hex ) {
						sprintf( tempstring, format_hex, intval );
						strcat( value, tempstring );
					}
					if( object->gui_info.display_as_binary ) {
						sprintf( tempstring, format_binary, text_binary );
						strcat( value, tempstring );
					}
					if( object->gui_info.display_as_char ) {
						sprintf( tempstring, format_char, intval );
						strcat( value, tempstring );
					}
				}


				break;

			case POBJ_CPU:
				sprintf( value, "(%s) RTC:%4.4u/%2.2u/%2.2u-%2.2u:%2.2u:%2.2u",
				         object->ex.cpu.running ? "running":"stopped",
				         object->ex.cpu.rtc_time.tm_year + 1900,
				         object->ex.cpu.rtc_time.tm_mon + 1,
				         object->ex.cpu.rtc_time.tm_mday,
				         object->ex.cpu.rtc_time.tm_hour,
				         object->ex.cpu.rtc_time.tm_min,
				         object->ex.cpu.rtc_time.tm_sec
				       );
				break;

			case POBJ_TASK:
				if( ((PVIOBJECT*)object->ex.task.cpu)->ex.cpu.running == FALSE ) {
					strcpy( value, "(CPU not running)");
				} else {
					strcpy( value, object->ex.task.status );
				}
				break;

			default:
				strcpy( value, "" );
				break;
		}

		ListView_SetItemText( mylistviewparam.hwndLV, index, 3, value );

	}

}



/* is called on key change in list view */

void MyListViewKeydown( WORD vkey ) {
	int index;
	LVITEM lvitem;
	PVIOBJECT *object;


	switch( vkey ) {
		case VK_DELETE:
			index = ListView_GetNextItem( mylistviewparam.hwndLV, -1, LVNI_SELECTED );
			while( index != -1 ) {
				memset(&lvitem, 0, sizeof(lvitem) );
				lvitem.iItem = index;
				lvitem.mask = LVIF_PARAM;
				ListView_GetItem( mylistviewparam.hwndLV, &lvitem );
				object = FindPviObjectByName( (char*) lvitem.lParam );
				WatchPviObject( object, FALSE ); // remove from wath
				ListView_DeleteItem( mylistviewparam.hwndLV, index ); // remove from list view
				index = ListView_GetNextItem( mylistviewparam.hwndLV, index, LVNI_SELECTED );
			}
			break;


		case VK_RETURN:
			index = ListView_GetNextItem( mylistviewparam.hwndLV, -1, LVNI_SELECTED );
			if( index != -1 ) {
				memset(&lvitem, 0, sizeof(lvitem) );
				lvitem.iItem = index;
				lvitem.mask = LVIF_PARAM;
				ListView_GetItem( mylistviewparam.hwndLV, &lvitem );

				if( lvitem.lParam != 0 ) {
					object = FindPviObjectByName((char*)lvitem.lParam );
					DlgWritePvarShowDialog(object);
					DlgEditCpuShowDialog(object);
					DlgEditTaskShowDialog(object);
				}
			}
			break;
	}
}



/* is called on double click in list view */

void MyListViewDblClick( LPNMLISTVIEW lpnm ) {
	LVITEM lvitem;
	PVIOBJECT *object;
	BOOL write_allowed;

	// Ini- Datei lesen
	write_allowed = GetPrivateProfileInt( "General", "WriteAllowed", 0, SettingsGetFileName() );

	if( !write_allowed )
		return;

	memset( &lvitem, 0, sizeof(lvitem) );
	lvitem.mask = LVIF_PARAM;
	lvitem.iItem = lpnm->iItem;
	ListView_GetItem( mylistviewparam.hwndLV, &lvitem );


	if( lvitem.lParam != 0 ) {
		object = FindPviObjectByName((char*)lvitem.lParam );
		DlgWritePvarShowDialog(object);
		DlgEditCpuShowDialog(object);
		DlgEditTaskShowDialog(object);
	}

}


void MyListViewRClick( LPNMLISTVIEW lpnm ) {
	HMENU hmenu;
	//RECT offset;
	LVITEM lvitem;
	UINT enabled;
	char *entry;
	POINT pt;


	memset( &lvitem, 0, sizeof(lvitem) );
	lvitem.mask = LVIF_PARAM ;
	lvitem.iItem = lpnm->iItem;
	ListView_GetItem( mylistviewparam.hwndLV, &lvitem );

	if( lvitem.lParam == 0 )
		return; // should not happen :-)

	g_selectedPVIObject = FindPviObjectByName( (char*) lvitem.lParam );

	if( g_selectedPVIObject == NULL )
		return; // should also not happen :-)

	//GetWindowRect( mylistviewparam.hwndLV, &offset );
	GetCursorPos(&pt);

	hmenu = CreatePopupMenu();

	switch( g_selectedPVIObject->type ) {
		case POBJ_PVAR:
			switch( g_selectedPVIObject->ex.pv.type ) {
				case BR_USINT:
				case BR_SINT:
					enabled = g_selectedPVIObject->gui_info.display_as_char ? MF_CHECKED : MF_UNCHECKED;
					AppendMenu( hmenu, MF_ENABLED | MF_STRING | enabled, IDM_CHANGE_VIEW_CHAR, "char" );
				case BR_UINT:
				case BR_INT:
				case BR_UDINT:
				case BR_DINT:
				case BR_TIME:
					enabled = g_selectedPVIObject->gui_info.display_as_decimal ? MF_CHECKED : MF_UNCHECKED;
					AppendMenu( hmenu, MF_ENABLED | MF_STRING | enabled, IDM_CHANGE_VIEW_DECIMAL, "dec" );
					enabled = g_selectedPVIObject->gui_info.display_as_hex ? MF_CHECKED : MF_UNCHECKED;
					AppendMenu( hmenu, MF_ENABLED | MF_STRING | enabled, IDM_CHANGE_VIEW_HEX, "hex" );
					enabled = g_selectedPVIObject->gui_info.display_as_binary ? MF_CHECKED : MF_UNCHECKED;
					AppendMenu( hmenu,  MF_ENABLED | MF_STRING | enabled, IDM_CHANGE_VIEW_BINARY, "bin" );
					break;

				case BR_REAL:
				case BR_LREAL:
					AppendMenu( hmenu, MF_ENABLED | MF_STRING, 0, "no option" );
					break;

				case BR_STRING:
					entry = g_selectedPVIObject->gui_info.interpret_as_oem ? "interpret as ANSI" : "interpret as OEM";
					AppendMenu( hmenu, MF_ENABLED | MF_STRING, IDM_CHANGE_VIEW_OEMCHAR, entry );
					break;


				case BR_DATI:
					entry = g_selectedPVIObject->gui_info.display_as_decimal ? "hide decimal value" : "show decimal value";
					AppendMenu( hmenu, MF_ENABLED | MF_STRING, IDM_CHANGE_VIEW_DECIMAL, entry );
					break;

				default:
					break;
			}

			break;

		default:
			break;

	}

	AppendMenu( hmenu, MF_ENABLED | MF_STRING, IDM_DELETE_OBJECT_FROM_LIST, "remove this object" );
	//TrackPopupMenu( hmenu, TPM_VERTICAL, offset.left + lpnm->ptAction.x , offset.top + lpnm->ptAction.y, 0, hMainWindow, NULL );
	TrackPopupMenu( hmenu, TPM_VERTICAL, pt.x, pt.y, 0, g_hwndMainWindow, NULL );

}



void MyListViewActivate( LPNMLISTVIEW lpnm ) {
	LVITEM lvitem;

	memset( &lvitem, 0, sizeof(lvitem) );
	lvitem.mask = LVIF_PARAM ;
	lvitem.iItem = lpnm->iItem;
	ListView_GetItem( mylistviewparam.hwndLV, &lvitem );

}


void MyListViewBeginDragging( int iItem ) {
	HIMAGELIST himl;
	LVITEM lvitem;
	POINT p;
	RECT rect_mainwindow, rect_listview;

	ZeroMemory( &rect_mainwindow, sizeof(rect_mainwindow) );
	ZeroMemory( &rect_listview, sizeof(rect_listview) );

	GetWindowRect( mylistviewparam.hwndLV, &rect_listview );
	GetWindowRect( g_hwndMainWindow, &rect_mainwindow );

	memset( &lvitem, 0, sizeof(lvitem) );
	lvitem.mask = LVIF_PARAM ;
	lvitem.iItem = iItem;
	ListView_GetItem( mylistviewparam.hwndLV, &lvitem );
	g_draggedPVIObject = FindPviObjectByName( (char*) lvitem.lParam);

	if( g_draggedPVIObject == NULL )
		return;

	himl = ListView_CreateDragImage( mylistviewparam.hwndLV, iItem, &p );
	ImageList_BeginDrag(himl, 0, 0, 0);
	// start drag effect
	ImageList_DragEnter(g_hwndMainWindow, rect_listview.left - rect_mainwindow.left,
	                    rect_listview.top - rect_mainwindow.top );
	SetCapture(g_hwndMainWindow);

	WatchPviObject( g_draggedPVIObject, FALSE ); // remove from watch
	ListView_DeleteItem( mylistviewparam.hwndLV, iItem );
	ListView_Update( mylistviewparam.hwndLV, iItem );

}



