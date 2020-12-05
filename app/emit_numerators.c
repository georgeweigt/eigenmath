#include "app.h"

void
emit_numerators(struct atom *p)
{
	int t;
	char *s;
	struct atom *q;

	t = tos;

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (!MEQUAL(q->u.q.a, 1)) {
			s = mstr(q->u.q.a);
			emit_roman_string(s);
		}
		p = cdr(p);
	}

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (isdenominator(q))
			continue;

		if (tos > t)
			emit_medium_space();

		emit_factor(q);
	}

	if (t == tos)
		emit_roman_char('1'); // no numerators

	emit_update_list(t);
}
