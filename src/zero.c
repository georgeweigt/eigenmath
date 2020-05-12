#include "defs.h"

void
eval_zero(void)
{
	int dim[MAXDIM], i, m, n;
	m = 1;
	n = 0;
	p1 = cdr(p1);
	while (iscons(p1)) {
		if (n == MAXDIM)
			stop("zero: rank exceeds max");
		push(car(p1));
		eval();
		i = pop_integer();
		if (i == ERR || i < 2)
			stop("zero: dimension error");
		m *= i;
		dim[n++] = i;
		p1 = cdr(p1);
	}
	if (n == 0) {
		push(zero);
		return;
	}
	p1 = alloc_tensor(m);
	p1->u.tensor->ndim = n;
	for (i = 0; i < n; i++)
		p1->u.tensor->dim[i] = dim[i];
	push(p1);
}
