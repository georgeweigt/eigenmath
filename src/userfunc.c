#include "defs.h"

#undef FUNC_NAME
#undef FUNC_ARGS
#undef FUNC_DEFN

#define FUNC_NAME p4
#define FUNC_ARGS p5
#define FUNC_DEFN p6

void
eval_user_function(void)
{
	int h, i;

	FUNC_NAME = car(p1);
	FUNC_ARGS = cdr(p1);

	FUNC_DEFN = get_usrfunc(FUNC_NAME);

	// undefined function?

	if (FUNC_DEFN == symbol(NIL)) {
		if (FUNC_NAME == symbol(SYMBOL_D)) {
			eval_derivative();
			return;
		}
		h = tos;
		push(FUNC_NAME);
		while (iscons(FUNC_ARGS)) {
			push(car(FUNC_ARGS));
			eval();
			FUNC_ARGS = cdr(FUNC_ARGS);
		}
		list(tos - h);
		return;
	}

	// eval all args before changing bindings

	for (i = 0; i < 9; i++) {
		push(car(FUNC_ARGS));
		eval();
		FUNC_ARGS = cdr(FUNC_ARGS);
	}

	save_symbol(symbol(ARG1));
	save_symbol(symbol(ARG2));
	save_symbol(symbol(ARG3));
	save_symbol(symbol(ARG4));
	save_symbol(symbol(ARG5));
	save_symbol(symbol(ARG6));
	save_symbol(symbol(ARG7));
	save_symbol(symbol(ARG8));
	save_symbol(symbol(ARG9));

	p1 = pop();
	set_symbol(symbol(ARG9), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG8), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG7), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG6), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG5), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG4), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG3), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG2), p1, symbol(NIL));

	p1 = pop();
	set_symbol(symbol(ARG1), p1, symbol(NIL));

	push(FUNC_DEFN);
	eval();

	restore_symbol(symbol(ARG9));
	restore_symbol(symbol(ARG8));
	restore_symbol(symbol(ARG7));
	restore_symbol(symbol(ARG6));
	restore_symbol(symbol(ARG5));
	restore_symbol(symbol(ARG4));
	restore_symbol(symbol(ARG3));
	restore_symbol(symbol(ARG2));
	restore_symbol(symbol(ARG1));
}
