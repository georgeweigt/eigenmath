#include "defs.h"

void
eval_hermite(void)
{
	int k, n;

	p1 = cdr(p1);
	push(car(p1));
	eval();
	p0 = pop();

	p1 = cdr(p1);
	push(car(p1));
	eval();
	n = pop_integer();

	if (n == ERR || n < 0)
		stop("hermite");

	for (k = 0; k <= n / 2; k++) {

		push_integer(-1);
		push_integer(k);
		power();

		push_integer(k);
		factorial();
		divide();

		push_integer(n - 2 * k);
		factorial();
		divide();

		push_integer(2);
		push(p0);
		multiply();
		push_integer(n - 2 * k);
		power();
		multiply();
	}

	add_terms(n / 2 + 1);

	push_integer(n);
	factorial();
	multiply();
}
