function
emit_function(p)
{
	// d(f(x),x)

	if (car(p) == symbol(DERIVATIVE)) {
		emit_roman("d");
		emit_paren_begin();
		emit_arglist(p);
		emit_paren_end();
		return;
	}

	// n!

	if (car(p) == symbol(FACTORIAL)) {
		p = cadr(p);
		if (isposint(p) || issymbol(p))
			emit_expr(p);
		else
			emit_subexpr(p);
		emit_roman("!");
		return;
	}

	// A[1,2]

	if (car(p) == symbol(INDEX)) {
		p = cdr(p);
		if (issymbol(car(p)))
			emit_symbol(car(p));
		else
			emit_subexpr(car(p));
		emit_bracket_begin();
		emit_arglist(p);
		emit_bracket_end();
		return;
	}

	if (car(p) == symbol(SETQ)) {
		emit_expr(cadr(p));
		emit_thick_space();
		emit_roman("=");
		emit_thick_space();
		emit_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTEQ)) {
		emit_expr(cadr(p));
		emit_thick_space();
		emit_roman("=");
		emit_thick_space();
		emit_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGE)) {
		emit_expr(cadr(p));
		emit_thick_space();
		emit_roman("&ge;");
		emit_thick_space();
		emit_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTGT)) {
		emit_expr(cadr(p));
		emit_thick_space();
		emit_roman("&gt;");
		emit_thick_space();
		emit_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLE)) {
		emit_expr(cadr(p));
		emit_thick_space();
		emit_roman("&le;");
		emit_thick_space();
		emit_expr(caddr(p));
		return;
	}

	if (car(p) == symbol(TESTLT)) {
		emit_expr(cadr(p));
		emit_thick_space();
		emit_roman("&lt;");
		emit_thick_space();
		emit_expr(caddr(p));
		return;
	}

	// default

	if (issymbol(car(p)))
		emit_symbol(car(p));
	else
		emit_subexpr(car(p));

	emit_paren_begin();
	emit_arglist(p);
	emit_paren_end();
}
