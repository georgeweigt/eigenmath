// Scan expr for vars, return in vector
//
// Input:		Expression on stack
//
// Output:		Vector

#include "defs.h"

int global_h;

void
variables(void)
{
	int i, n;
	save();
	p1 = pop();
	global_h = tos;
	lscan(p1);
	n = tos - global_h;
	if (n > 1)
		qsort(stack + global_h, n, sizeof (struct atom *), var_cmp);
	p1 = alloc_tensor(n);
	p1->u.tensor->ndim = 1;
	p1->u.tensor->dim[0] = n;
	for (i = 0; i < n; i++)
		p1->u.tensor->elem[i] = stack[i];
	tos = global_h;
	push(p1);
	restore();
}

void
lscan(struct atom *p)
{
	int i;
	if (iscons(p)) {
		p = cdr(p);
		while (iscons(p)) {
			lscan(car(p));
			p = cdr(p);
		}
	} else if (issymbol(p) && p != symbol(EXP1)) {
		for (i = global_h; i < tos; i++)
			if (stack[i] == p)
				return;
		push(p);
	}
}

int
var_cmp(const void *p1, const void *p2)
{
	return cmp_expr(*((struct atom **) p1), *((struct atom **) p2));
}
