#include "defs.h"

void
eval_product(struct atom *p1)
{
	int h, i, j, k, n;
	struct atom *p2, *p3;

	if (length(p1) == 2) {
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

	p2 = cadr(p1);
	if (!isusersymbol(p2))
		stop("product: symbol error");

	push(caddr(p1));
	eval();
	j = pop_integer();

	push(cadddr(p1));
	eval();
	k = pop_integer();

	p1 = caddddr(p1);

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
