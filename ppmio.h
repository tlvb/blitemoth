#ifndef __PNMIO_H__
#define __PNMIO_H__ 1

#include <stdint.h>
#include <stdio.h>

#define PPM_OK 0
#define PPM_FILEERROR 1
#define PPM_BADFILE 2
#define PPM_BADALLOC 3

#define ppm_data(i,x,y,j) (i->data[i->width*y+x].c[j])

typedef union {
	struct {
		uint8_t r,g,b;
	} n;
	uint8_t c[3];
} rgb888_t;

typedef struct {
	unsigned int width;
	unsigned int height;
	rgb888_t *data;
} ppm_t;

void ignorewhitespace(FILE *f);
void ignorecomment(FILE *f);
ppm_t *ppm_read(ppm_t *recycle, char *fn, int *status);
int ppm_write(char *fn, ppm_t *img);
ppm_t *ppm_free(ppm_t *prisoner);

#endif
