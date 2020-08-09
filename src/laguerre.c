#include "defs.h"

// L(x,n,m) = (n + m)! sum(k,0,n,(-x)^k / ((n - k)! (m + k)! k!))

void
eval_laguerre(void)
{
	int k, m = 0, n;

	p1 = cdr(p1);
	push(car(p1));
	eval();
	negate();
	p0 = pop();

	p1 = cdr(p1);
	push(car(p1));
	eval();
	n = pop_integer();

	p1 = cdr(p1);
	if (iscons(p1)) {
		push(car(p1));
		eval();
		m = pop_integer();
	}

	if (n == ERR || m == ERR || n < 0 || m < 0)
		stop("laguerre");

	for (k = 0; k <= n; k++) {

		push(p0);
		push_integer(k);
		power();

		push_integer(n - k);
		factorial();

		push_integer(m + k);
		factorial();

		push_integer(k);
		factorial();

		multiply_factors(3);
		divide();
	}

	add_terms(n + 1);

	push_integer(n + m);
	factorial();
	multiply();
}
