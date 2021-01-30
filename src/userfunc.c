#include "defs.h"

#undef FUNC_NAME
#undef FUNC_DEFN
#undef FORMAL
#undef ACTUAL
#undef T

#define FUNC_NAME p4
#define FUNC_DEFN p5
#define FORMAL p6 // formal argument list
#define ACTUAL p7 // actual argument list
#define T p8

void
eval_user_function(void)
{
	int h, k;

	FUNC_NAME = car(p1);
	FUNC_DEFN = get_binding(dual(FUNC_NAME));

	FORMAL = get_arglist(FUNC_NAME);
	ACTUAL = cdr(p1);

	// use "derivative" instead of "d" if there is no user function "d"

	if (FUNC_NAME == symbol(SYMBOL_D) && get_arglist(symbol(SYMBOL_D)) == symbol(NIL)) {
		eval_derivative();
		return;
	}

	// undefined function?

	if (FUNC_DEFN == symbol(NIL)) {
		h = tos;
		push(FUNC_NAME);
		p1 = ACTUAL;
		while (iscons(p1)) {
			push(car(p1));
			eval();
			p1 = cdr(p1);
		}
		list(tos - h);
		return;
	}

	// eval actual args (ACTUAL can be shorter than FORMAL, NIL is pushed for missing args)

	h = tos;
	p1 = FORMAL;
	p2 = ACTUAL;

	while (iscons(p1)) {
		push(car(p2));
		eval();
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	// assign actual to formal

	k = h;
	p1 = FORMAL;

	while (iscons(p1)) {
		p2 = car(p1);
		p3 = stack[k];
		stack[k] = get_binding(p2);
		set_binding(p2, p3);
		k++;
		p1 = cdr(p1);
	}

	// evaluate user function

	push(FUNC_DEFN);
	eval();
	T = pop();

	// restore bindings

	k = h;
	p1 = FORMAL;

	while (iscons(p1)) {
		p2 = car(p1);
		p3 = stack[k];
		set_binding(p2, p3);
		k++;
		p1 = cdr(p1);
	}

	tos = h; // pop all

	push(T);
}
