#include "defs.h"

#undef F
#undef A
#undef B
#undef S

#define F p4 // F is the function body
#define A p5 // A is the formal argument list
#define B p6 // B is the calling argument list
#define S p7 // S is the argument substitution list

void
eval_user_function(void)
{
	int h;

	// use "derivative" instead of "d" if there is no user function "d"

	if (car(p1) == symbol(SYMBOL_D) && get_arglist(symbol(SYMBOL_D)) == symbol(NIL)) {
		eval_derivative();
		return;
	}

	F = get_binding(car(p1));
	A = get_arglist(car(p1));
	B = cdr(p1);

	// undefined function?

	if (F == car(p1)) {
		h = tos;
		push(F);
		p1 = B;
		while (iscons(p1)) {
			push(car(p1));
			eval();
			p1 = cdr(p1);
		}
		list(tos - h);
		return;
	}

	// create the argument substitution list S

	p1 = A;
	p2 = B;
	h = tos;
	while (iscons(p1) && iscons(p2)) {
		push(car(p1));
		push(car(p2));
		eval();
		p1 = cdr(p1);
		p2 = cdr(p2);
	}
	list(tos - h);
	S = pop();

	// evaluate the function body

	push(F);

	if (iscons(S))
		rewrite();

	eval();
}

int
rewrite(void)
{
	int n;
	save();
	n = rewrite_nib();
	restore();
	return n;
}

int
rewrite_nib(void)
{
	int h, i, m, n = 0;

	p1 = pop();

	if (istensor(p1)) {
		push(p1);
		copy_tensor();
		p1 = pop();
		m = p1->u.tensor->nelem;
		for (i = 0; i < m; i++) {
			push(p1->u.tensor->elem[i]);
			n += rewrite();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return n;
	}

	if (iscons(p1)) {

		h = tos;

		push(car(p1)); // don't rewrite function name
		p1 = cdr(p1);

		while (iscons(p1)) {
			push(car(p1));
			n += rewrite();
			p1 = cdr(p1);
		}

		list(tos - h);

		return n;
	}

	// if not a symbol then done

	if (!issymbol(p1)) {
		push(p1);
		return 0; // no substitution
	}

	// check argument substitution list

	p2 = S;
	while (iscons(p2)) {
		if (p1 == car(p2)) {
			push(cadr(p2));
			return 1; // substitution occurred
		}
		p2 = cddr(p2);
	}

	// get the symbol's binding, try again

	p2 = get_binding(p1);

	if (p1 == p2) {
		push(p1);
		return 0; // no substitution
	}

	push(p2);

	n = rewrite();

	if (n == 0) {
		p2 = pop(); // undo
		push(p1);
	}

	return n;
}
