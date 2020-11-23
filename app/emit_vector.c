#include "app.h"

void
emit_vector(struct atom *p)
{
	int i, n, span, t;

	// compute element span

	span = 1;

	for (i = 1; i < p->u.tensor->ndim; i++)
		span *= p->u.tensor->dim[i];

	t = tos;

	n = p->u.tensor->dim[0]; // number of rows

	for (i = 0; i < n; i++)
		emit_matrix(p, 1, i * span);

	emit_update_table(n, 1);
}
