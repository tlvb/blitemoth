#include "blitemoth_engine.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define badreturnval(...) erpr("called function returned an error: " __VA_ARGS__)
#define badalloc() erpr("memory allocation failure\n")
#define erpr(...) fprintf(stderr, "ENGINE :: " __VA_ARGS__)

void showerr(char *text, unsigned int accum) { /*{{{*/
	erpr("\"%s\"\n", text);
	erpr(" ");
	int a = accum;
	while (a-- > 0)
		fputc('.', stderr);
	fprintf(stderr,"\x1b[1;31m^\x1b[0mAFTER HERE (character %d)\n", accum);
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
bool match_int(char *text, int *target, unsigned int *accum) { /*{{{*/
	if (strlen(text) > *accum) {
		int delta = 0;
		if (sscanf(text+*accum, "%d%n", target, &delta) != 0) {
			*accum += delta;
			return true;
		}
	}
	erpr("Expected an integer\n");
	showerr(text, *accum);

	return false;
} /*}}}*/
bool match_uint(char *text, unsigned int *target, unsigned int *accum) { /*{{{*/
	if (strlen(text) > *accum) {
		int delta = 0;
		if (sscanf(text+*accum, "%u%n", target, &delta) != 0) {
			*accum += delta;
			return true;
		}
	}
	erpr("Expected a positive integer\n");
	showerr(text, *accum);

	return false;
} /*}}}*/
bool parse_ranged_val(ranged_val_t *target, char *text, unsigned int *accum) { /*{{{*/
	int n = 0;
	if (!match_int(text, &target->value, accum)) {
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
		if (!match_int(text, &target->stop, accum)) {
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
	else if (wi == 0) {
		target->priority = UINT_MAX;
		if (!match_uint(text, &target->priority, accum)) {
			return false;
		}
	}
	else {
		*accum -= 3; // hack in order to rescan the word in parse_edgemap_mapopt
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
	unsigned int modevs[5] = {ENGA_COPY, ENGA_VALUE, ENGA_PAL, ENGA_CLIP, ENGA_IGNORE};
	int typei = match_word(text, "cpy,val,pal,clp,ign", accum);
	if (typei < 0) {
		return false;
	}
	target->type = modevs[typei];
	if (target->type == ENGA_VALUE || target->type == ENGA_PAL) {
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
		*status = ENG_BADALLOC;
		return NULL;
	} /*}}}*/

	int delta = 0;
	unsigned int accum = 0;
	char swtmp[10];
	int n;

	*status = ENG_BADFORM;
	if (!parse_ranged_val(&a->lbound, text, &accum))	{ return a; }
	if (match_word(text, "to", &accum) < 0)				{ return a; }
	if (!parse_ranged_val(&a->ubound, text, &accum))	{ return a; }
	if (match_word(text, "as", &accum) < 0)				{ return a; }
	if (!match_uint(text, &a->palette.n, &accum))		{ return a; }
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
	*status = ENG_OK;
	return a;
} /*}}}*/
action_t *free_action(action_t *prisoner) { /*{{{*/
	if (prisoner != NULL) {
		free(prisoner);
	}
	return NULL;
} /*}}}*/

#undef badreturnval
#undef badalloc
#undef erpr
