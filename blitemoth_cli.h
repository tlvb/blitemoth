#ifndef __BLITEMOTH_CLI_H__
#define __BLITEMOTH_CLI_H__ 1

#include <stdint.h>
#include <stdbool.h>

#define CLI_OK 0
#define CLI_BADALLOC 1
#define CLI_BADPARM 2
#define CLI_CONFUSED 3

typedef struct {
	bool verbose;
	struct {
		char *input;
		char *output;
		bool range;
		unsigned int begin;
		unsigned int end;
	} file;
} setup_t;

setup_t *interpret_argv(int argc, char * const argv[], int *status);
setup_t *free_setup(setup_t *prisoner);
#endif
