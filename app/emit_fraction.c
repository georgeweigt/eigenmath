#include "app.h"

void
emit_frac(struct atom *p)
{
	emit_numerators(p);
	emit_denominators(p);
	emit_update_fraction();
}
