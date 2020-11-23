#include "app.h"

void
emit_subexpr(struct atom *p)
{
	int t = tos;
	emit_expr(p);
	emit_update_subexpr(t);
}
