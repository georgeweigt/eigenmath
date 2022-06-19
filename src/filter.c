/* Remove terms that involve a given symbol or expression. For example...

	filter(x^2 + x + 1, x)		=>	1

	filter(x^2 + x + 1, x^2)	=>	x + 1
*/

#include "defs.h"

void
eval_filter(struct atom *p1)
{
	push(cadr(p1));
	eval();

	p1 = cddr(p1);

	while (iscons(p1)) {
		push(car(p1));
		eval();
		filter();
		p1 = cdr(p1);
	}
}

void
filter(void)
{
	struct atom *p1, *p2;

	p2 = pop();
	p1 = pop();

	if (car(p1) == symbol(ADD))
		filter_sum(p1, p2);
	else if (istensor(p1))
		filter_tensor(p1, p2);
	else if (find(p1, p2))
		push_integer(0);
	else
		push(p1);
}

void
filter_sum(struct atom *p1, struct atom *p2)
{
	push_integer(0);
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		push(p2);
		filter();
		add();
		p1 = cdr(p1);
	}
}

void
filter_tensor(struct atom *p1, struct atom *p2)
{
	int i, n;
	struct atom *p3;
	n = p1->u.tensor->nelem;
	p3 = alloc_tensor(n);
	p3->u.tensor->ndim = p1->u.tensor->ndim;
	for (i = 0; i < p1->u.tensor->ndim; i++)
		p3->u.tensor->dim[i] = p1->u.tensor->dim[i];
	for (i = 0; i < n; i++) {
		push(p1->u.tensor->elem[i]);
		push(p2);
		filter();
		p3->u.tensor->elem[i] = pop();
	}
	push(p3);
}
