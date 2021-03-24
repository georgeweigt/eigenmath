#include "defs.h"

#undef RE
#undef IM

#define RE p3
#define IM p4

void
eval_arg(void)
{
	push(cadr(p1));
	eval();
	arg();
}

// use numerator and denominator to handle (a+i*b)/(c+i*d)

void
arg(void)
{
	save();
	arg_nib();
	restore();
}

void
arg_nib(void)
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
			arg();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	numerator();
	arg1();

	push(p1);
	denominator();
	arg1();

	subtract();

	p1 = pop();
	push(p1);

	if (iscons(p1) && isdoublesomewhere(p1))
		floatv();
}

void
arg1(void)
{
	save();
	arg1_nib();
	restore();
}

void
arg1_nib(void)
{
	int h;

	p1 = pop();

	if (isrational(p1)) {
		if (p1->sign == MPLUS)
			push_integer(0);
		else {
			push_symbol(PI);
			negate();
		}
		return;
	}

	if (isdouble(p1)) {
		if (p1->u.d >= 0.0)
			push_double(0.0);
		else
			push_double(-M_PI);
		return;
	}

	// (-1) ^ expr

	if (car(p1) == symbol(POWER) && equaln(cadr(p1), -1)) {
		push(symbol(PI));
		push(caddr(p1));
		multiply();
		return;
	}

	// e ^ expr

	if (car(p1) == symbol(POWER) && cadr(p1) == symbol(EXP1)) {
		push(caddr(p1));
		imag();
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		h = tos;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			arg();
			p1 = cdr(p1);
		}
		add_terms(tos - h);
		return;
	}

	if (car(p1) == symbol(ADD)) {
		push(p1);
		rect(); // convert polar and clock forms
		p1 = pop();
		push(p1);
		real();
		RE = pop();
		push(p1);
		imag();
		IM = pop();
		push(IM);
		push(RE);
		arctan();
		return;
	}

	push_integer(0);
}
