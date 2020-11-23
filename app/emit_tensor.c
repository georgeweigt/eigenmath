#include "app.h"

void
emit_tensor(struct atom *p)
{
	if (p->u.tensor->ndim % 2 == 1)
		emit_vector(p); // odd rank
	else
		emit_matrix(p, 0, 0); // even rank
}
