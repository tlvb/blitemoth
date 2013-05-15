#include "blitemoth_cli.h"
#include "blitemoth_info.h"
#include "blitemoth_engine.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define badreturnval(...) erpr("called function returned an error: " __VA_ARGS__)
#define badalloc(...) erpr("memory allocation failure\n")
#define erpr(...) fprintf(stderr, "CLI :: " __VA_ARGS__)

setup_t *interpret_argv(int argc, char * const argv[], int *status) {
	*status = CLI_OK;
	setup_t *setup = calloc(1, sizeof(setup_t));
	if (setup == NULL) {
		badalloc();
		*status = CLI_BADALLOC;
		return setup;
	}
	const char *optstr = "Vvi:o:m:c:";
	opterr = 0;
	int opt = getopt(argc, argv, optstr);
	int actionstatus = ENG_OK;

	while (opt > -1) {
		action_t *action = NULL;
		action_t **tmp = NULL;
		switch (opt) {
			case 'V': /* version/info {{{ */
				fprintf(stderr, BLITEMOTH_INFO_STRING);
				break; /* }}} */
			case 'v': /* verbose {{{ */
				setup->verbose = true;
				break; /* }}} */
			case 'i': /* input file {{{ */
				setup->file.input = calloc(strlen(optarg)+1, sizeof(char));
				if (setup->file.input == NULL) { /*{{{*/
					badalloc();
					*status = CLI_BADALLOC;
					return setup;
				} /*}}}*/
				strcpy(setup->file.input, optarg);
				break; /* }}} */
			case 'o': /* output file {{{ */
				setup->file.output = calloc(strlen(optarg)+1, sizeof(char));
				if (setup->file.output == NULL) { /*{{{*/
					badalloc();
					*status = CLI_BADALLOC;
					return setup;
				} /*}}}*/
				strcpy(setup->file.output, optarg);
				break; /* }}} */
			case 'm': /* multiple file range {{{ */
				setup->file.range = true;
				int n = sscanf(optarg, "%u %u",
						&(setup->file.begin),
						&(setup->file.end));
				switch (n) {
					case 0:
						erpr("argument \"-m\" needs one or two positive numbers\n");
						break;
					case 1:
						setup->file.end = setup->file.begin;
						setup->file.begin = 0;
						break;
				}
				break; /* }}} */
			case 'c': /* all-channel work order {{{ */
				action = parse_action(optarg, &actionstatus);
				if (actionstatus != ENG_OK) { /*{{{*/
					free_action(action);
					switch (actionstatus) {
						case ENG_BADFORM:
							badreturnval("parameter to \"-c\" has bad syntax\n");
							*status = CLI_BADPARM;
							return setup;
							break;
						case ENG_BADALLOC:
							badreturnval("memory\n");
							*status = CLI_BADALLOC;
							return setup;
							break;
					}
				} /*}}}*/
				setup->work.count += 1;
				tmp = realloc(setup->work.action, setup->work.count*sizeof(action_t*));
				if (tmp == NULL) { /*{{{*/
					badalloc();
					free_action(action);
					setup->work.count -= 1;
					*status = CLI_BADALLOC;
					return setup;
				} /*}}}*/
				setup->work.action = tmp;
				setup->work.action[setup->work.count-1] = action;
				break; /* }}} */
			case '?': /* now I am confused! {{{ */
				erpr("Missing argument to parameter \"-%c\" or parameter \"%c\" not recognized.\n", optopt, optopt);
				*status = CLI_CONFUSED;
				return setup;
				break; /* }}} */
		}
		opt = getopt(argc, argv, optstr);
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
		if (prisoner->work.action != NULL) {
			for (unsigned int i=0; i<prisoner->work.count; ++i) {
				free_action(prisoner->work.action[i]);
			}
			free(prisoner->work.action);
		}
		free(prisoner);
	}
}

#undef erpr
#undef badalloc
#undef badreturnval
