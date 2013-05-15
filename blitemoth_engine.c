#include "blitemoth_engine.h"
#include <stdio.h>

#define badreturnval(...) erpr("called function returned an error: " __VA_ARGS__)
#define badalloc() erpr("memory allocation failure\n")
#define erpr(...) fprintf(stderr, "ENGINE :: " __VA_ARGS__)

void showerr(char *text, int accum) {
	erpr("\"%s\"\n", text);
	erpr(" ");
	while (accum-- > 0)
		fputc('.', stderr);
	fprintf(stderr,"^HERE\n");
}
int parse_ranged_val(ranged_val_t *target, char *text, int *accum) {
	int delta = 0;
	int n = 0;
	n = sscanf(text+*accum, "%d%n", &target->value, &delta);
	if (n != 1) {
		erpr("expected a number around character %d/'%c' of command string\n", *accum, text[*accum]);
		showerr(text, *accum);
		return 0;
	}
	*accum += delta;
	if (text[*accum] == ':') {
		// we have a range
		*accum += 1;
		target->start = target->value;
		target->is_ranged = true;
		n = sscanf(text+*accum, "%d%n", &target->stop, &delta);
		if (n != 1) {
			erpr("expected a number around character %d/'%c' of command string\n", *accum, text[*accum]);
			showerr(text, *accum);
			return 0;
		}
		*accum += delta;
	}
	else {
		target->is_ranged = false;
	}
	return 1;
}

int parse_order(order_t *target, char *text, int *accum, int n) {
	ranged_val_t tmp;
	int counter = 0;
	while (counter < n) {
		if (!parse_ranged_val(&tmp, text, accum)) {
			return 0;
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
	return 1;
}

action_t *parse_action(char *text, int *status) {
	action_t *a = malloc(sizeof(action_t));
	if (a == NULL) { /*{{{*/
		badalloc();
		*status = ENG_BADALLOC;
		return NULL;
	} /*}}}*/

	int delta = 0;
	int accum = 0;
	char swtmp[10];
	int n;

	*status = ENG_BADFORM;
	if (!parse_ranged_val(&a->lbound, text, &accum)) {
		return a;
	}
	if (!(text[accum]=='.'&&text[accum+1]=='.'&&text[accum+2]=='.')) {
		erpr("expected \"...\" around character %d/'%c' of command string\n", accum, text[accum]);
		showerr(text, accum);
		return a;
	}
	accum += 3;
	if (!parse_ranged_val(&a->ubound, text, &accum)) {
		return a;
	}
	if (sscanf(text+accum, " to %u prio %n", &a->palette.n, &delta) != 1 ||
			a->palette.n < 2) {
		erpr("expected \" to \" and then a 1<number<%d and then \" prio\" around "
				"charecter %d/'%c' of command string\n",
				ENG_MAXPALETTE, accum, text[accum]);
		showerr(text, accum);
		return a;
	}
	accum += delta;
	if (!parse_order(&a->priority, text, &accum, a->palette.n)) {
		return a;
	}
	accum += delta;
	delta = 0;
	erpr("SO FAR SO GOOD\n");
	return a;
}
action_t *free_action(action_t *prisoner) {
	if (prisoner != NULL) {
		free(prisoner);
	}
	return NULL;
}

#undef badreturnval
#undef badalloc
#undef erpr
