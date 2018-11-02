#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include "main.h"
#include "zip.h"


#define USE_STATIC_ZIP_LIB

static BOOL zip_dll_found = FALSE;
static gzFile (*_gzopen) (const char *path, const char *mode);
static int (*_gzwrite) (gzFile file, voidpc buf, unsigned len);
static  int (*_gzread) (gzFile file, voidp buf, unsigned len);
static int (*_gzclose) (gzFile file);
static const char* (*_gzerror) (gzFile file, int *errnum);
static const char* (* _zlibVersion) (void);


static int InitZLib( void )
{
#ifdef USE_STATIC_ZIP_LIB
    _gzopen = gzopen;
    _gzwrite = gzwrite;
    _gzread = gzread;
    _gzclose = gzclose;
    _gzerror = gzerror;
    _zlibVersion = zlibVersion;
#else

    HMODULE hmod;
    if( zip_dll_found )
        return 0;

    hmod = LoadLibrary( "zlib1.dll" );
    if( hmod == NULL )
    {
        return -1;
    }

    _gzopen = (void*) GetProcAddress( hmod, "gzopen" );

    if( _gzopen == NULL )
    {
        return -1;
    }

    _gzwrite = (void*) GetProcAddress( hmod, "gzwrite" );

    if( _gzwrite == NULL )
    {
        return -1;
    }

    _gzread = (void*) GetProcAddress( hmod, "gzread" );

    if( _gzread == NULL )
    {
        return -1;
    }

    _gzclose = (void*) GetProcAddress( hmod, "gzclose" );

    if( _gzclose == NULL )
    {
        return -1;
    }

    _gzerror = (void*) GetProcAddress( hmod, "gzerror" );

    if( _gzerror == NULL )
    {
        return -1;
    }

    _zlibVersion = (void*) GetProcAddress( hmod, "zlibVersion" );

    if( _zlibVersion == NULL )
    {
        return -1;
    }

#endif
    zip_dll_found = TRUE;
    return 0;
}


BOOL ZipDllFound( void )
{
    if( InitZLib() == 0 )
        return TRUE;
    else
        return FALSE;
}


const char *GetZibLibVersion( void )
{
    static char version[30];

    if( InitZLib() == 0 )
    {
        strcpy( version, "zlib V" );
        strcat( version, _zlibVersion() );
        return version;
    }
    else
    {
        return "Zip-dll not found !";
    }
}

int CompressFile( char * filename )
{
    char outfilename[MAX_PATH];
    char buf[256];
    gzFile out;
    FILE * in;
    unsigned long len;

    if( InitZLib() != 0 )
        return -1;

    strcpy( outfilename, filename );
    strcat( outfilename, ".gz" );

    // Eingabedatei öffnen
    in = fopen( filename, "rb" );
    if( in == NULL )
        return -1;

    // Ausgabedatei öffnen

    out = _gzopen( outfilename, "wb9f" );

    if( out == NULL )
    {
        fclose(in);
        return -2;
    }

    // komprimieren
    for (;;)
    {
        len = (int)fread(buf, 1, sizeof(buf), in);
        if (ferror(in))
        {
            fclose(in);
            _gzclose(out);
            return -3;
        }
        if (len == 0)
            break; // End of file...
        if (_gzwrite(out, buf, (unsigned)len) != len)
        {
            //error(gzerror(out, &err));
            fclose(in);
            _gzclose(out);
            return -4;
        }
    }

    if( fclose(in) == EOF )
    {
        _gzclose(out);
        return -5;
    }

    if( _gzclose(out) != 0 )
    {
        return -6;
    }

    unlink(filename);
    return 0;
}


int DecompressFile( char *zipfilename )
{
    char drive[3];
    char directory[FILENAME_MAX];
    char filename[FILENAME_MAX];
    char extension[FILENAME_MAX];
    char outfilename[MAX_PATH];
    gzFile in;
    FILE * out;
    unsigned long len;
    char buf[256];

    if( InitZLib() != 0 )
        return -1;

    _splitpath( zipfilename, drive, directory, filename, extension );

    // Eingabedatei öffnen
    in = _gzopen( zipfilename, "rb" );

    if( in == NULL )
        return -2;

    // Ausgabedatei öffnen
    sprintf( outfilename, "%s%s%s", drive, directory, filename );
    out = fopen( outfilename, "w+b" );

    if( out == NULL )
    {
        return -3;
    }

    // de- komprimieren
    for (;;)
    {
        len = _gzread(in, buf, sizeof(buf));
        if (len < 0)   // Fehler
        {
            fclose(out);
            _gzclose(in);
            return -4;
        }

        if (len == 0)
            break; // EOF

        if ((int)fwrite(buf, 1, (unsigned)len, out) != len)
        {
            fclose(out);
            _gzclose(in);
            return -5;
        }
    }
    fclose(out);
    _gzclose(in);
    return 0;

}
