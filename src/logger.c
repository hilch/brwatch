
#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include "main.h"
#include "settings.h"
#include "pvi_interface.h"
#include "logger.h"
#include "compressfile.h"

#include "resource.h"

#define CACHESIZE (16 * 1024 )

// prototypes
static int CountLoggedObjects( void );
BOOL IsLoggerRunning( void );
static void ReadSettings( void );
extern int minigzip( int argc, char *argv[] );

static BOOL zip_the_files;
static BOOL write_by_change;
static char loggerfile[MAX_PATH];
static int filesize;
static int	cycletime;
static BOOL logger_is_running=0;
static BOOL start_write_by_change=0;
static PVIOBJECT *cpu_timemaster=NULL;

#define START_LOGGING 10
#define RENAME_OLD_FILE 20
#define WRITE_HEADER 30
#define OPEN_FILE 40
#define WRITE_TO_FILE 50
#define COMPRESS_FILE 60
#define STOP_LOGGING 70

static PVIOBJECT **logged_object = NULL;  // point to list with objects to be logged
static int no_of_logged_objects=0;			// number of logged objects
static int estimated_row_size;			  // estimated row size
static volatile int logger_step;

BOOL IsLoggerRunning( void ) {
	return logger_is_running;
}


/* creates a list with logged objects */

static int CountLoggedObjects( void ) {
	PVIOBJECT *object;
	char *p;

	no_of_logged_objects = 0;
	estimated_row_size = 0;

	if( logged_object != NULL ) {
		free(logged_object);
		logged_object = NULL;
	}

	object = GetNextPviObject(TRUE);
	while( (object = GetNextPviObject(FALSE)) != NULL ) {
		if( object->watchsort >= 0 ) { // is object in watch list ?
			switch( object->type ) {
				case POBJ_PVAR:
					p = realloc( logged_object, sizeof(PVIOBJECT*) * (no_of_logged_objects+1) );
					if( p == NULL ) {
						break;
					} else {
						logged_object = (PVIOBJECT**) p;
						logged_object[no_of_logged_objects] = object;
						estimated_row_size += object->ex.pv.length;
						++no_of_logged_objects;
					}
					break;

				default:
					break;
			}
		}
	}

	return no_of_logged_objects;
}



/* gets INI parameters */

static void ReadSettings( void ) {
	PVIOBJECT *object;
	int last_watchsort;


	GetPrivateProfileString( "Logger", "Filename", "C:\\temp\\logger.csv", loggerfile, sizeof(loggerfile), SettingsGetFileName() );
	filesize = GetPrivateProfileInt( "Logger", "maxfilesize", 10, SettingsGetFileName() );
	cycletime = GetPrivateProfileInt( "Logger", "cycletime", 500, SettingsGetFileName() );
	zip_the_files = GetPrivateProfileInt( "Logger", "zip", 0, SettingsGetFileName() );
	write_by_change = GetPrivateProfileInt( "Logger", "write_by_change", 0, SettingsGetFileName() );

	// the first CPU is time master
	cpu_timemaster = NULL;
	object = GetNextPviObject(TRUE);
	last_watchsort = (int) 1e6;

	while( (object = GetNextPviObject(FALSE)) != NULL ) {
		if( object->type == POBJ_CPU && object->watchsort >= 0 && object->watchsort < last_watchsort ) {
			last_watchsort = object->watchsort;
			cpu_timemaster = object;
		}
	}

	/* if no CPU object is found we retrieve it from the first variable */
	if( cpu_timemaster == NULL ) {
		object = GetNextPviObject(TRUE);
		while( (object = GetNextPviObject(FALSE)) != NULL ) {
			if( object->type == POBJ_PVAR && object->watchsort >= 0 && object->watchsort < last_watchsort ) {
				if( object->ex.pv.task != NULL  ) {
					object = object->ex.pv.task;
					if( object->ex.task.cpu ) {
						cpu_timemaster = object->ex.task.cpu;
						last_watchsort = object->watchsort;
					}
				}
			}
		}

	}

}


