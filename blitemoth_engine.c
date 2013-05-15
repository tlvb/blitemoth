#include "blitemoth_engine.h"

action_t *parse_action(char *text, int *status) {
	*status = ENG_BADALLOC;
	return NULL;
}
action_t *free_action(action_t *prisoner) {
	if (prisoner != NULL) {
		free(prisoner);
	}
	return NULL;
}
