// 'product' function

#include "defs.h"

#undef I
#undef X

#define I p5
#define X p6

void
eval_product(void)
{
	int i, j, k;

	// 1st arg (quoted)

	X = cadr(p1);
	if (!issymbol(X))
		stop("product: 1st arg?");

	// 2nd arg

	push(caddr(p1));
	eval();
	j = pop_integer();
	if (j == ERR)
		stop("product: 2nd arg?");

	// 3rd arg

	push(cadddr(p1));
	eval();
	k = pop_integer();
	if (k == ERR)
		stop("product: 3rd arg?");

	// 4th arg

	p1 = caddddr(p1);

	push_binding(X);

	push_integer(1);

	for (i = j; i <= k; i++) {
		push_integer(i);
		I = pop();
		set_binding(X, I);
		push(p1);
		eval();
		multiply();
	}

	p1 = pop();

	pop_binding(X);

	push(p1);
}
