#include "defs.h"

void
eval_product(void)
{
	int h, i, j, k, n;

	n = length(p1);

	if (n == 2) {
		push(cadr(p1));
		eval();
		p1 = pop();
		if (!istensor(p1)) {
			push(p1);
			return;
		}
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++)
			push(p1->u.tensor->elem[i]);
		multiply_factors(n);
		return;
	}

	if (n != 5)
		stop("product: 4 args expected");

	p2 = cadr(p1);

	if (!isusersymbol(p2))
		stop("product 1st arg: symbol expected");

	p1 = cddr(p1);

	push(car(p1));
	eval();
	j = pop_integer();

	if (j == ERR)
		stop("product 2nd arg: integer value expected");

	push(cadr(p1));
	eval();
	k = pop_integer();

	if (k == ERR)
		stop("product 3rd arg: integer value expected");

	p1 = caddr(p1);

	save_symbol(p2);

	h = tos;

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_symbol(p2, p3, symbol(NIL));
		push(p1);
		eval();
		if (j < k)
			j++;
		else if (j > k)
			j--;
		else
			break;
	}

	multiply_factors(tos - h);

	restore_symbol(p2);
}
