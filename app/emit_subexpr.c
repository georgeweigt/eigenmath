#include "app.h"

void
emit_subexpr(struct atom *p)
{
	emit_list(p);
	emit_update_subexpr();
}
