#include "app.h"

void
emit_denominators(struct atom *p)
{
	int n, t;
	char *s;
	struct atom *q;

	t = tos;
	n = count_denominators(p);
	p = cdr(p);

	while (iscons(p)) {

		q = car(p);
		p = cdr(p);

		if (!isdenominator(q))
			continue;

		if (tos > t)
			emit_medium_space();

		if (isrational(q)) {
			s = mstr(q->u.q.b);
			emit_roman_string(s);
			continue;
		}

		if (isminusone(caddr(q))) {
			q = cadr(q);
			if (car(q) == symbol(ADD) && n == 1)
				emit_expr(q); // parens not needed
			else
				emit_factor(q);
		} else {
			emit_base(cadr(q));
			emit_numeric_exponent(caddr(q)); // sign is not emitted
		}
	}

	emit_update_list(t);
}
