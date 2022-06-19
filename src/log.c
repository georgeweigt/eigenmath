#include "defs.h"

void
eval_log(struct atom *p1)
{
	push(cadr(p1));
	eval();
	logfunc();
}

void
logfunc(void)
{
	int h, i;
	double d;
	struct atom *p1, *p2;

	p1 = pop();

	// log of zero is not evaluated

	if (iszero(p1)) {
		push_symbol(LOG);
		push_integer(0);
		list(2);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		if (d > 0.0) {
			push_double(log(d));
			return;
		}
	}

	// log(z) -> log(mag(z)) + i arg(z)

	if (isdouble(p1) || isdoublez(p1)) {
		push(p1);
		mag();
		logfunc();
		push(p1);
		arg();
		push(imaginaryunit);
		multiply();
		add();
		return;
	}

	// log(1) -> 0

	if (isplusone(p1)) {
		push_integer(0);
		return;
	}

	// log(e) -> 1

	if (p1 == symbol(EXP1)) {
		push_integer(1);
		return;
	}

	if (isnegativenumber(p1)) {
		push(p1);
		negate();
		logfunc();
		push(imaginaryunit);
		push_symbol(PI);
		multiply();
		add();
		return;
	}

	// log(10) -> log(2) + log(5)

	if (isrational(p1)) {
		h = tos;
		push(p1);
		factor_factor();
		for (i = h; i < tos; i++) {
			p2 = stack[i];
			if (car(p2) == symbol(POWER)) {
				push(caddr(p2)); // exponent
				push_symbol(LOG);
				push(cadr(p2)); // base
				list(2);
				multiply();
			} else {
				push_symbol(LOG);
				push(p2);
				list(2);
			}
			stack[i] = pop();
		}
		add_terms(tos - h);
		return;
	}

	// log(a ^ b) -> b log(a)

	if (car(p1) == symbol(POWER)) {
		push(caddr(p1));
		push(cadr(p1));
		logfunc();
		multiply();
		return;
	}

	// log(a * b) -> log(a) + log(b)

	if (car(p1) == symbol(MULTIPLY)) {
		h = tos;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			logfunc();
			p1 = cdr(p1);
		}
		add_terms(tos - h);
		return;
	}

	push_symbol(LOG);
	push(p1);
	list(2);
}
