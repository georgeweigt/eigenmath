#include "app.h"

void
emit_exponent(struct atom *p)
{
	if (isnum(p) && !isnegativenumber(p)) {
		emit_numeric_exponent(p); // sign is not emitted
		return;
	}

	emit_level++;
	emit_list(p);
	emit_level--;

	emit_update_superscript();
}
