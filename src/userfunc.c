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

	// Use "derivative" instead of "d" if there is no user function "d"

	if (car(p1) == symbol(SYMBOL_D) && get_arglist(symbol(SYMBOL_D)) == symbol(NIL)) {
		eval_derivative();
		return;
	}

	F = get_binding(car(p1));
	A = get_arglist(car(p1));
	B = cdr(p1);

	// Undefined function?

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

	// Create the argument substitution list S

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

	// Evaluate the function body

	push(F);
	if (iscons(S))
		rewrite();
	eval();
}

void
rewrite(void)
{
	save();
	rewrite_nib();
	restore();
}

void
rewrite_nib(void)
{
	int h, i, n;

	p1 = pop();

	if (istensor(p1)) {
		push(p1);
		copy_tensor();
		p1 = pop();
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			rewrite();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (iscons(p1)) {
		h = tos;
		push(car(p1)); // Do not rewrite function name
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			rewrite();
			p1 = cdr(p1);
		}
		list(tos - h);
		return;
	}

	// If not a symbol then done

	if (!issymbol(p1)) {
		push(p1);
		return;
	}

	// Try for an argument substitution first

	p2 = S;
	while (iscons(p2)) {
		if (p1 == car(p2)) {
			push(cadr(p2));
			return;
		}
		p2 = cddr(p2);
	}

	// Get the symbol's binding, try again

	p2 = get_binding(p1);
	push(p2);
	if (p1 != p2)
		rewrite();
}
