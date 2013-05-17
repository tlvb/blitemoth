#include "blitemoth_parser.h"
#include "blitemoth_info.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#define badalloc() erpr("memory allocation failure\n")
#define erpr(...) fprintf(stderr, "parser error :: " __VA_ARGS__)

void showerr(char *text, unsigned int accum) { /*{{{*/
	erpr("\"%s\"\n", text);
	erpr(" ");
	int a = accum;
	while (a-- > 0)
		fputc('.', stderr);
	fprintf(stderr,"\x1b[1;31m^\x1b[0mHERE (character %d)\n", accum);
} /*}}}*/
int match_word(char *text, char *words, unsigned int *accum) { /*{{{*/
	if (strlen(text) > *accum) {;
		char tmp[32]; // some arbitrary large enough value for the implementation in order to avoid heap management overhead
		int delta = 0;
		int i = 0;
		char *wordsp = words;
		char *wordsq;
		do {
			wordsq = strchr(wordsp, ',');
			unsigned int len;
			if (wordsq == NULL)
				len = strlen(wordsp);
			else
				len = wordsq-wordsp;
			if (sscanf(text+*accum, "%31s%n", tmp, &delta) != 0) {
				if ((strlen(tmp) == len) && (strncmp(tmp, wordsp, len) == 0)) {
					*accum += delta;
					return i;
				}
			}
			wordsp = wordsq + 1;
			++i;
		} while (wordsq != NULL);
	}
	erpr("Expected word from list [%s]\n", words);
	showerr(text, *accum);
	return -1;
} /*}}}*/
bool parse_int(char *text, int *target, unsigned int *accum) { /*{{{*/
	if (strlen(text) > *accum) {
		int delta = 0;
		if (sscanf(text+*accum, "%d%n", target, &delta) != 0) {
			if (delta > 0) {
				*accum += delta;
				return true;
			}
		}
	}
	erpr("Expected an integer\n");
	showerr(text, *accum);

	return false;
} /*}}}*/
bool parse_uint(char *text, unsigned int *target, unsigned int *accum) { /*{{{*/
	if (strlen(text) > *accum) {
		int delta = 0;
		if (sscanf(text+*accum, "%u%n", target, &delta) != 0) {
			if (delta > 0) {
				*accum += delta;
				return true;
			}
		}
	}
	erpr("Expected a positive integer\n");
	showerr(text, *accum);

	return false;
} /*}}}*/
bool parse_ranged_val(ranged_val_t *target, char *text, unsigned int *accum) { /*{{{*/
	int n = 0;
	if (!parse_int(text, &target->value, accum)) {
		return false;
	}
	if ((strlen(text) > *accum) && (text[*accum] == ':')) {
		// we have a range
		*accum += 1;
		if (!(strlen(text) > *accum)) {
			erpr("colon indicates range, but we are running out of string\n");
			showerr(text, *accum);
			return false;
		}
		target->start = target->value;
		target->is_ranged = true;
		if (!parse_int(text, &target->stop, accum)) {
			return false;
		}
	}
	else {
		target->is_ranged = false;
	}
	return true;
} /*}}}*/
bool parse_order(order_t *target, char *text, unsigned int *accum, int n) { /*{{{*/
	ranged_val_t tmp;
	int counter = 0;
	while (counter < n) {
		if (!parse_ranged_val(&tmp, text, accum)) {
			return false;
		}
		if (tmp.is_ranged) {
			if (tmp.stop > tmp.start) {
				while (counter < n && tmp.value <= tmp.stop) {
					target->o[counter++] = tmp.value++;
				}
			}
			else {
				while (counter < n && tmp.value >= tmp.stop) {
					target->o[counter++] = tmp.value--;
				}
			}
		}
		else {
			target->o[counter++] = tmp.value;
		}
	}
	return true;
} /*}}}*/
bool parse_edgemap(edgemap_t *target, char *text, unsigned int *accum) { /*{{{*/
	int wi = match_word(text, "prio,cpy,val,pal,clp,ign", accum);
	if (wi < 0) {
		return false;
	}
	else if (wi > 0) {
		target->priority = UINT_MAX;
		*accum -= 3; // hack in order to rescan the word in parse_edgemap_mapopt
	}
	else {
		if (!parse_uint(text, &target->priority, accum)) {
			return false;
		}
	}
	if (target->priority == UINT_MAX) { // no priority set = lowest priority (highest value)
		if (!parse_edgemap_mapopt(&target->unmatched, text, accum)) {
			return false;
		}
	}
	else {
		if (!parse_edgemap_mapopt(&target->matched, text, accum)) {
			return false;
		}
		if (match_word(text, "or", accum) < 0) {
			return false;
		}
		if (!parse_edgemap_mapopt(&target->unmatched, text, accum)) {
			return false;
		}
	}
	return true;
} /*}}}*/
bool parse_edgemap_mapopt(edgemap_mapopt_t *target, char *text, unsigned int *accum) { /*{{{*/
	char tmp[8];
	unsigned int modevs[5] = {COPY, VALUE, PAL, CLIP, IGNORE};
	int typei = match_word(text, "cpy,val,pal,clp,ign", accum);
	if (typei < 0) {
		return false;
	}
	target->type = modevs[typei];
	if (target->type == VALUE || target->type == PAL) {
		if (!parse_ranged_val(&target->value, text, accum)) {
			return false;
		}
	}
	return true;
} /*}}}*/
action_t *parse_action(char *text, int *status) { /*{{{*/
	action_t *a = malloc(sizeof(action_t));
	if (a == NULL) { /*{{{*/
		badalloc();
		*status = PARSER_BADALLOC;
		return NULL;
	} /*}}}*/

	int delta = 0;
	unsigned int accum = 0;
	char swtmp[10];
	int n;

	*status = PARSER_BADFORM;
	if (!parse_ranged_val(&a->lbound, text, &accum))	{ return a; }
	if (match_word(text, "to", &accum) < 0)				{ return a; }
	if (!parse_ranged_val(&a->ubound, text, &accum))	{ return a; }
	if (match_word(text, "as", &accum) < 0)				{ return a; }
	if (!parse_uint(text, &a->palette.n, &accum))		{ return a; }
	if (match_word(text, "prio", &accum) < 0)			{ return a; }

	if (!parse_order(&a->priority, text, &accum, a->palette.n)) {
		return a;
	}

	int wi = match_word(text, "else,elsebelow", &accum);
	if (wi == 0) {
		a->splitab = false;
	}
	else if (wi == 1) {
		a->splitab = true;
	}
	else {
		return a;
	}
	if (!parse_edgemap(&a->below, text, &accum))		{ return a; }
	if (a->splitab) {
		if (match_word(text, "elseabove", &accum) < 0) 	{ return a; }
		if (!parse_edgemap(&a->above, text, &accum))   	{ return a; }
	}
	*status = PARSER_OK;
	return a;
} /*}}}*/
action_t *free_action(action_t *prisoner) { /*{{{*/
	if (prisoner != NULL) {
		free(prisoner);
	}
	return NULL;
} /*}}}*/

