// 'sum' function

#include "defs.h"

#undef I
#undef X

#define I p5
#define X p6

void
eval_sum(void)
{
	int i, j, k;

	// 1st arg (quoted)

	X = cadr(p1);
	if (!issymbol(X))
		stop("sum: 1st arg?");

	// 2nd arg

	push(caddr(p1));
	eval();
	j = pop_integer();
	if (j == ERR)
		stop("sum: 2nd arg?");

	// 3rd arg

	push(cadddr(p1));
	eval();
	k = pop_integer();
	if (k == ERR)
		stop("sum: 3rd arg?");

	// 4th arg

	p1 = caddddr(p1);

	push_binding(X);

	push_integer(0);

	for (i = j; i <= k; i++) {
		push_integer(i);
		set_binding(X, pop());
		push(p1);
		eval();
		add();
	}

	p1 = pop();

	pop_binding(X);

	push(p1);
}
