#include "blitemoth_cli.h"
#include "blitemoth_info.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

setup_t *interpret_argv(int argc, char * const argv[], int *status) {
	*status = CLI_OK;
	setup_t *setup = calloc(1, sizeof(setup_t));
	if (setup == NULL) {
		*status = CLI_BADALLOC;
		return setup;
	}
	const char *optstr = "Vvi:o:r:";
	opterr = 0;
	int opt = getopt(argc, argv, optstr);
	while (opt > -1) {
		switch (opt) {
			case 'V': /* version/info {{{ */
				fprintf(stderr, BLITEMOTH_INFO_STRING);
				break; /* }}} */
			case 'v': /* verbose {{{ */
				setup->verbose = true;
				break; /* }}} */
			case 'i': /* input file {{{ */
				setup->file.input = calloc(strlen(optarg)+1, sizeof(char));
				if (setup->file.input == NULL) {
					*status = CLI_BADALLOC;
					return setup;
				}
				strcpy(setup->file.input, optarg);
				break; /* }}} */
			case 'o': /* output file {{{ */
				setup->file.output = calloc(strlen(optarg)+1, sizeof(char));
				if (setup->file.output == NULL) {
					*status = CLI_BADALLOC;
					return setup;
				}
				strcpy(setup->file.output, optarg);
				break; /* }}} */
			case 'r': /* file range {{{ */
				setup->file.range = true;
				int n = sscanf(optarg, "%u %u",
						&(setup->file.begin),
						&(setup->file.end));
				switch (n) {
					case 0:
						fprintf(stderr, "-r needs 1 or two numbers\n");
						break;
					case 1:
						setup->file.end = setup->file.begin;
						setup->file.begin = 0;
						break;
				}
				break; /* }}} */
			case '?': /* now I am confused! {{{ */
				fprintf(stderr, "Missing argument to parameter -%c\n", optopt);
				*status = CLI_CONFUSED;
				return setup;
				break; /* }}} */
		}
		opt = getopt(argc, argv, optstr);
	}

	if ((setup->file.input == NULL) || (setup->file.output == NULL)) {
		fprintf(stderr, "Either an input or output file spec is missing, or both.\n");
		*status = CLI_CONFUSED;
		return setup;
	}

	if (setup->verbose) {
		if (setup->file.input != NULL) { /*{{{*/
			if (setup->file.range) {
				fprintf(stderr, "Input: files '");
				fprintf(stderr, setup->file.input, setup->file.begin);
				fprintf(stderr, "' to '");
				fprintf(stderr, setup->file.input, setup->file.end);
				fprintf(stderr, "' inclusive.\n");
			}
			else {
				fprintf(stderr, "Input: '%s'\n", setup->file.input);
			}
		} /*}}}*/
		if (setup->file.output != NULL) { /*{{{*/
			if (setup->file.range) {
				fprintf(stderr, "Output: files '");
				fprintf(stderr, setup->file.output, setup->file.begin);
				fprintf(stderr, "' to '");
				fprintf(stderr, setup->file.output, setup->file.end);
				fprintf(stderr, "' inclusive.\n");
			}
			else {
				fprintf(stderr, "Output: '%s'\n", setup->file.output);
			}
		} /*}}}*/
	}
	return setup;
}

setup_t *free_setup(setup_t *prisoner) {
	if (prisoner != NULL) {
		if (prisoner->file.input != NULL)
			free(prisoner->file.input);
		if (prisoner->file.output != NULL)
			free(prisoner->file.output);
		free(prisoner);
	}
}
