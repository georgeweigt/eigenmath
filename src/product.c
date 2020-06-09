#include "defs.h"

void
eval_product(void)
{
	int i, j, k;

	// 1st arg (quoted)

	p1 = cdr(p1);
	p2 = car(p1);
	if (!issymbol(p2))
		stop("product: 1st arg?");

	// 2nd arg

	p1 = cdr(p1);
	push(car(p1));
	eval();
	j = pop_integer();
	if (j == ERR)
		stop("product: 2nd arg?");

	// 3rd arg

	p1 = cdr(p1);
	push(car(p1));
	eval();
	k = pop_integer();
	if (k == ERR)
		stop("product: 3rd arg?");

	if (k - j < 0) {
		push_integer(1);
		return;
	}

	// 4th arg

	p1 = cadr(p1);

	save_binding(p2);

	for (i = j; i <= k; i++) {
		push_integer(i);
		p3 = pop();
		set_binding(p2, p3);
		push(p1);
		eval();
	}

	multiply_factors(k - j + 1);

	restore_binding(p2);
}