setup_t *interpret_argv(int argc, char * const argv[], int *status) { /*{{{*/
	*status = PARSER_OK;
	setup_t *setup = calloc(1, sizeof(setup_t));
	if (setup == NULL) {
		badalloc();
		*status = PARSER_BADALLOC;
		return setup;
	}
	const char *optstr = "Vvi:o:m:c:";
	opterr = 0;
	int opt = getopt(argc, argv, optstr);
	int actionstatus = PARSER_OK;

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
					*status = PARSER_BADALLOC;
					return setup;
				} /*}}}*/
				strcpy(setup->file.input, optarg);
				break; /* }}} */
			case 'o': /* output file {{{ */
				setup->file.output = calloc(strlen(optarg)+1, sizeof(char));
				if (setup->file.output == NULL) { /*{{{*/
					badalloc();
					*status = PARSER_BADALLOC;
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
				if (actionstatus != PARSER_OK) { /*{{{*/
					free_action(action);
					switch (actionstatus) {
						case PARSER_BADFORM:
							erpr("argument to parameter \"-c\" has bad syntax\n");
							*status = PARSER_BADFORM;
							return setup;
							break;
						case PARSER_BADALLOC:
							erpr("failed to allocate small amounts of memory, probably bad\n");
							*status = PARSER_BADALLOC;
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
					*status = PARSER_BADALLOC;
					return setup;
				} /*}}}*/
				setup->work.action = tmp;
				setup->work.action[setup->work.count-1] = action;
				break; /* }}} */
			case '?': /* now I am confused! {{{ */
				erpr("Missing argument to parameter \"-%c\" or parameter \"%c\" not recognized.\n", optopt, optopt);
				*status = PARSER_CONFUSED;
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
} /*}}}*/
setup_t *free_setup(setup_t *prisoner) { /*{{{*/
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
} /*}}}*/

#undef badreturnval
#undef badalloc
#undef erpr
