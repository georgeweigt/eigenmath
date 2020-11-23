#include "app.h"

void
emit_rational(struct atom *p)
{
	int t;
	char *s;

	if (MEQUAL(p->u.q.b, 1)) {
		s = mstr(p->u.q.a);
		emit_roman_string(s);
		return;
	}

	emit_level++;

	t = tos;
	s = mstr(p->u.q.a);
	emit_roman_string(s);
	emit_update_list(t);

	t = tos;
	s = mstr(p->u.q.b);
	emit_roman_string(s);
	emit_update_list(t);

	emit_level--;

	emit_update_fraction();
}
