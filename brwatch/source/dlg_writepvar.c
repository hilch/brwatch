
#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include "main.h"
#include "pvi_interface.h"
#include "resource.h"



/*-@@+@@--------------------------------[Do not edit manually]------------
 Procedure: WritePvarDlgProc
 Created  : Mon Apr  3 12:09:56 2006
 Modified : Mon Apr  3 12:09:56 2006

 Synopsys : Dialogbox zum Schreiben von Prozessvariablen
 Input    : 
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

LRESULT CALLBACK  WritePvarDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static PVIOBJECT *object;
	int result;

    switch (uMsg)
    {
        case WM_INITDIALOG:
            /*
             * Nothing special to initialize.
             */
			SetWindowText( hDlg, "Write Value" );
			if( lParam != 0 ) {
				object = (PVIOBJECT *) lParam;
				SetDlgItemText( hDlg, IDR_STATIC_NAME, object->descriptor );
				SetDlgItemText( hDlg, IDR_EDIT_VALUE, "" );
				CheckDlgButton( hDlg, IDC_CHECK1, BST_CHECKED );
				// Variablentyp und - Bereich holen
				switch( object->ex.pv.type ){
					case BR_STRING: 
						{
							char tempstring[256];

							sprintf( tempstring, "STRING(%u) = %u Bytes", object->ex.pv.length-1, object->ex.pv.length ); 
							SetDlgItemText( hDlg, IDC_STATIC_VARTYPE, tempstring ); 
							if( object->ex.pv.pvalue != NULL ){
								SetDlgItemText( hDlg, IDR_EDIT_VALUE, object->ex.pv.pvalue );
							}
						}
						break; 

					case BR_BOOL:
						SetDlgItemText( hDlg, IDC_STATIC_VARTYPE, "BOOL (0 ... 1)" );
						if( object->ex.pv.pvalue != NULL )
							SetDlgItemInt( hDlg, IDR_EDIT_VALUE, *((unsigned char*) object->ex.pv.pvalue), 0 );
						break;
					case BR_USINT:
						SetDlgItemText( hDlg, IDC_STATIC_VARTYPE, "USINT (0 ... +255)" );
						if( object->ex.pv.pvalue != NULL )
							SetDlgItemInt( hDlg, IDR_EDIT_VALUE, *((unsigned char*) object->ex.pv.pvalue), 0 );
						break;
					case BR_SINT:
						SetDlgItemText( hDlg, IDC_STATIC_VARTYPE, "SINT (-128 ... +127)" );
						if( object->ex.pv.pvalue != NULL )
							SetDlgItemInt( hDlg, IDR_EDIT_VALUE, *((signed char*) object->ex.pv.pvalue), 1 );
						break;
					case BR_UINT:
						SetDlgItemText( hDlg, IDC_STATIC_VARTYPE, "UINT (0 ... +65535)" );
						if( object->ex.pv.pvalue != NULL )
							SetDlgItemInt( hDlg, IDR_EDIT_VALUE, *((unsigned short*) object->ex.pv.pvalue), 0 );
						break;
					case BR_INT:
						SetDlgItemText( hDlg, IDC_STATIC_VARTYPE, "INT (-32768 ... +32767)" );
						if( object->ex.pv.pvalue != NULL )
							SetDlgItemInt( hDlg, IDR_EDIT_VALUE, *((signed short*) object->ex.pv.pvalue), 1 );
						break;
					case BR_UDINT:
						SetDlgItemText( hDlg, IDC_STATIC_VARTYPE, "UDINT (0 ... +4294967295)" );
						if( object->ex.pv.pvalue != NULL )
							SetDlgItemInt( hDlg, IDR_EDIT_VALUE, *((unsigned int*) object->ex.pv.pvalue), 0 );
						break;
					case BR_DINT:
						SetDlgItemText( hDlg, IDC_STATIC_VARTYPE, "DINT (-2147483648 ... +2147483647)" );
						if( object->ex.pv.pvalue != NULL )
							SetDlgItemInt( hDlg, IDR_EDIT_VALUE, *((signed int*) object->ex.pv.pvalue), 1 );
						break;
					case BR_REAL:
						SetDlgItemText( hDlg, IDC_STATIC_VARTYPE, "REAL (-3.4e38 ... +3.4e38)" );
						if( object->ex.pv.pvalue != NULL ){
							char tempstring[256];
							sprintf( tempstring, "%g", *( float*) object->ex.pv.pvalue );
							SetDlgItemText( hDlg, IDR_EDIT_VALUE, tempstring );
						}
						break;
					case BR_TIME:
						SetDlgItemText( hDlg, IDC_STATIC_VARTYPE, "TIME ( 1ms ... +/-24 days)" );
						if( object->ex.pv.pvalue != NULL ){
						    int days, hours, minutes, milliseconds, intval;

							memcpy( &intval, object->ex.pv.pvalue, 4 );
							days = (int) intval / (24*3600*1000);
							intval -= days * (24*3600*1000);
							hours = (int) intval / (3600*1000);
							intval -= hours * (3600*1000);
							minutes = (int) intval / (60*1000);
							intval -= minutes * (60*1000);
							milliseconds = (int) intval;
							SetDlgItemInt( hDlg, IDC_EDIT_DAY, days, FALSE );
							SetDlgItemInt( hDlg, IDC_EDIT_HOUR, hours, FALSE );
							SetDlgItemInt( hDlg, IDC_EDIT_MINUTE, minutes, FALSE );
							SetDlgItemInt( hDlg, IDC_EDIT_MILLISECONDS, milliseconds, FALSE );
						}
						break;
					case BR_DATI:
						SetDlgItemText( hDlg, IDC_STATIC_VARTYPE, "DATE_AND_TIME (midnight, January 1, 1970 ... 3:14:07 January 19, 2038)" );
						if( object->ex.pv.pvalue != NULL ){
							__time64_t t=0;
							struct tm *ptst;

							memcpy( &t, object->ex.pv.pvalue, sizeof(time_t) );
							ptst = _gmtime64( &t );

							if( ptst == NULL ){
								time_t t=time(0);
								ptst = gmtime( &t );
								MessageBox( GetParent(hDlg), "Actual time was invalid.\nUsed system time !", "Error", MB_OK | MB_ICONERROR );
							}
							SetDlgItemInt( hDlg, IDC_EDIT_YEAR, ptst->tm_year+1900, FALSE );
							SetDlgItemInt( hDlg, IDC_EDIT_MONTH, ptst->tm_mon+1, FALSE );
							SetDlgItemInt( hDlg, IDC_EDIT_DAY, ptst->tm_mday, FALSE );
							SetDlgItemInt( hDlg, IDC_EDIT_HOUR, ptst->tm_hour, FALSE );
							SetDlgItemInt( hDlg, IDC_EDIT_MINUTE, ptst->tm_min, FALSE );
							SetDlgItemInt( hDlg, IDC_EDIT_MILLISECONDS, ptst->tm_sec*1000, FALSE );
						}
						break;
				}
				// Task und CPU - Namen holen
				if( object->ex.pv.task != NULL ){
					PVIOBJECT *o;
					char tempstring[256];

					o = object->ex.pv.task;
					SetDlgItemText( hDlg, IDC_STATIC_TASK, o->descriptor );
					o = object->ex.pv.cpu;

					sprintf( tempstring, "%s %s %s", o->ex.cpu.cputype, o->ex.cpu.arversion, o->descriptor ); 
					SetDlgItemText( hDlg, IDC_STATIC_CPU, tempstring );

				}

				// nicht benutzbare Felder disablen
				if( object->ex.pv.type == BR_DATI ){
					EnableWindow( GetDlgItem(hDlg, IDR_EDIT_VALUE), FALSE );
				}
				else if ( object->ex.pv.type == BR_TIME ){
					EnableWindow( GetDlgItem(hDlg, IDR_EDIT_VALUE), FALSE );
					EnableWindow( GetDlgItem(hDlg, IDC_EDIT_YEAR), FALSE );
					EnableWindow( GetDlgItem(hDlg, IDC_EDIT_MONTH), FALSE );
				}
				else {
					EnableWindow( GetDlgItem(hDlg, IDC_EDIT_YEAR), FALSE );
					EnableWindow( GetDlgItem(hDlg, IDC_EDIT_DAY), FALSE );
					EnableWindow( GetDlgItem(hDlg, IDC_EDIT_MONTH), FALSE );
					EnableWindow( GetDlgItem(hDlg, IDC_EDIT_MINUTE), FALSE );
					EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HOUR), FALSE );
					EnableWindow( GetDlgItem(hDlg, IDC_EDIT_MILLISECONDS), FALSE );
				}
				return TRUE;
			}
            return FALSE;



        case WM_COMMAND:

            switch (wParam)
            {
                case IDOK:
					result = -1; // erst mal Fehler annehmen
					if( object != NULL  ){
						char tempstring[256];

						tempstring[0] = 0;
						GetDlgItemText( hDlg, IDR_EDIT_VALUE, tempstring, sizeof(tempstring) );
							
						switch( object->ex.pv.type ){
								// String
							case BR_STRING:
								result = WritePviPvar( object, tempstring );
								SendMessage( GetDlgItem( hDlg, IDR_EDIT_VALUE ), EM_SETSEL, 0, -1 ); // alles markieren
								break;

							case BR_BOOL:
								{
									unsigned char byte;

									if( (*tempstring == '1') || !strcmp( tempstring, "true" ) || !strcmp( tempstring, "True" ) || !strcmp( tempstring, "TRUE")  ){
										byte = 1;
										result = WritePviPvar( object, &byte );
									}
									else if( (*tempstring == '0') || !strcmp( tempstring, "false" ) || !strcmp( tempstring, "False" ) || !strcmp( tempstring, "FALSE")  ){
										byte = 0;
										result = WritePviPvar( object, &byte );
									}
								}
								SendMessage( GetDlgItem( hDlg, IDR_EDIT_VALUE ), EM_SETSEL, 0, -1 ); // alles markieren
								break;

								// Integer- Formate

							case BR_USINT:
							case BR_SINT:
							case BR_UINT:
							case BR_INT:
							case BR_UDINT:
							case BR_DINT:
								{
									__int64 integer;
									BOOL valid;

									valid = FALSE;
									if( sscanf( tempstring, " 10#%10I64d" , &integer ) == 1 ){
										valid = TRUE;
									}
									else
									if( sscanf( tempstring, " 16#%8I64x", &integer ) == 1 ){
										valid = TRUE;
									}
									else
									if( sscanf( tempstring, " %10I64d", &integer ) == 1 ){
										valid = TRUE;
									}

									// Wertebereich überprüfen
									if( object->ex.pv.type == BR_USINT || object->ex.pv.type == BR_UINT || object->ex.pv.type == BR_UDINT ){
										if( integer >> (object->ex.pv.length*8) ) {
											valid = FALSE;
										}
									}
									else { // signed - Typen
										if( integer <  -( 1<<(object->ex.pv.length*8-1) )  ||
											integer > ( (1<<(object->ex.pv.length*8-1)) -1)          ){
												valid = FALSE;
										}
									}
									if( valid ){
										result = WritePviPvar( object, &integer );
									}
								}
								SendMessage( GetDlgItem( hDlg, IDR_EDIT_VALUE ), EM_SETSEL, 0, -1 ); // alles markieren
								break;


								// Fliessomma
							case BR_REAL:
								{
									float real;

									if( sscanf( tempstring, "%g", &real ) == 1 ){
										result = WritePviPvar( object, &real );
									}
								}
								SendMessage( GetDlgItem( hDlg, IDR_EDIT_VALUE ), EM_SETSEL, 0, -1 ); // alles markieren
								break;

							case BR_TIME:
								{
									int days, hours, minutes, milliseconds;
									__int64 integer;

									days = GetDlgItemInt( hDlg, IDC_EDIT_DAY, NULL, FALSE );
									hours = GetDlgItemInt( hDlg, IDC_EDIT_HOUR, NULL, FALSE );
									minutes = GetDlgItemInt( hDlg, IDC_EDIT_MINUTE, NULL, FALSE );
									milliseconds = GetDlgItemInt( hDlg, IDC_EDIT_MILLISECONDS, NULL, FALSE );
									
									integer =  (days * 24 * 3600 * 1000);
									integer += (hours * 3600 * 1000);
									integer += (minutes * 60 * 1000);
									integer += milliseconds;
									
									
									if( (integer & 0xFFFFFFFF) == integer )
										result = 0;

									SetDlgItemText( hDlg, IDR_EDIT_VALUE, result == 0 ? "OK" : "TO BIG!!" );

									if( result == 0 )
										WritePviPvar( object, &integer );

								}
								break;


							case BR_DATI:
								{
									struct tm tst;

									memset( &tst, 0, sizeof(tst) );

									tst.tm_year = GetDlgItemInt( hDlg, IDC_EDIT_YEAR, NULL, FALSE )-1900;
									tst.tm_mon = GetDlgItemInt( hDlg, IDC_EDIT_MONTH, NULL, FALSE )-1;
									tst.tm_mday = GetDlgItemInt( hDlg, IDC_EDIT_DAY, NULL, FALSE );
									tst.tm_hour = GetDlgItemInt( hDlg, IDC_EDIT_HOUR, NULL, FALSE );
									tst.tm_min  = GetDlgItemInt( hDlg, IDC_EDIT_MINUTE, NULL, FALSE );
									tst.tm_sec = GetDlgItemInt( hDlg, IDC_EDIT_MILLISECONDS, NULL, FALSE )/1000;

									if( tst.tm_year > 138 || tst.tm_year < 70 ){
										HWND hwnd = GetDlgItem( hDlg, IDC_EDIT_YEAR );
										SetDlgItemText( hDlg, IDR_EDIT_VALUE, "Wrong year !" );
										SetFocus( hwnd );
										SendMessage( hwnd, EM_SETSEL, 0, -1 ); 
									}
									else if( tst.tm_mon < 0 || tst.tm_mon > 11 ){
										HWND hwnd = GetDlgItem( hDlg, IDC_EDIT_MONTH );
										SetDlgItemText( hDlg, IDR_EDIT_VALUE, "Wrong month !" );
										SetFocus( hwnd );
										SendMessage( hwnd, EM_SETSEL, 0, -1 ); 
									}
									else if( tst.tm_mday < 1 || tst.tm_mday > 31 ){
										HWND hwnd = GetDlgItem( hDlg, IDC_EDIT_DAY );
										SetDlgItemText( hDlg, IDR_EDIT_VALUE, "Wrong day !" );
										SetFocus( hwnd );
										SendMessage( hwnd, EM_SETSEL, 0, -1 ); 

									}
									else if( tst.tm_hour > 23 ){
										HWND hwnd = GetDlgItem( hDlg, IDC_EDIT_HOUR );
										SetDlgItemText( hDlg, IDR_EDIT_VALUE, "Wrong hour !" );
										SetFocus( hwnd );
										SendMessage( hwnd, EM_SETSEL, 0, -1 ); 

									}
									else if( tst.tm_sec > 59 ){
										HWND hwnd = GetDlgItem( hDlg, IDC_EDIT_MILLISECONDS );
										SetDlgItemText( hDlg, IDR_EDIT_VALUE, "Wrong milliseconds !" );
										SetFocus( hwnd );
										SendMessage( hwnd, EM_SETSEL, 0, -1 ); 

									}
									else {
										extern long _timezone;

										time_t t = mktime( &tst );

										if( t != -1 ){
											t -= _timezone;
											WritePviPvar( object, &t );
											result = 0;
										}
	
										SetDlgItemText( hDlg, IDR_EDIT_VALUE, "" );
									}
								}
								break;
						}					


						if( result == 0 ){
							if( IsDlgButtonChecked( hDlg, IDC_CHECK1 ) == BST_CHECKED )
								EndDialog(hDlg, TRUE );
						}
						else {
							MessageBeep( MB_ICONEXCLAMATION );
						}
						return TRUE;
					}
					return FALSE;


                case IDCANCEL:
                    /*
                     * OK or Cancel was clicked, close the dialog.
                     */
                    EndDialog(hDlg, TRUE);
                    return TRUE;
            }
            break;
    }

    return FALSE;
}
