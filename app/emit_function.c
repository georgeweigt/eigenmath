#include "app.h"

void
emit_function(struct atom *p)
{
	// d(f(x),x)

	if (car(p) == symbol(DERIVATIVE)) {
		emit_roman_char('d');
		emit_args(p);
		return;
	}

	// n!

	if (car(p) == symbol(FACTORIAL)) {
		p = cadr(p);
		if (isposint(p) || issymbol(p))
			emit_expr(p);
		else
			emit_subexpr(p);
		emit_roman_char('!');
		return;
	}

	// A[1,2]

	if (car(p) == symbol(INDEX)) {
		p = cdr(p);
		if (issymbol(car(p)))
			emit_symbol(car(p));
		else
			emit_subexpr(car(p));
		emit_indices(p);
		return;
	}

	if (car(p) == symbol(SETQ) || car(p) == symbol(TESTEQ)) {
		emit_expr(cadr(p));
		emit_infix_operator('=');
		emit_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGE)) {
		emit_expr(cadr(p));
		emit_infix_operator(GREATEREQUAL);
		emit_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGT)) {
		emit_expr(cadr(p));
		emit_infix_operator('>');
		emit_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLE)) {
		emit_expr(cadr(p));
		emit_infix_operator(LESSEQUAL);
		emit_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLT)) {
		emit_expr(cadr(p));
		emit_infix_operator('<');
		emit_expr(caddr(p));
		return;
	}

	// default

	if (issymbol(car(p)))
		emit_symbol(car(p));
	else
		emit_subexpr(car(p));

	emit_args(p);
}
