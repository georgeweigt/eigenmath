#include "defs.h"

#undef FUNC
#undef FARG
#undef AARG

#define FUNC p4 // function body
#define FARG p5 // formal argument list
#define AARG p6 // actual argument list

void
eval_user_function(void)
{
	int h, i, j, n;

	// use "derivative" instead of "d" if there is no user function "d"

	if (car(p1) == symbol(SYMBOL_D) && get_arglist(symbol(SYMBOL_D)) == symbol(NIL)) {
		eval_derivative();
		return;
	}

	FUNC = get_binding(car(p1));
	FARG = get_arglist(car(p1));
	AARG = cdr(p1);

	// undefined function?

	if (FUNC == symbol(NIL)) {
		push(car(p1)); // function name
		p1 = AARG;
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

	n = length(FARG); // if AARG is shorter than FARG then NIL is pushed for missing args

	p1 = AARG;

	for (i = 0; i < n; i++) {
		push(car(p1));
		eval();
		p1 = cdr(p1);
	}

	// resolve collisions

	p1 = FARG;

	for (i = 0; i < n; i++) {

		p2 = car(p1);

		for (j = 0; j < n; j++)
			if (find(stack[h + j], p2))
				break;

		if (j < n) {

			// collision, use dual

			p3 = dual(p2);

			push(FUNC);
			push(p2);
			push(p3);
			subst();
			FUNC = pop();

			p2 = p3;
		}

		push(p2);

		p1 = cdr(p1);
	}

	list(n);

	FARG = pop();

	// assign to formal args

	p1 = FARG;

	for (i = 0; i < n; i++) {
		push_binding(car(p1), stack[h + i]);
		p1 = cdr(p1);
	}

	tos = h; // pop all

	// evaluate function body

	push(FUNC);
	eval();

	// remove args

	p1 = FARG;

	for (i = 0; i < n; i++) {
		pop_binding(car(p1));
		p1 = cdr(p1);
	}
}
