#include "defs.h"

void
eval_hilbert(void)
{
	push(cadr(p1));
	eval();
	hilbert();
}

#undef A
#undef N
#undef AELEM

#define A p1
#define N p2
#define AELEM(i, j) A->u.tensor->elem[i * n + j]

void
hilbert(void)
{
	int i, j, n;
	save();
	N = pop();
	push(N);
	n = pop_integer();
	if (n < 2)
		stop("hilbert arg");
	push_zero_matrix(n, n);
	A = pop();
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			push_integer(i + j + 1);
			reciprocate();
			AELEM(i, j) = pop();
		}
	}
	push(A);
	restore();
}

void
push_zero_matrix(int i, int j)
{
	push(alloc_tensor(i * j));
	stack[tos - 1]->u.tensor->ndim = 2;
	stack[tos - 1]->u.tensor->dim[0] = i;
	stack[tos - 1]->u.tensor->dim[1] = j;
}
