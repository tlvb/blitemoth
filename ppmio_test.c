#include "ppmio.h"
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

int test1_setup() { /*{{{*/
	size_t n = 3 + 4 + 14 + 4 + (3*3*3);
	unsigned char data[] = {'P','6','\n',
		'3',' ','3','\n',
		'#','s','o','m','e',' ','c','o','m','m','e','n','t','\n',
		'2','5','5','\n',
		  0,   1,    2,    10,  11,  12,     20,  21,  22,
		100, 101, 102,    110, 111, 112,    120, 121, 122,
		200, 201, 202,    210, 211, 212,    220, 221, 222
	};
	assert(222 == data[n-1]);
	FILE *f = fopen("test.ppm", "r");
	if (f != NULL) {
		fclose(f);
		fprintf(stderr, "testfile exists, aborting test\n");
		return 1;
	}
	f = fopen("test.ppm", "w");
	if (f == NULL) {
		fprintf(stderr, "could not open file in write mode, aborting test\n");
		return 1;
	}
	if (n != fwrite(data, sizeof(char), n, f)) {
		fprintf(stderr, "could not write all data to file, aborting test\n");
		return 1;
	}
	fclose(f);
	return 0;
} /*}}}*/
int test1() { /*{{{*/
	ppm_t *img = NULL;
	int flag = PPM_OK;
	fprintf(stderr, "calling ppm_read(...)\n");
	img = ppm_read(img, "test.ppm", &flag);
	fprintf(stderr, "returned from ppm_read(...)\n");
	if (PPM_OK != flag) {
		switch (flag) {
			case PPM_FILEERROR:
				fprintf(stderr, "could not open file\n");
				break;
			case PPM_BADFILE:
				fprintf(stderr, "unsupported file format or broken file\n");
				break;
			case PPM_ALLOCFAIL:
				fprintf(stderr, "could not allocate memory\n");
				break;
		}
		ppm_free(img);
		return 1;
	}
	else if (img == NULL) {
		fprintf(stderr, "ppm_read(...) returned NULL for no apparent reason\n");
		return 1;
	}
	if(img->width != 3) {
		fprintf(stderr, "width is %d, expected 3\n", img->width);
	}
	if(img->height != 3) {
		fprintf(stderr, "height is %d, expected 3\n", img->width);
	}
	for (unsigned int y=0; y<img->height; ++y) {
		for (unsigned int x=0; x<img->width; ++x) {
			for (int c=0; c<3; ++c) {
				int expected = y*100 + x*10 + c;
				int actual = ppm_data(img, x, y, c);
				if (actual != expected) {
					fprintf(stderr, "pixel (%d,%d).%d expected: %d, actual %d\n", x, y, c, expected, actual);
					ppm_free(img);
					return 1;
				}
			}
		}
	}
	fprintf(stderr, "test 1 ok\n");
	ppm_free(img);
	return 0;
} /*}}}*/
int test1_teardown() { /*{{{*/
	if (0 != unlink("test.ppm")) {
		fprintf(stderr, "could not unlink file\n");
		return 1;
	}
	return 0;
} /*}}}*/

int main(int argc, char **argv) {
	fprintf(stderr, "TEST main()\n");
	if (test1_setup() != 0) {
		fprintf(stderr, "test 1 SETUP error\n");
		return 1;
	}
	fprintf(stderr, "test 1 SETUP done\n");
	if (test1() != 0) {
		fprintf(stderr, "test 1 ACTUAL error\n");
		return 1;
	}
	fprintf(stderr, "test 1 ACTUAL done\n");
	if (test1_teardown() != 0) {
		fprintf(stderr, "test 1 TEARDOWN error\n");
		return 1;
	}
	fprintf(stderr, "test 1 TEARDOWN done\n");
	return 0;
}
