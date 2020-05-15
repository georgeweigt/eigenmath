#include "defs.h"

#undef X

#define X p3

void
eval_integral(void)
{
	int i, n;

	// evaluate 1st arg to get function F

	p1 = cdr(p1);
	push(car(p1));
	eval();

	// check for single arg

	if (cdr(p1) == symbol(NIL)) {
		guess();
		integral();
		return;
	}

	p1 = cdr(p1);

	while (iscons(p1)) {

		// next arg should be a symbol

		push(car(p1)); // have to eval in case of $METAX
		eval();
		X = pop();

		if (!issymbol(X))
			stop("integral: symbol expected");

		p1 = cdr(p1);

		// if next arg is a number then use it

		n = 1;

		if (isnum(car(p1))) {
			push(car(p1));
			n = pop_integer();
			if (n < 1)
				stop("nth integral: check n");
			p1 = cdr(p1);
		}

		for (i = 0; i < n; i++) {
			push(X);
			integral();
		}
	}
}

void
integral(void)
{
	save();

	p2 = pop(); // x
	p1 = pop(); // f(x)

	if (car(p1) == symbol(ADD))
		integral_of_sum();
	else if (car(p1) == symbol(MULTIPLY))
		integral_of_product();
	else
		integral_of_form();

	restore();
}

void
integral_of_sum(void)
{
	int h = tos;

	p1 = cdr(p1);

	while (iscons(p1)) {
		push(car(p1));
		push(p2);
		integral();
		p1 = cdr(p1);
	}

	add_terms(tos - h);
}

void
integral_of_product(void)
{
	push(p1); // f(x)
	push(p2); // x
	partition(); // pushes const part, then pushes var part
	p1 = pop(); // pop var part
	integral_of_form();
	multiply(); // multiply by const part
}

void
integral_of_form(void)
{
	push(p1); // f(x)
	push(p2); // x
	transform();
}
