#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H
char* int2bin( unsigned long long i , int bits );
int CountToken( char *s, char token );
int FindToken( char **source, char* token );
int GetIntValue( char **source );
int validate_ip4(const char* buffer);
#endif // STRINGTOOLS_H
