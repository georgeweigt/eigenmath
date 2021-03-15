#include "defs.h"

void
eval_float(void)
{
	push(cadr(p1));
	eval();
	floatv();
}

void
floatv(void)
{
	float_subst();
	eval();
	float_subst(); // in case pi popped up
	eval();
}

void
float_subst(void)
{
	save();
	float_subst_nib();
	restore();
}

void
float_subst_nib(void)
{
	int h, i, n;

	p1 = pop();

	if (p1 == symbol(PI)) {
		push_double(M_PI);
		return;
	}

	if (p1 == symbol(EXP1)) {
		push_double(M_E);
		return;
	}

	if (isrational(p1)) {
		push(p1);
		bignum_float();
		return;
	}

	// don't float exponential

	if (car(p1) == symbol(POWER) && cadr(p1) == symbol(EXP1)) {
		push_symbol(POWER);
		push_symbol(EXP1);
		push(caddr(p1));
		float_subst();
		list(3);
		return;
	}

	// don't float imaginary unit, but multiply it by 1.0

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		push_symbol(MULTIPLY);
		push_double(1.0);
		push_symbol(POWER);
		push(cadr(p1));
		push(caddr(p1));
		float_subst();
		list(3);
		list(3);
		return;
	}

	if (iscons(p1)) {
		h = tos;
		push(car(p1));
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			float_subst();
			p1 = cdr(p1);
		}
		list(tos - h);
		return;
	}

	if (istensor(p1)) {
		push(p1);
		copy_tensor();
		p1 = pop();
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			float_subst();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
}
