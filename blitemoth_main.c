#include "blitemoth_cli.h"
#include <stdio.h>

int main(int argc, char * const argv[]) {
	int status;
	setup_t *setup = interpret_argv(argc, argv, &status);
	if (status != CLI_OK) {
		free_setup(setup);
		fprintf(stderr, "Something went wrong...\n");
	}
	return 0;
}
