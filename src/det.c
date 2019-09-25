#include "defs.h"

void
eval_det(void)
{
	push(cadr(p1));
	eval();
	det();
}

void
det(void)
{
	int h, i, k, n, sign;
	int *a, *c, *d;

	save();

	p1 = pop();

	if (!istensor(p1) || p1->u.tensor->ndim != 2 || p1->u.tensor->dim[0] != p1->u.tensor->dim[1])
		stop("square matrix expected");

	n = p1->u.tensor->dim[0];

	a = (int *) malloc(3 * n * sizeof (int));

	if (a == NULL)
		malloc_kaput();

	c = a + n;

	d = c + n;

	for (i = 0; i < n; i++) {
		a[i] = i;
		c[i] = 0;
		d[i] = 1;
	}

	h = tos;

	sign = 1;

	for (;;) {

		for (i = 0; i < n; i++) {
			k = n * a[i] + i;
			push(p1->u.tensor->elem[k]);
		}

		if (sign == -1) {
			push(minusone);
			multiply_factors(n + 1);
		} else
			multiply_factors(n);

		if (next_permutation(n, a, c, d) == 0)
			break;

		sign = -sign;
	}

	free(a);

	add_terms(tos - h);

	restore();
}

// Knuth's algorithm P

int
next_permutation(int n, int *a, int *c, int *d)
{
	int j, q, s, t;
	j = n - 1;
	s = 0;
	for (;;) {
		q = c[j] + d[j];
		if (q < 0) {
			d[j] = -d[j];
			j--;
			continue;
		}
		if (q == j + 1) {
			if (j == 0)
				return 0;
			s++;
			d[j] = -d[j];
			j--;
			continue;
		}
		break;
	}
	t = a[j - c[j] + s];
	a[j - c[j] + s] = a[j - q + s];
	a[j - q + s] = t;
	c[j] = q;
	return 1;
}
