#include "app.h"

void
emit_vector(struct atom *p)
{
	int i, n, span;

	// compute element span

	span = 1;

	n = p->u.tensor->ndim;

	for (i = 1; i < n; i++)
		span *= p->u.tensor->dim[i];

	n = p->u.tensor->dim[0]; // number of rows

	for (i = 0; i < n; i++)
		emit_matrix(p, 1, i * span);

	emit_update_table(n, 1); // n rows, 1 column
}
