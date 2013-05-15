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
#define ENGA_IGNORE 3

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

typedef union {
	ranged_val_t value;
	uint8_t palmap;
} edgemap_data_t;

typedef struct {
	uint8_t priority;
	uint8_t type;
	edgemap_data_t data[2];
} edgemap_t;


typedef struct {
	ranged_val_t lbound;
	ranged_val_t ubound;
	palette_t palette;
	order_t priority;
	edgemap_t below;
	edgemap_t above;
} action_t;

int parse_ranged_val(ranged_val_t *target, char *text, int *accum);
int parse_order(order_t *target, char *text, int *accum, int n);
action_t *parse_action(char *text, int *status);
action_t *free_action(action_t *prisoner);

#endif
