#include "app.h"

void
emit_numerators(struct atom *p)
{
	int n, t;
	char *s;
	struct atom *q;

	t = tos;

	n = count_numerators(p);

	p = cdr(p);
	q = car(p);

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isnumerator(q))
			continue;

		if (tos > t)
			emit_medium_space();

		if (isrational(q)) {
			s = mstr(q->u.q.a);
			emit_roman_string(s);
			continue;
		}

		if (car(q) == symbol(ADD) && n == 1)
			emit_expr(q); // parens not needed
		else
			emit_factor(q);
	}

	if (t == tos)
		emit_roman_char('1'); // no numerators

	emit_update_list(t);
}
