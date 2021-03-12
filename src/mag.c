#include "defs.h"

#undef RE
#undef IM

#define RE p3
#define IM p4

void
eval_mag(void)
{
	push(cadr(p1));
	eval();
	mag();
}

// use numerator and denominator to handle (a+i*b)/(c+i*d)

void
mag(void)
{
	save();
	mag_nib();
	restore();
}

void
mag_nib(void)
{
	int i, n;

	p1 = pop();

	if (istensor(p1)) {
		push(p1);
		copy_tensor();
		p1 = pop();
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			mag();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	numerator();
	mag1();

	push(p1);
	denominator();
	mag1();

	divide();
}

void
mag1(void)
{
	save();
	mag1_nib();
	restore();
}

void
mag1_nib(void)
{
	int h;

	p1 = pop();

	if (isnum(p1)) {
		push(p1);
		absval();
		return;
	}

	if (car(p1) == symbol(POWER) && equaln(cadr(p1), -1)) {
		// -1 to a power
		push_integer(1);
		return;
	}

	if (car(p1) == symbol(POWER) && cadr(p1) == symbol(EXP1)) {
		// exponential
		push(caddr(p1));
		real();
		exponential();
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		// product
		p1 = cdr(p1);
		h = tos;
		while (iscons(p1)) {
			push(car(p1));
			mag();
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
		return;
	}

	if (car(p1) == symbol(ADD)) {
		// sum
		push(p1);
		rect(); // convert polar terms, if any
		p1 = pop();
		push(p1);
		real();
		RE = pop();
		push(p1);
		imag();
		IM = pop();
		push(RE);
		push(RE);
		multiply();
		push(IM);
		push(IM);
		multiply();
		add();
		push_rational(1, 2);
		power();
		return;
	}

	// real

	push(p1);
}
