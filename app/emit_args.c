#include "app.h"

void
emit_args(struct atom *p)
{
	int t;

	emit_thin_space();

	p = cdr(p);

	if (!iscons(p)) {
		emit_roman_char('(');
		emit_roman_char(')');
		return;
	}

	t = tos;

	emit_expr(car(p));

	p = cdr(p);

	while (iscons(p)) {
		emit_roman_char(',');
		emit_expr(car(p));
		p = cdr(p);
	}

	emit_update_list(t);

	emit_update_subexpr();
}
