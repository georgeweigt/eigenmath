#include "app.h"

void
emit_matrix(struct atom *p, int d, int k)
{
	int i, j, m, n, span;

	if (d == p->u.tensor->ndim) {
		emit_list(p->u.tensor->elem[k]);
		return;
	}

	// compute element span

	span = 1;

	for (i = d + 2; i < p->u.tensor->ndim; i++)
		span *= p->u.tensor->dim[i];

	n = p->u.tensor->dim[d];	// number of rows
	m = p->u.tensor->dim[d + 1];	// number of columns

	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			emit_matrix(p, d + 2, k + (i * m + j) * span);

	emit_update_table(n, m);
}
