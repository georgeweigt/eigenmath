#include "app.h"

// p is rational or double, sign is not emitted

void
emit_numeric_exponent(struct atom *p)
{
	int t;
	char *s;

	emit_level++;

	t = tos;

	if (isrational(p)) {
		s = mstr(p->u.q.a);
		emit_roman_string(s);
		if (!MEQUAL(p->u.q.b, 1)) {
			emit_roman_char('/');
			s = mstr(p->u.q.b);
			emit_roman_string(s);
		}
	} else
		emit_double(p);

	emit_update_list(t);

	emit_level--;

	emit_update_superscript();
}
