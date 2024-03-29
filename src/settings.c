#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include "main.h"

#define INIFILE 				"brwatch.ini"

static char inifile[MAX_PATH];



char *SettingsGetFileName( void ) {
	return inifile;
}



void SettingsInitialize(void) {
	FILE *file;
	HRSRC hrsrc;
	HGLOBAL hglobal;
	char * p;
	DWORD length;
	BOOL inifile_exist=FALSE;
	char errorMessage[256];

	strcpy( inifile, GetApplicationPath() );
	strcat( inifile, "\\" INIFILE);

	file = fopen( inifile, "r" );
	if( file == NULL ) {
		if( (hrsrc = FindResource( NULL, MAKEINTRESOURCE(IDR_DEFAULT_INI1), "DEFAULT_INI" )) != NULL ) {
			if( (hglobal = LoadResource( NULL, hrsrc )) != NULL ) {
				length = SizeofResource( NULL, hrsrc );
				if( (p=LockResource(hglobal)) != NULL ) {
					if( (file = fopen( inifile, "wb+" )) != NULL ) {
						if( fwrite( p, 1, length, file ) == length ) {
							char tempstring[256];
							inifile_exist = TRUE;
							snprintf( tempstring, sizeof(tempstring), "The settings were not found.\n%s\nwas created !", inifile );
							MessageBox( MainWindowGetHandle(), tempstring, "Create *.ini file", MB_OK );
						} else {
							perror(errorMessage);
						}
						fclose(file);
					} else {
						perror(errorMessage);
					}

				}
			}
		}
	} else {
		fclose(file);
		inifile_exist = TRUE;
	}

	if( inifile_exist == FALSE ) {
		char tempstring[256];
		snprintf( tempstring, sizeof(tempstring), "Error creating *.ini file !\n%s", errorMessage );
		MessageBox( MainWindowGetHandle(), tempstring, "Error!", MB_OK | MB_ICONERROR );
	}

}




BOOL CALLBACK  SettingsDlg(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_INITDIALOG: {
			FILE *file;
			size_t bytes_read;
			char *buf;
			HFONT hfont;



			buf = malloc(32768);
			if( buf == NULL ) {
				MessageBox( MainWindowGetHandle(), "To less memory !", "Error", MB_OK | MB_ICONERROR );
				return FALSE;
			}

			file = fopen( SettingsGetFileName(), "rb" );

			if( file != NULL ) {
				if( (bytes_read = fread( buf, 1, 32768, file )) ) {
					buf[bytes_read] = 0;
					SetDlgItemText( hDlg, IDC_EDIT1, buf );
					free(buf);
					fclose(file);
				} else if( ferror(file) ) {
					sprintf( buf, "Error reading %s !", SettingsGetFileName() );
					MessageBox( MainWindowGetHandle(), buf, "Error", MB_OK | MB_ICONERROR );
					free(buf);
					fclose(file);
					return FALSE;
				}
			} else {
				sprintf( buf, "Could not open %s !", SettingsGetFileName() );
				MessageBox( MainWindowGetHandle(), buf, "Error", MB_OK | MB_ICONERROR );
				free(buf);
				fclose(file);
				return FALSE;
			}

			hfont = GetStockObject( SYSTEM_FIXED_FONT );
			SendDlgItemMessage( hDlg, IDC_EDIT1, WM_SETFONT, (WPARAM) hfont, TRUE );

		}

		return TRUE;

		case WM_COMMAND:
			switch (wParam) {
				case IDOK: {
					FILE *file;
					DWORD length;
					char * buf;

					file = fopen( SettingsGetFileName(), "wb+" );
					if( file != NULL ) {
						length = GetWindowTextLength( GetDlgItem(hDlg, IDC_EDIT1) );
						buf = malloc(length+1);
						if( buf != NULL ) {
							GetDlgItemText( hDlg, IDC_EDIT1, buf, length+1 );
							buf[length] = 0;
							fwrite( buf, 1, length, file );
							fclose(file);
							free(buf);
							EndDialog(hDlg, TRUE );
							return TRUE;
						}
					}
					MessageBox( MainWindowGetHandle(), "Could not save the settings", "Error", MB_OK | MB_ICONERROR );
				}
				// weiter wie bei IDCANCEL...

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

