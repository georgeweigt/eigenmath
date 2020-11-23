#include "app.h"

void
emit_base(struct atom *p)
{
	if (isnegativenumber(p) || isfraction(p) || isdouble(p) || car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY) || car(p) == symbol(POWER))
		emit_subexpr(p);
	else
		emit_expr(p);
}
