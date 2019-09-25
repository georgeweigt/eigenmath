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
	p2 = pop();
	p1 = pop();
	if (car(p1) == symbol(ADD))
		integral_of_sum();
	else if (car(p1) == symbol(MULTIPLY))
		integral_of_product();
	else
		integral_of_form();
	p1 = pop();
	if (find(p1, symbol(INTEGRAL)))
		stop("integral could not find a solution");
	push(p1);
	eval();		// normalize the result
	restore();
}

void
integral_of_sum(void)
{
	p1 = cdr(p1);
	push(car(p1));
	push(p2);
	integral();
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		push(p2);
		integral();
		add();
		p1 = cdr(p1);
	}
}

void
integral_of_product(void)
{
	push(p1);
	push(p2);
	partition();
	p1 = pop();			// pop variable part
	integral_of_form();
	multiply();			// multiply constant part
}

extern char *itab[];

void
integral_of_form(void)
{
	push(p1);
	push(p2);
	transform(itab);
	p3 = pop();
	if (p3 == symbol(NIL)) {
		push_symbol(INTEGRAL);
		push(p1);
		push(p2);
		list(3);
	} else
		push(p3);
}
