#include "app.h"

void
emit_string(struct atom *p)
{
	emit_roman_string(p->u.str);
}
