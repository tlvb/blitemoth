#ifndef __BLITEMOTH_ENGINE_H__
#define __BLITEMOTH_ENGINE_H__ 1

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define ENG_OK 0
#define ENG_BADFORM 1
#define ENG_BADALLOC 2

#define ENGA_COPY 0
#define ENGA_VALUE 1
#define ENGA_PAL 2
#define ENGA_CLIP 3
#define ENGA_IGNORE 4

#define ENG_MAXPALETTE 12

typedef struct {
	bool is_ranged;
	int value;
	int start;
	int stop;
} ranged_val_t;

typedef struct {
	unsigned int n;
	unsigned int c[ENG_MAXPALETTE];
} palette_t;

typedef struct {
	uint8_t o[ENG_MAXPALETTE];
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
bool match_int(char *text, int *target, unsigned int *accum);
bool match_uint(char *text, unsigned int *target, unsigned int *accum);
bool parse_ranged_val(ranged_val_t *target, char *text, unsigned int *accum);
bool parse_order(order_t *target, char *text, unsigned int *accum, int n);
bool parse_edgemap(edgemap_t *target, char *text, unsigned int *accum);
bool parse_edgemap_mapopt(edgemap_mapopt_t *target, char *text, unsigned int *accum);
action_t *parse_action(char *text, int *status);
action_t *free_action(action_t *prisoner);

#endif
