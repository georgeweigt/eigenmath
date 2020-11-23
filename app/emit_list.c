#include "app.h"

void
emit_list(struct atom *p)
{
	int t = tos;
	emit_expr(p);
	emit_update_list(t);
}
