#include <windows.h>
#include <stdio.h>
#include "zlib.h"

#define USE_STATIC_ZIP_LIB


BOOL ZipDllFound( void );
int CompressFile( char * file );
int DecompressFile( char *zipfilename );
const char *GetZibLibVersion( void );
