#include "app.h"

void
emit_exponent(struct atom *p)
{
	int t = tos;

	if (isnum(p) && !isnegativenumber(p)) {
		emit_numeric_exponent(p); // sign is not emitted
		return;
	}

	emit_expr(p);
	emit_update_superscript(t);
}
