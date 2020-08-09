#include "defs.h"

// P(x,n,m) = 1/(2^n n!) (1 - x^2)^(m/2) d((x^2 - 1)^n,x,n + m)

void
eval_legendre(void)
{
	int i, m = 0, n;

	p1 = cdr(p1);
	push(car(p1));
	eval();
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

	if (n == ERR || m == ERR || n < 0 || abs(m) > n)
		stop("legendre");

	push_integer(2);
	push_integer(-n);
	power();
	push_integer(n);
	factorial();
	divide();

	push_integer(1);
	push_symbol(SPECX);
	push_integer(2);
	power();
	subtract();
	push_integer(m);
	push_rational(1, 2);
	multiply();
	power();

	push_symbol(SPECX);
	push_integer(2);
	power();
	push_integer(-1);
	add();
	push_integer(n);
	power();

	for (i = 0; i < n + m; i++) {
		push_symbol(SPECX);
		derivative();
	}

	multiply_factors(3);

	push_symbol(SPECX);
	push(p0);
	subst();
	eval();
}
