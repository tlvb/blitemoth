#ifndef __BLITEMOTH_PARSER_H__
#define __BLITEMOTH_PARSER_H__ 1

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define PARSER_OK 0
#define PARSER_BADALLOC 1
#define PARSER_BADFORM 2
#define PARSER_CONFUSED 3

#define COPY 0
#define VALUE 1
#define PAL 2
#define CLIP 3
#define IGNORE 4

#define MAXPALETTE 16

typedef struct {
	bool is_ranged;
	int value;
	int start;
	int stop;
} ranged_val_t;

typedef struct {
	unsigned int n;
	unsigned int c[MAXPALETTE];
} palette_t;

typedef struct {
	uint8_t o[MAXPALETTE];
} order_t;

typedef struct {
	unsigned int type;
	ranged_val_t value;
} edgemap_mapopt_t;

typedef struct {
	unsigned int priority;
	edgemap_mapopt_t matched;
	edgemap_mapopt_t unmatched;
} edgemap_t;

typedef struct {
	ranged_val_t lbound;
	ranged_val_t ubound;
	palette_t palette;
	order_t priority;
	bool splitab;
	edgemap_t below;
	edgemap_t above;
} action_t;

void showerr(char *text, unsigned int accum);
int match_word(char *text, char *words, unsigned int *accum);
bool parse_int(char *text, int *target, unsigned int *accum);
bool parse_uint(char *text, unsigned int *target, unsigned int *accum);
bool parse_ranged_val(ranged_val_t *target, char *text, unsigned int *accum);
bool parse_order(order_t *target, char *text, unsigned int *accum, int n);
bool parse_edgemap(edgemap_t *target, char *text, unsigned int *accum);
bool parse_edgemap_mapopt(edgemap_mapopt_t *target, char *text, unsigned int *accum);
action_t *parse_action(char *text, int *status);
action_t *free_action(action_t *prisoner);

typedef struct {
	bool verbose;
	struct {
		char *input;
		char *output;
		bool range;
		unsigned int begin;
		unsigned int end;
	} file;
	struct {
		unsigned int count;
		action_t **action;
	} work;
} setup_t;

setup_t *interpret_argv(int argc, char * const argv[], int *status);
setup_t *free_setup(setup_t *prisoner);

#endif
