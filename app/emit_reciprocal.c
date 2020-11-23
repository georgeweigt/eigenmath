#include "app.h"

// p = y^x where x is a negative number

void
emit_reciprocal(struct atom *p)
{
	int t;

	emit_roman_char('1'); // numerator

	t = tos;

	if (isminusone(caddr(p)))
		emit_expr(cadr(p));
	else {
		emit_base(cadr(p));
		emit_numeric_exponent(caddr(p)); // sign is not emitted
	}

	emit_update_list(t);

	emit_update_fraction();
}
