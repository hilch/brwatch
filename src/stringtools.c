#define WIN32_LEAN_AND_MEAN  /* speed up compilations */
#include <string.h>

char* int2bin( unsigned long long i, int bits )
{
    static char bin[40];
    char *d=bin;

    while( bits-- )
    {
        *d++ = i & 1 ? '1' : '0';
        i>>=1;
        if( bits == 28 || bits == 24 || bits == 20 || bits == 16 || bits == 12 || bits == 8 || bits == 4 )
        {
            *d++='_';
        }
    } ;
    *d = 0;
    _strrev(bin);

    return(bin);
}
