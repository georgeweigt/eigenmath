#include "app.h"

void
emit_indices(struct atom *p)
{
	emit_roman_char('[');

	p = cdr(p);

	if (iscons(p)) {
		emit_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			emit_roman_char(',');
			emit_expr(car(p));
			p = cdr(p);
		}
	}

	emit_roman_char(']');
}
