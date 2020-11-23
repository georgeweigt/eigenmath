#include "app.h"

void
emit_expr(struct atom *p)
{
	if (isnegativeterm(p) || (car(p) == symbol(ADD) && isnegativeterm(cadr(p)))) {
		emit_roman_char(MINUS);
		emit_thin_space();
	}

	if (car(p) == symbol(ADD))
		emit_expr_nib(p);
	else
		emit_term(p);
}

void
emit_expr_nib(struct atom *p)
{
	p = cdr(p);
	emit_term(car(p));
	p = cdr(p);
	while (iscons(p)) {
		if (isnegativeterm(car(p)))
			emit_infix_operator(MINUS);
		else
			emit_infix_operator('+');
		emit_term(car(p));
		p = cdr(p);
	}
}
