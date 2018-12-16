/*
 * gziphm.c: Fast version of gzip using only Huffman coding.
 * For raw sequence data in bioinformatics this method gives better results than regular gzip
 * Note: gziphm.c is based on on zpipe.c obtained from https://www.zlib.net/zpipe.c
 *
 * Changes made by Steffen Mikkelsen (https://github.com/smikkelsendk/gziphm)
 * Version 1.0. Initial release
 */
#include <stdio.h>
#include "zlib.h"
#include <string.h>

#define CHUNK 16384

int def(FILE * source, FILE * dest)
{
	int ret, flush;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 9, Z_HUFFMAN_ONLY);
	if (ret != Z_OK)
		return ret;

	/* compress until end of file */
	do {
		strm.avail_in = fread(in, 1, CHUNK, source);
		if (ferror(source)) {
			(void) deflateEnd(&strm);
			return Z_ERRNO;
		}
		flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;

		/* run deflate() on input until output buffer not full, finish
		   compression if all of source has been read in */
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = deflate(&strm, flush);	/* no bad return value */

			have = CHUNK - strm.avail_out;
			if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
				(void) deflateEnd(&strm);
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);

		/* done when last data in file processed */
	} while (flush != Z_FINISH);

	/* clean up and return */
	(void) deflateEnd(&strm);
	return Z_OK;
}

int main(int argc, char **argv)
{
	int ret;

	if (argc == 2 && strcmp(argv[1], "-") == 0) {

		ret = def(stdin, stdout);
		if (ret != Z_OK) {
			printf("Error compressing", stderr);
		}
	} else if (argc >= 2) {
		int fret = Z_OK;
		int i;
		for (i = 1; i < argc; i++) {
			FILE *ifp, *ofp;

			ifp = fopen(argv[i], "r");
			if (!ifp) {
				printf("Failed to open %s\n", argv[i]);
				fret = 1;
				continue;
			}
			char output[strlen(argv[i]) + 4];
			strcpy(output, argv[i]);
			strcat(output, ".gz");
			ofp = fopen(output, "w");
			if (!ofp) {
				printf("Failed to create %s\n", output);
				fret = 1;
				continue;
			}

			ret = def(ifp, ofp);

			fclose(ifp);
			fclose(ofp);
			if (ret != Z_OK) {
				fret = ret;
				printf(strcat("Error compressing ", argv[i]), stderr);
			}

		}
		return fret;

	} else {
		printf("Usage: %s [FILE]...\n", argv[0]);
		printf("  when FILE is -, read standard input.\n");
		return 1;
	}
}
