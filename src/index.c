#include "defs.h"

#undef T
#define T p3

void
eval_index(void)
{
	int h, n;

	T = cadr(p1);

	p1 = cddr(p1);

	h = tos;

	while (iscons(p1)) {
		push(car(p1));
		eval();
		p1 = cdr(p1);
	}

	n = tos - h; // number of indices on stack

	// try to optimize by indexing before eval

	if (isusersymbol(T)) {
		p1 = get_binding(T);
		if (istensor(p1) && n <= p1->u.tensor->ndim) {
			T = p1;
			indexfunc(h);
			eval();
			return;
		}
	}

	push(T);
	eval();
	T = pop();

	if (!istensor(T)) {
		tos = h; // pop all
		push(T); // quirky, but EVA2.txt depends on it
		return;
	}

	indexfunc(h);
}

void
indexfunc(int h)
{
	int i, k, m, n, t, w;

	n = tos - h; // number of indices

	m = T->u.tensor->ndim;

	if (n < 1 || n > m)
		stop("index error");

	k = 0;

	for (i = 0; i < n; i++) {
		push(stack[h + i]);
		t = pop_integer();
		if (t < 1 || t > T->u.tensor->dim[i])
			stop("index error");
		k = k * T->u.tensor->dim[i] + t - 1;
	}

	tos = h; // pop all

	if (n == m) {
		push(T->u.tensor->elem[k]); // scalar result
		return;
	}

	w = 1;

	for (i = n; i < m; i++)
		w *= T->u.tensor->dim[i];

	k *= w;

	p1 = alloc_tensor(w);

	for (i = 0; i < w; i++)
		p1->u.tensor->elem[i] = T->u.tensor->elem[k + i];

	p1->u.tensor->ndim = m - n;

	for (i = 0; i < m - n; i++)
		p1->u.tensor->dim[i] = T->u.tensor->dim[n + i];

	push(p1);
}
