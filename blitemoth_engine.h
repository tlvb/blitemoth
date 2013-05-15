#ifndef __BLITEMOTH_ENGINE_H__
#define __BLITEMOTH_ENGINE_H__ 1

#include <stdlib.h>

#define ENG_OK 0
#define ENG_BADFORM 1
#define ENG_BADALLOC 2

typedef struct {
	int foobar;
} action_t;

action_t *parse_action(char *text, int *status);
action_t *free_action(action_t *prisoner);

#endif
