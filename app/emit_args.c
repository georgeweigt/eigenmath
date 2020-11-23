#include "app.h"

void
emit_args(struct atom *p)
{
	int t = tos;

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

	emit_update_subexpr(t);
}