/* validate logger settings */

static BOOL ValidateSettings( void ) {

	if( filesize < 1 ) {
		MessageBox( MainWindowGetHandle(), "filesize at least 1 kByte !", "Logger Error - Wrong size", MB_OK | MB_ICONERROR );
		return FALSE;
	}

	if( cycletime < 50 ) {
		MessageBox( MainWindowGetHandle(), "cycletime must not below 50 ms !", "Logger Error - Wrong cycletime", MB_OK | MB_ICONERROR );
		return FALSE;
	}

	if( cpu_timemaster == NULL ) {
		MessageBox( MainWindowGetHandle(), "no cpu for timestamp found !", "Logger Error - No Timestamp- CPU", MB_OK | MB_ICONERROR );
		return FALSE;
	}

	if( CountLoggedObjects() == 0 ) {
		MessageBox( MainWindowGetHandle(), "no objects to log !", "Logger Error - No Objects", MB_OK | MB_ICONERROR );
		return FALSE;
	}

	return TRUE;

}


BOOL CreateBackupFilename( LPTSTR oldfilename, LPTSTR renamedfilename ) {
	FILETIME ftWrite;
	SYSTEMTIME stUTC, stLocal;
	HANDLE hFile;
	char drive[3];
	char directory[FILENAME_MAX];
	char filename[FILENAME_MAX];
	char extension[FILENAME_MAX];

	hFile = CreateFile( oldfilename, GENERIC_READ,FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

	if( hFile == INVALID_HANDLE_VALUE ) {
		return FALSE;
	}

	// Retrieve the file times for the file.
	if (!GetFileTime(hFile, NULL, NULL, &ftWrite)) {
		CloseHandle(hFile);
		return FALSE;
	}

	CloseHandle(hFile);

	// Convert the last-write time to local time.
	FileTimeToSystemTime(&ftWrite, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

	_splitpath( oldfilename, drive, directory, filename, extension );

	// Build a string showing the date and time.
	sprintf( renamedfilename, "%s%s%s_%4.4u-%2.2u-%2.2u@%2.2u_%2.2u_%2.2u%s",
	         drive, directory, filename,
	         stLocal.wYear, stLocal.wMonth, stLocal.wDay,
	         stLocal.wHour, stLocal.wMinute, stLocal.wSecond,
	         extension
	       );

	return TRUE;
}



/* is called by PVI callback when process variables have been changed */

void LoggerDataChanged( PVIOBJECT *object ) {
	static FILE *f;
//    char *p_step_str;
	static char *cache;
	static char errstring[130];
	static long long int written_chars;
	static char renamedfilename[MAX_PATH];
	int result;
	static DWORD dwLastTime;
	DWORD dwTime;
	int iDifftime;
	static int milliseconds;
	static int flushinterval;

	if( start_write_by_change ) {

		dwTime = GetTickCount();
		iDifftime = dwTime - dwLastTime;
		if( iDifftime > 0 ) {
			milliseconds += iDifftime;
			if( milliseconds > 9999 ) {
				milliseconds -= 10000;
			}
			flushinterval += iDifftime;
		}
		dwLastTime = dwTime;


		for(;;) { // save execution cycles with continue
			switch (logger_step) {
				case START_LOGGING:
//					p_step_str = "START_LOGGING";
					strcpy( errstring, "" );
					cache = malloc( CACHESIZE );
					milliseconds = 0;
					if( cache == NULL ) {
						strcpy( errstring, "Logger: can't allocate cache. To less memory !" );
						logger_step = STOP_LOGGING;
						break;
					}
					logger_step = RENAME_OLD_FILE;
					continue;


				case RENAME_OLD_FILE: // if logger file exists rename it
//					p_step_str = "RENAME_OLD_FILE";

					if( CreateBackupFilename( loggerfile, renamedfilename ) ) {
						rename( loggerfile, renamedfilename);

						if( zip_the_files ) {
							result = CompressFile(renamedfilename);

							if( result < 0 ) {
								sprintf( errstring, "Error %i Compressing actual loggerfile ! ", result );
								logger_step = STOP_LOGGING;
								break;
							}
						}
					}
					logger_step = WRITE_HEADER;
					continue;


				case WRITE_HEADER: // write header into log
//					p_step_str = "WRITE_HEADER";
					written_chars = 0;
					f = fopen( loggerfile, "w+" );
					if( f == NULL ) {
						strcpy( errstring,  "Logger: can't open logfile !" );
						logger_step = STOP_LOGGING;
						break;
					} else {
						result = fprintf( f, LOGFILE_HEADER_BY_CHANGE "\n" );
					}
					fclose(f);
					f = NULL;

					if( result < 0 ) {
						// error during write
						strcpy( errstring, "Logger: Error when writing header to logfile !" );
						logger_step = STOP_LOGGING;
					} else {
						written_chars += result;
						logger_step = OPEN_FILE;
					}
					continue;


				case OPEN_FILE:
//					p_step_str = "OPEN_FILE";
					f = fopen( loggerfile, "a+" );
					if( f == NULL ) {
						strcpy( errstring, "Logger: can't open logfile !" );
						logger_step = STOP_LOGGING;
					} else {
						setvbuf( f, cache, _IOFBF, CACHESIZE );
						logger_step = WRITE_TO_FILE;
						flushinterval = 0;
					}
					continue;


				case WRITE_TO_FILE: // Daten Schreiben
					//				p_step_str = "WRITE_TO_FILE";

					if( object == NULL ) { // logger was stopped
						strcpy( errstring, "" );
						fclose(f);
						f = NULL;
						logger_step = STOP_LOGGING;
						break;
					}

					// timestamp
					result = fprintf( f, "%4.4u/%2.2u/%2.2u-%2.2u:%2.2u:%2.2u,%4.4u",
					                  cpu_timemaster->ex.cpu.rtc_time.tm_year + 1900,
					                  cpu_timemaster->ex.cpu.rtc_time.tm_mon + 1,
					                  cpu_timemaster->ex.cpu.rtc_time.tm_mday,
					                  cpu_timemaster->ex.cpu.rtc_time.tm_hour,
					                  cpu_timemaster->ex.cpu.rtc_time.tm_min,
					                  cpu_timemaster->ex.cpu.rtc_time.tm_sec,
					                  milliseconds
					                );

					if( result >= 0 ) {
						written_chars += result;


						if( object->type == POBJ_PVAR ) {
							PVIOBJECT *task = object->ex.pv.task;
							PVIOBJECT *cpu  = object->ex.pv.cpu;

							// Name
							if( object->ex.pv.scope[0] == 'l' || object->ex.pv.scope[0] == 'd' ) { // local or dynamic (pointer) variable
								result = fprintf( f, ",%s[%s]%s->%s:%s", cpu->ex.cpu.cputype, cpu->ex.cpu.arversion, cpu->descriptor, task->descriptor, object->descriptor );
							} else { // global variable
								result = fprintf( f, ",%s[%s]%s->%s", cpu->ex.cpu.cputype, cpu->ex.cpu.arversion, cpu->descriptor, object->descriptor );
							}

							// Wert
							switch( object->ex.pv.type ) {
								case BR_USINT:
									result = fprintf( f, ",%u", * ( (unsigned char*) object->ex.pv.pvalue ) );
									break;

								case BR_SINT:
									result = fprintf( f, ",%+i", * ( (signed char*) object->ex.pv.pvalue ) );
									break;

								case BR_UINT:
									result = fprintf( f, ",%u", * ( (unsigned short*) object->ex.pv.pvalue ) );
									break;

								case BR_INT:
									result = fprintf( f, ",%+i", * ( (signed short*) object->ex.pv.pvalue ) );
									break;

								case BR_DATI:
								case BR_UDINT:
								case BR_TIME:
								case BR_TOD:
								case BR_DATE:
									result = fprintf( f, ",%u", * ( (unsigned int*) object->ex.pv.pvalue ) );
									break;

								case BR_DINT:
									result = fprintf( f, ",%+i", * ( (signed int*) object->ex.pv.pvalue ) );
									break;

								case BR_BOOL:
									result = fprintf( f, ",%.1u", * ( (unsigned  char*) object->ex.pv.pvalue ) );
									break;

								case BR_REAL: {
									float real = * ((float*) object->ex.pv.pvalue );
									result = fprintf( f, ",%g", real );
								}
								break;

								case BR_LREAL: {
									double lreal = * ((double*) object->ex.pv.pvalue );
									result = fprintf( f, ",%g", lreal );
								}
								break;


								case BR_STRING:
									if( object->ex.pv.pvalue != NULL ) {
										char *s;
										char *d;
										char value[256];
										int count = 0;
										s = (char*) object->ex.pv.pvalue;
										d = value;
										while( *s && (count < sizeof(value)) ) {
											if( (*s & 0xff) < ' ' ) {
												sprintf( d, "\\x%3.3x", (*s & 0xff) );
												d+=5;
												++s;
												count += 5;
											} else {
												++count;
												if( object->gui_info.interpret_as_oem ) {
													OemToCharBuff( s++, d++, 1 );
												} else {
													*d++=*s++;
												}

											}
										}
										*d = 0;
										result = fprintf( f, ",\"%.120s\"", value );
									}
									break;


								case BR_WSTRING:
									if( object->ex.pv.pvalue != NULL ) {
										fwprintf( f, L",\"%.120s\"", object->ex.pv.pvalue );
									}
									break;

								default:
									result = 0;
									break;
							}
							if( result < 0 ) {
								break;
							} else {
								written_chars += result;
							}
						} else { // all other objects could be ignored

						}
					}
					fprintf( f, "\n" );

					if( flushinterval > 10000 ) {			// flush file after a while
						flushinterval = 0;
						fflush(f);
					}

					if( result < 0 ) { // Fehler
						strcpy( errstring, "Logger: can't write into logfile !" );
						fclose(f);
						f = NULL;
						logger_step = STOP_LOGGING;
					}

					else if( written_chars >= (filesize * 1024) ) { // max. size of log file reached ?
						fclose(f);
						f = NULL;
						logger_step = COMPRESS_FILE;
					}
					break;


				case COMPRESS_FILE:
					//				p_step_str = "COMPRESS_FILE";

					result = CreateBackupFilename( loggerfile, renamedfilename );

					rename( loggerfile, renamedfilename);
					// compress
					if( zip_the_files ) {
						result = CompressFile(renamedfilename);

						if( result < 0 ) {
							sprintf( errstring, "Error %i Compressing actual loggerfile ! ", result );
							logger_step = STOP_LOGGING;
							break;
						}
					}
					logger_step = WRITE_HEADER;
					break;


				case STOP_LOGGING:
					//			p_step_str = "STOP_LOGGING";
					free(cache);
					logger_is_running = FALSE;
					start_write_by_change = 0; // self stop
					SendMessage( MainWindowGetHandle(), NOTIFY_LOGGER_STATUS, 0, 0 );
					if( strcmp( errstring, "") ) {
						MessageBox( NULL, errstring, APPLICATION_NAME " - Logger",  MB_OK | MB_ICONERROR );
					}
					break;

				default:
//					p_step_str = "??";
					break;


			} /* end of switch(logger_step) */
			break; // exit loop
		} /* end of for(;;) */
	}
}




/* this function is called by timer interval */

static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	static FILE *f;
	static DWORD dwLastTime;
	int iDifftime;
	static int milliseconds;
	static int flushinterval;
	static long long int written_chars;
	int result;
	int i;
	static char *cache;
	static char errstring[130];
//    char *p_step_str;
	static char renamedfilename[MAX_PATH];

	iDifftime = dwTime - dwLastTime;
	if( iDifftime > 0 ) {
		milliseconds += iDifftime;
		if( milliseconds > 9999 ) {
			milliseconds -= 10000;
		}
		flushinterval += iDifftime;
	}


	switch (logger_step) {
		case START_LOGGING:
//			p_step_str = "START_LOGGING";
			milliseconds = 0;
			strcpy( errstring, "" );
			cache = malloc( CACHESIZE );
			if( cache == NULL ) {
				strcpy( errstring, "Logger: can't allocate cache. To less memory !" );
				logger_step = STOP_LOGGING;
				break;
			}
			logger_step = RENAME_OLD_FILE;
			break;


		case RENAME_OLD_FILE: // rename logger file if already exists
//			p_step_str = "RENAME_OLD_FILE";

			if( CreateBackupFilename( loggerfile, renamedfilename ) ) {
				rename( loggerfile, renamedfilename);

				if( zip_the_files ) {
					result = CompressFile(renamedfilename);

					if( result < 0 ) {
						sprintf( errstring, "Error %i Compressing actual loggerfile ! ", result );
						logger_step = STOP_LOGGING;
						break;
					}
				}
			}

			logger_step = WRITE_HEADER;
			break;


		case WRITE_HEADER: // write header into log
			//		p_step_str = "WRITE_HEADER";
			written_chars = 0;
			f = fopen( loggerfile, "w+" );
			if( f == NULL ) {
				strcpy( errstring,  "Logger: can't open logfile !" );
				logger_step = STOP_LOGGING;
				break;
			} else {
				result = fprintf( f, LOGFILE_HEADER );
				if( result > 0 ) {
					written_chars += result;
					for( i=0; i< no_of_logged_objects; ++i ) {
						if( logged_object[i]->type == POBJ_PVAR ) {

							if( logged_object[i]->ex.pv.task != NULL ) { // get name of task of variable
								PVIOBJECT *task = logged_object[i]->ex.pv.task;
								result = fprintf( f, ";%s:%s", task->descriptor, logged_object[i]->descriptor );
							} else { // global variable
								result = fprintf( f, ";%s", logged_object[i]->descriptor );
							}

							if( result < 0 ) {
								break;
							} else {
								written_chars += result;
							}
						}
					}
					fprintf( f, "\n" );
					fclose(f);
					f = NULL;
					logger_step = OPEN_FILE;
				}
			}

			if( result < 0 ) {
				// error during write
				strcpy( errstring, "Logger: Error when writing header to logfile !" );
				logger_step = STOP_LOGGING;
			}
			break;


		case OPEN_FILE: // open file and set cache size
//			p_step_str = "OPEN_FILE";
			f = fopen( loggerfile, "a+" );
			if( f == NULL ) {
				strcpy( errstring, "Logger: can't open logfile !" );
				logger_step = STOP_LOGGING;
			} else {
				setvbuf( f, cache, _IOFBF, CACHESIZE );
				logger_step = WRITE_TO_FILE;
				flushinterval = 0;
			}
			break;


		case WRITE_TO_FILE: // write into log
//			p_step_str = "WRITE_TO_FILE";

			for( i=0; i< no_of_logged_objects; ++i ) {
				if( logged_object[i]->ex.pv.value_changed ) {
					logged_object[i]->ex.pv.value_changed = 0;
				}
			}


			result = fprintf( f, "%4.4u/%2.2u/%2.2u-%2.2u:%2.2u:%2.2u;%4.4u",
			                  cpu_timemaster->ex.cpu.rtc_time.tm_year + 1900,
			                  cpu_timemaster->ex.cpu.rtc_time.tm_mon + 1,
			                  cpu_timemaster->ex.cpu.rtc_time.tm_mday,
			                  cpu_timemaster->ex.cpu.rtc_time.tm_hour,
			                  cpu_timemaster->ex.cpu.rtc_time.tm_min,
			                  cpu_timemaster->ex.cpu.rtc_time.tm_sec,
			                  milliseconds
			                );

			if( result >= 0 ) {
				written_chars += result;

				for( i=0; i< no_of_logged_objects; ++i ) {
					if( logged_object[i]->type == POBJ_PVAR ) {
						switch( logged_object[i]->ex.pv.type ) {
							case BR_USINT:
								result = fprintf( f, ";%u", * ( (unsigned char*) logged_object[i]->ex.pv.pvalue ) );
								break;

							case BR_SINT:
								result = fprintf( f, ";%i", * ( (signed char*) logged_object[i]->ex.pv.pvalue ) );
								break;

							case BR_UINT:
								result = fprintf( f, ";%u", * ( (unsigned short*) logged_object[i]->ex.pv.pvalue ) );
								break;

							case BR_INT:
								result = fprintf( f, ";%+i", * ( (signed short*) logged_object[i]->ex.pv.pvalue ) );
								break;

							case BR_DATI:
							case BR_UDINT:
							case BR_TIME:
							case BR_DATE:
							case BR_TOD:
								result = fprintf( f, ";%u", * ( (unsigned int*) logged_object[i]->ex.pv.pvalue ) );
								break;

							case BR_DINT:
								result = fprintf( f, ";%+i", * ( (signed int*) logged_object[i]->ex.pv.pvalue ) );
								break;

							case BR_BOOL:
								result = fprintf( f, ";%.1u", * ( (unsigned  char*) logged_object[i]->ex.pv.pvalue ) );
								break;

							case BR_REAL: {
								float real = * ((float*) logged_object[i]->ex.pv.pvalue );
								result = fprintf( f, ";%g", real );
							}
							break;

							case BR_LREAL: {
								double lreal = * ((double*) logged_object[i]->ex.pv.pvalue );
								result = fprintf( f, ";%g", lreal );
							}
							break;

							case BR_STRING:
								if( logged_object[i]->ex.pv.pvalue != NULL ) {
									char *s;
									char *d;
									char value[256];
									int count = 0;
									s = (char*) logged_object[i]->ex.pv.pvalue;
									d = value;
									while( *s && (count < sizeof(value)) ) {
										if( (*s & 0xff) < ' ' ) {
											sprintf( d, "\\x%3.3x", (*s & 0xff) );
											d+=5;
											++s;
											count += 5;
										} else {
											++count;
											if( logged_object[i]->gui_info.interpret_as_oem ) {
												OemToCharBuff( s++, d++, 1 );
											} else {
												*d++=*s++;
											}

										}
									}
									*d = 0;
									result = fprintf( f, ";\"%.120s\"", value );
								}
								break;
								
							case BR_WSTRING:
								if( logged_object[i]->ex.pv.pvalue != NULL ) {
									result = fwprintf( f, L";\"%.120s\"", logged_object[i]->ex.pv.pvalue );
								}
								break;

							default:
								result = 0;
								break;
						}
						if( result < 0 ) {
							break;
						} else {
							written_chars += result;
						}
					}
				}
			}
			fprintf( f, "\n" );

			if( flushinterval > 10000 ) {			// cyclically flush file
				flushinterval = 0;
				fflush(f);
			}

			if( result < 0 ) { // Fehler
				strcpy( errstring, "Logger: can't write into logfile !" );
				fclose(f);
				f = NULL;
				logger_step = STOP_LOGGING;
			}

			else if( !logger_is_running ) { // logger was stopped
				strcpy( errstring, "" );
				fclose(f);
				f = NULL;
				logger_step = STOP_LOGGING;
			}

			else if( written_chars >= (filesize * 1024) ) { // max. size of logger file reached ?
				fclose(f);
				f = NULL;
				logger_step = COMPRESS_FILE;
			}
			break;


		case COMPRESS_FILE:
//			p_step_str = "COMPRESS_FILE";

			result = CreateBackupFilename( loggerfile, renamedfilename );

			rename( loggerfile, renamedfilename);
			//zippen
			if( zip_the_files ) {
				result = CompressFile(renamedfilename);

				if( result < 0 ) {
					sprintf( errstring, "Error %i Compressing actual loggerfile ! ", result );
					logger_step = STOP_LOGGING;
					break;
				}
			}
			logger_step = WRITE_HEADER;
			break;


		case STOP_LOGGING:
//			p_step_str = "STOP_LOGGING";
			KillTimer( NULL, idEvent ); // function kills itself
			free(cache);
			logger_is_running = FALSE;
			SendMessage( MainWindowGetHandle(), NOTIFY_LOGGER_STATUS, 0, 0 );
			if( strcmp( errstring, "") ) {
				MessageBox( NULL, errstring, APPLICATION_NAME " - Logger",  MB_OK | MB_ICONERROR );
			}
			break;

		default:
//			p_step_str = "??";
			break;

	}

//	sprintf( tempstring, "%s:%u", p_step_str, written_chars );
//	SetWindowText( GetMainWindow(), tempstring );
	dwLastTime = dwTime;

}



/* logger configuration dialog */

BOOL CALLBACK  LoggerConfigDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	char tempstring[512];

	switch (uMsg) {
		case WM_INITDIALOG:
			ReadSettings();
			CheckDlgButton( hDlg, IDC_CHECK_ZIP, zip_the_files == TRUE ? BST_CHECKED : BST_UNCHECKED );
			SetDlgItemText( hDlg, IDR_EDIT_FILENAME, loggerfile );
			SetDlgItemInt( hDlg, IDR_EDIT_MAXSIZE, filesize, 0 );
			SetDlgItemInt( hDlg, IDR_EDIT_CYCLETIME, cycletime, 0 );
			CheckDlgButton( hDlg, IDC_CHECK_CHANGED, write_by_change == TRUE ? BST_CHECKED : BST_UNCHECKED );
			EnableWindow( GetDlgItem( hDlg, IDR_EDIT_CYCLETIME ), write_by_change == TRUE ? FALSE : TRUE );


			if( cpu_timemaster == NULL ) {
				strcpy( tempstring, "no cpu in watchlist !" );
			} else {
				sprintf( tempstring, "%s %s %s", cpu_timemaster->ex.cpu.cputype, cpu_timemaster->ex.cpu.arversion, cpu_timemaster->descriptor );
			}
			SetDlgItemText( hDlg, IDR_EDIT_TIMESTAMP_CPU, tempstring );
			SetDlgItemInt( hDlg, IDR_EDIT_OBJECT_COUNT, CountLoggedObjects(), 0 );

			if( logger_is_running ) { // bei laufendem Logger nur schauen erlaubt
				EnableWindow( GetDlgItem( hDlg, IDR_EDIT_FILENAME ), FALSE );
				EnableWindow( GetDlgItem( hDlg, IDR_EDIT_MAXSIZE ), FALSE );
				EnableWindow( GetDlgItem( hDlg, IDR_EDIT_CYCLETIME ), FALSE );
				EnableWindow( GetDlgItem( hDlg, IDOK ), FALSE );
				EnableWindow( GetDlgItem( hDlg, IDC_CHECK_ZIP), FALSE );
				EnableWindow( GetDlgItem( hDlg, IDC_CHECK_CHANGED), FALSE );
			}
			return TRUE;

		case WM_COMMAND:
			switch (wParam) {
				case IDOK:
					GetDlgItemText( hDlg, IDR_EDIT_FILENAME, loggerfile, sizeof(loggerfile) );
					GetDlgItemText( hDlg, IDR_EDIT_MAXSIZE, tempstring, sizeof(tempstring) );
					filesize = atoi(tempstring);


					GetDlgItemText( hDlg, IDR_EDIT_CYCLETIME, tempstring, sizeof(tempstring) );
					cycletime = atoi(tempstring);

					if( ValidateSettings() ) {
						WritePrivateProfileString( "Logger", "Filename", loggerfile, SettingsGetFileName() );
						sprintf( tempstring, "%u", filesize );
						WritePrivateProfileString( "Logger", "maxfilesize", tempstring, SettingsGetFileName() );
						sprintf( tempstring, "%u", cycletime );
						WritePrivateProfileString( "Logger", "cycletime", tempstring, SettingsGetFileName() );
						zip_the_files = IsDlgButtonChecked( hDlg, IDC_CHECK_ZIP ) == BST_CHECKED ? TRUE : FALSE;
						WritePrivateProfileString( "Logger", "zip", zip_the_files == TRUE ? "1" : "0", SettingsGetFileName() );
						write_by_change = IsDlgButtonChecked( hDlg, IDC_CHECK_CHANGED ) == BST_CHECKED ? TRUE : FALSE;
						WritePrivateProfileString( "Logger", "write_by_change", write_by_change == TRUE ? "1" : "0", SettingsGetFileName() );

						EndDialog(hDlg, TRUE );
						return TRUE;
					}
					return FALSE;


				case IDCANCEL:
					/*
					 * OK or Cancel was clicked, close the dialog.
					 */
					EndDialog(hDlg, TRUE);
					return TRUE;

				case IDR_BUTTON_FILESELECT: {
					OPENFILENAME ofn;
					memset(&ofn, 0, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = hDlg;
					ofn.lpstrFilter = "Comma Separated File\0*.csv\0\0";
					ofn.lpstrFile = loggerfile;
					ofn.lpstrDefExt = "csv";
					ofn.nMaxFile = sizeof(loggerfile);
					ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
					if (GetSaveFileName(&ofn)) {
						SetDlgItemText( hDlg, IDR_EDIT_FILENAME, loggerfile );
					}
				}
				return TRUE;

				case IDC_CHECK_CHANGED:
					/* if 'write on change' is selected we need to disable the field for cycle time */
					if( IsDlgButtonChecked( hDlg, IDC_CHECK_CHANGED ) == BST_CHECKED ) {
						EnableWindow( GetDlgItem( hDlg, IDR_EDIT_CYCLETIME ), FALSE );
						write_by_change = 1;
					} else {
						EnableWindow( GetDlgItem( hDlg, IDR_EDIT_CYCLETIME ), TRUE );
						write_by_change = 0;
					}
					return TRUE;

			}
			break;
	}

	return FALSE;
}


/* switches on/off the logger */

BOOL Logger( void ) {
	int i;

	if( logger_is_running ) {		// is logger already running ?
		logger_is_running = FALSE;  // switch it off
		if( write_by_change ) {
			for( i = 0; i < 10; ++i ) { // give chance to close file
				LoggerDataChanged( NULL );
			}
		}
	} else {	// Logger wird gestartet;
		ReadSettings();
		CountLoggedObjects();
		if( ValidateSettings() == FALSE )
			logger_is_running = FALSE;  // could not be started
		else {
			logger_is_running = TRUE;
			logger_step = START_LOGGING;
			if( write_by_change == 0) { // do not start timer procedure when 'write on change' is selected
				SetTimer( NULL, 0, cycletime, TimerProc );
			} else {
				start_write_by_change = TRUE; // 'write on change'
				for( i=0; i< no_of_logged_objects; ++i ) {
					LoggerDataChanged( logged_object[i] );
				}
			}
		}
	}


	return logger_is_running;
}

