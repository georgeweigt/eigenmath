function
emit_function(u, p)
{
	// d(f(x),x)

	if (car(p) == symbol(DERIVATIVE)) {
		emit_roman(u, "d");
		emit_arglist(u, p);
		return;
	}

	// n!

	if (car(p) == symbol(FACTORIAL)) {
		p = cadr(p);
		if (isposint(p) || issymbol(p))
			emit_expr(u, p);
		else
			emit_subexpr(u, p);
		emit_roman(u, "!");
		return;
	}

	// A[1,2]

	if (car(p) == symbol(INDEX)) {
		p = cdr(p);
		if (issymbol(car(p)))
			emit_symbol(u, car(p));
		else
			emit_subexpr(u, car(p));
		emit_index_list(u, p);
		return;
	}

	if (car(p) == symbol(SETQ) || car(p) == symbol(TESTEQ)) {
		emit_expr(u, cadr(p));
		emit_infix_operator(u, "=");
		emit_expr(u, caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGT)) {
		emit_expr(u, cadr(p));
		emit_infix_operator(u, "&gt;");
		emit_expr(u, caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLE)) {
		emit_expr(u, cadr(p));
		emit_infix_operator(u, "&le;");
		emit_expr(u, caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLT)) {
		emit_expr(u, cadr(p));
		emit_infix_operator(u, "&lt;");
		emit_expr(u, caddr(p));
		return;
	}

	// default

	if (issymbol(car(p)))
		emit_symbol(u, car(p));
	else
		emit_subexpr(u, car(p));

	emit_arglist(u, p);
}
