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
		p1 = FUNC_ARGS;
		while (iscons(p1)) {
			push(car(p1));
			eval();
			p1 = cdr(p1);
		}
		list(tos - h);
		return;
	}

	// eval all args before changing bindings

	p1 = FUNC_ARGS;

	for (i = 0; i < 9; i++) {
		push(car(p1));
		eval();
		p1 = cdr(p1);
	}

	save_symbol(ARG1);
	save_symbol(ARG2);
	save_symbol(ARG3);
	save_symbol(ARG4);
	save_symbol(ARG5);
	save_symbol(ARG6);
	save_symbol(ARG7);
	save_symbol(ARG8);
	save_symbol(ARG9);

	p1 = pop();
	set_binding(symbol(ARG9), p1);
	set_usrfunc(symbol(ARG9), symbol(NIL));

	p1 = pop();
	set_binding(symbol(ARG8), p1);
	set_usrfunc(symbol(ARG8), symbol(NIL));

	p1 = pop();
	set_binding(symbol(ARG7), p1);
	set_usrfunc(symbol(ARG7), symbol(NIL));

	p1 = pop();
	set_binding(symbol(ARG6), p1);
	set_usrfunc(symbol(ARG6), symbol(NIL));

	p1 = pop();
	set_binding(symbol(ARG5), p1);
	set_usrfunc(symbol(ARG5), symbol(NIL));

	p1 = pop();
	set_binding(symbol(ARG4), p1);
	set_usrfunc(symbol(ARG4), symbol(NIL));

	p1 = pop();
	set_binding(symbol(ARG3), p1);
	set_usrfunc(symbol(ARG3), symbol(NIL));

	p1 = pop();
	set_binding(symbol(ARG2), p1);
	set_usrfunc(symbol(ARG2), symbol(NIL));

	p1 = pop();
	set_binding(symbol(ARG1), p1);
	set_usrfunc(symbol(ARG1), symbol(NIL));

	push(FUNC_DEFN);
	eval();

	restore_symbol(ARG9);
	restore_symbol(ARG8);
	restore_symbol(ARG7);
	restore_symbol(ARG6);
	restore_symbol(ARG5);
	restore_symbol(ARG4);
	restore_symbol(ARG3);
	restore_symbol(ARG2);
	restore_symbol(ARG1);
}
