#include "ppmio.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

void ignorewhitespace(FILE *f) { /*{{{*/
	int c = 0;
	do {
		c = fgetc(f);
	} while (isspace(c));
	ungetc(c, f);
} /*}}}*/
void ignorecomment(FILE *f) { /*{{{*/
	int t = fgetc(f);
	if (t == '#') {
		while (fgetc(f) != '\n');
	}
	else {
		ungetc(t, f);
	}
	ignorewhitespace(f);
} /*}}}*/
ppm_t *ppm_read(ppm_t *recycle, char *fn, int *status) { /*{{{*/
	FILE *f = fopen(fn, "r");
	if (f == NULL) {
		*status = PPM_FILEERROR;
		return recycle;
	}

	/* magic {{{ */
	unsigned int magic;
	if (1 != fscanf(f, "P%u", &magic)) {
		fclose(f);
		*status = PPM_BADFILE;
		return recycle;
	}
	if (magic != 6) { /* only binary ppm supported currently */
		fclose(f);
		*status = PPM_BADFILE;
		return recycle;
	}
	ignorewhitespace(f);
	ignorecomment(f);
	/* }}} */
	/* width and height {{{ */
	unsigned int width, height, count;
	if (2 != fscanf(f, "%u %u", &width, &height)) {
		fclose(f);
		*status = PPM_BADFILE;
		return recycle;
	}

	count = width*height;
	ignorewhitespace(f);
	ignorecomment(f);
	/* }}} */
	/* maxval {{{ */
	unsigned int maxval;
	if (1 != fscanf(f, "%u%*c", &maxval)) { //NOTE: *c to read the 1 char whitespace
		fclose(f);
		*status = PPM_BADFILE;
		return recycle;
	}
	if (maxval != 255) {
		fclose(f);
		*status = PPM_BADFILE;
		return recycle;
	}
	/* }}} */
	/* allocate memory {{{ */
	if (recycle == NULL) {
		recycle = calloc(1, sizeof(ppm_t));
		if (recycle == NULL) {
			fclose(f);
			*status = PPM_ALLOCFAIL;
			return recycle;
		}
	}
	if (recycle->data != NULL) {
		if (count != recycle->width * recycle->height) {
			free(recycle->data);
			recycle->data = malloc(count*sizeof(rgb888_t));
			if (recycle->data == NULL) {
				fclose(f);
				*status = PPM_ALLOCFAIL;
				return recycle;
			}
		}
	}
	else {
		recycle->data = malloc(count*sizeof(rgb888_t));
		if (recycle->data == NULL) {
			fclose(f);
			*status = PPM_ALLOCFAIL;
			return recycle;
		}
	}
	/* }}} */
	recycle->width = width;
	recycle->height = height;
	if (count != fread(recycle->data, sizeof(rgb888_t), count, f)) {
		fclose(f);
		*status = PPM_BADFILE;
		return recycle;
	}
	*status = PPM_OK;
	return recycle;
} /*}}}*/
int ppm_write(char *fn, ppm_t *img) { /*{{{*/
	return 0;
} /*}}}*/
ppm_t *ppm_free(ppm_t *prisoner) { /*{{{*/
	if (prisoner != NULL) {
		if (prisoner->data != NULL) {
			free(prisoner->data);
		}
		free(prisoner);
	}
	return NULL;
} /*}}}*/

