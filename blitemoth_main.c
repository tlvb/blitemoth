#include "blitemoth_parser.h"
#include <stdio.h>

int main(int argc, char * const argv[]) {
	int status;
	setup_t *setup = interpret_argv(argc, argv, &status);
	if (status != PARSER_OK) {
		free_setup(setup);
		fprintf(stderr, "something went wrong...\n");
		return 1;
	}
	return 0;
}
