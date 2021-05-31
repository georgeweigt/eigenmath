#include "defs.h"

void
eval_sum(void)
{
	int h, i, j, k, n;

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
		add_terms(n);
		return;
	}

	p2 = cadr(p1);

	if (!isusersymbol(p2))
		stop("sum: 1st arg?");

	p1 = cddr(p1);

	push(car(p1));
	eval();
	j = pop_integer();

	if (j == ERR)
		stop("sum: 2nd arg?");

	push(cadr(p1));
	eval();
	k = pop_integer();

	if (k == ERR)
		stop("sum: 3rd arg?");

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

	add_terms(tos - h);

	restore_symbol(p2);
}
