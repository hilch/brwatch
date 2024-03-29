#include <string.h>
#include <stdlib.h>

char* int2bin( unsigned long long i, int bits ) {
	static char bin[40];
	char *d=bin;

	while( bits-- ) {
		*d++ = i & 1 ? '1' : '0';
		i>>=1;
		if( bits == 28 || bits == 24 || bits == 20 || bits == 16 || bits == 12 || bits == 8 || bits == 4 ) {
			*d++='_';
		}
	} ;
	*d = 0;
	_strrev(bin);

	return(bin);
}




int CountToken( char *s, char token ) {
	int count = 0;
	while( *s ) {
		if( *s == token )
			++count;
		++s;
	}
	return count;
}

int FindToken( char **source, char* token ) {
	size_t length;
	char *s;

	length = strlen(token);
	s = *source;

	while( *s && *token ) {
		if( *s++ == *token++ )
			--length;
		else
			break;
	}

	if( length == 0 ) {
		*source = s;
		return 1;
	}
	return 0;
}


int GetIntValue( char **source ) {
	char tempstring[256];
	int i;


	i = 0;
	while( **source && **source != ' ' && **source != '\t' && i <sizeof(tempstring) ) {
		tempstring[i++] = * (*source)++;
	}
	tempstring[i] = 0;
	return(atoi(tempstring));
}


int validate_ip4(const char* buffer) {
	if (NULL == buffer) return 0;

	const      char*   pos     = buffer;
	unsigned   char    ch      = *pos;
	unsigned   short   count   = 0;

	while (ch != 0) {
		if (!((ch >= '0' && ch <= '9') || ch == '.')) return 0;

		if (ch == '.')
			if (++count > 3) return 0;

		ch = *++pos;
	}

	if (count == 3 && *--pos != '.') return 1;

	return 0;
}



int ends_with(const char *str, const char *suffix) {
	if (!str || !suffix)
		return 0;
	size_t lenstr = strlen(str);
	size_t lensuffix = strlen(suffix);
	if (lensuffix > lenstr)
		return 0;
	return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}