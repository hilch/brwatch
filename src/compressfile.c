#include "main.h"
#include "compressfile.h"
#include "zlib.h"


int CompressFile( char * filename ) {
	char outfilename[MAX_PATH];
	char buf[256];
	gzFile out;
	FILE * in;
	unsigned long len;


	strcpy( outfilename, filename );
	strcat( outfilename, ".gz" );

	// open source file
	in = fopen( filename, "rb" );
	if( in == NULL )
		return -1;

	// open output file

	out = gzopen( outfilename, "wb9f" );

	if( out == NULL ) {
		fclose(in);
		return -2;
	}

	// compress
	for (;;) {
		len = (int)fread(buf, 1, sizeof(buf), in);
		if (ferror(in)) {
			fclose(in);
			gzclose(out);
			return -3;
		}
		if (len == 0)
			break; // End of file...
		if (gzwrite(out, buf, (unsigned)len) != len) {
			//error(gzerror(out, &err));
			fclose(in);
			gzclose(out);
			return -4;
		}
	}

	if( fclose(in) == EOF ) {
		gzclose(out);
		return -5;
	}

	if( gzclose(out) != 0 ) {
		return -6;
	}

	unlink(filename);
	return 0;
}


int DecompressFile( char *zipfilename ) {
	char drive[3];
	char directory[FILENAME_MAX];
	char filename[FILENAME_MAX];
	char extension[FILENAME_MAX];
	char outfilename[MAX_PATH];
	gzFile in;
	FILE * out;
	unsigned long len;
	char buf[256];

	_splitpath( zipfilename, drive, directory, filename, extension );

	// open source file
	in = gzopen( zipfilename, "rb" );

	if( in == NULL )
		return -2;

	// open output file
	snprintf( outfilename, sizeof(outfilename), "%s%s%s", drive, directory, filename );
	out = fopen( outfilename, "w+b" );

	if( out == NULL ) {
		return -3;
	}

	// decompress
	for (;;) {
		len = gzread(in, buf, sizeof(buf));
		if (len < 0) { // error
			fclose(out);
			gzclose(in);
			return -4;
		}

		if (len == 0)
			break; // EOF

		if ((int)fwrite(buf, 1, (unsigned)len, out) != len) {
			fclose(out);
			gzclose(in);
			return -5;
		}
	}
	fclose(out);
	gzclose(in);
	return 0;

}
