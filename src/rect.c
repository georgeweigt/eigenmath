#include "defs.h"

void
eval_rect(void)
{
	push(cadr(p1));
	eval();
	rect();
}

void
rect(void)
{
	save();
	rect_nib();
	restore();
}

#undef BASE
#undef EXPO

#define BASE p3
#define EXPO p4

void
rect_nib(void)
{
	int h;

	p1 = pop();

	if (car(p1) == symbol(ADD)) {
		p1 = cdr(p1);
		h = tos;
		while (iscons(p1)) {
			push(car(p1));
			rect();
			p1 = cdr(p1);
		}
		add_terms(tos - h);
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		h = tos;
		while (iscons(p1)) {
			push(car(p1));
			rect();
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
		return;
	}

	if (car(p1) != symbol(POWER)) {
		push(p1);
		return;
	}

	BASE = cadr(p1);
	EXPO = caddr(p1);

	// handle sum in exponent

	if (car(EXPO) == symbol(ADD)) {
		p1 = cdr(EXPO);
		h = tos;
		while (iscons(p1)) {
			push_symbol(POWER);
			push(BASE);
			push(car(p1));
			list(3);
			rect();
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
		return;
	}

	// return mag(p1) * cos(arg(p1)) + i sin(arg(p1)))

	push(p1);
	mag();

	push(p1);
	arg();
	p2 = pop();

	push(p2);
	cosine();

	push(imaginaryunit);
	push(p2);
	sine();
	multiply();

	add();

	multiply();
}
