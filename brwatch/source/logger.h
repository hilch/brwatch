
#ifndef __LOGGER_H
#define __LOGGER_H

BOOL CALLBACK LoggerConfigDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL Logger( void );
BOOL IsLoggerRunning( void );
void LoggerDataChanged( PVIOBJECT *object );

#define LOGFILE_HEADER				"cycle@rtc(yyyy/mm/dd-hh:mm:ss);ticks(ms)"
#define LOGFILE_HEADER_BY_CHANGE	"event@rtc(yyyy/mm/dd-hh:mm:ss);ticks(ms);name;value"

#endif

