#include "defs.h"

#undef FUNC_NAME
#undef FUNC_DEFN
#undef FORMAL
#undef ACTUAL

#define FUNC_NAME p4
#define FUNC_DEFN p5
#define FORMAL p6 // formal argument list
#define ACTUAL p7 // actual argument list

void
eval_user_function(void)
{
	int h, i, j, n;

	FUNC_NAME = car(p1);
	FUNC_DEFN = get_binding(FUNC_NAME);

	FORMAL = get_arglist(FUNC_NAME);
	ACTUAL = cdr(p1);

	// use "derivative" instead of "d" if there is no user function "d"

	if (FUNC_NAME == symbol(SYMBOL_D) && get_arglist(symbol(SYMBOL_D)) == symbol(NIL)) {
		eval_derivative();
		return;
	}

	// undefined function?

	if (FUNC_DEFN == symbol(NIL)) {
		push(FUNC_NAME);
		p1 = ACTUAL;
		n = length(p1);
		for (i = 0; i < n; i++) {
			push(car(p1));
			eval();
			p1 = cdr(p1);
		}
		list(n + 1);
		return;
	}

	// eval actual args

	h = tos;

	n = length(FORMAL); // if ACTUAL is shorter than FORMAL then NIL is pushed for missing args

	p1 = ACTUAL;

	for (i = 0; i < n; i++) {
		push(car(p1));
		eval();
		p1 = cdr(p1);
	}

	// resolve symbol collisions

	p1 = FORMAL;

	for (i = 0; i < n; i++) {

		p2 = car(p1);

		for (j = 0; j < n; j++)
			if (find(stack[h + j], p2))
				break;

		if (j < n) {

			// collision, use dual

			p3 = dual(p2);

			push(FUNC_DEFN);
			push(p2);
			push(p3);
			subst();
			FUNC_DEFN = pop();

			p2 = p3;
		}

		push(p2);

		p1 = cdr(p1);
	}

	list(n);

	FORMAL = pop();

	// assign to formal args

	p1 = FORMAL;

	for (i = 0; i < n; i++) {
		push_binding(car(p1), stack[h + i]);
		p1 = cdr(p1);
	}

	tos = h; // pop all

	// evaluate user function

	push(FUNC_DEFN);
	eval();

	// remove args

	p1 = FORMAL;

	for (i = 0; i < n; i++) {
		pop_binding(car(p1));
		p1 = cdr(p1);
	}
}
