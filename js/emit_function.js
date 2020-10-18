function
emit_function(u, p, small_font)
{
	// d(f(x),x)

	if (car(p) == symbol(DERIVATIVE)) {
		emit_roman_text(u, "d", small_font);
		emit_arglist(u, p, small_font);
		return;
	}

	// n!

	if (car(p) == symbol(FACTORIAL)) {
		p = cadr(p);
		if (isposint(p) || issymbol(p))
			emit_expr(u, p, small_font);
		else
			emit_subexpr(u, p, small_font);
		emit_roman_text(u, "!", small_font);
		return;
	}

	// A[1,2]

	if (car(p) == symbol(INDEX)) {
		p = cdr(p);
		if (issymbol(car(p)))
			emit_symbol(u, car(p), small_font);
		else
			emit_subexpr(u, car(p), small_font);
		emit_index_list(u, p, small_font);
		return;
	}

	if (car(p) == symbol(SETQ) || car(p) == symbol(TESTEQ)) {
		emit_expr(u, cadr(p), small_font);
		emit_infix_operator(u, "equals", small_font);
		emit_expr(u, caddr(p), small_font);
		return;
	}

	if (car(p) == symbol(TESTGT)) {
		emit_expr(u, cadr(p), small_font);
		emit_infix_operator(u, "gt", small_font);
		emit_expr(u, caddr(p), small_font);
		return;
	}

	if (car(p) == symbol(TESTLE)) {
		emit_expr(u, cadr(p), small_font);
		emit_infix_operator(u, "le", small_font);
		emit_expr(u, caddr(p), small_font);
		return;
	}

	if (car(p) == symbol(TESTLT)) {
		emit_expr(u, cadr(p), small_font);
		emit_infix_operator(u, "lt", small_font);
		emit_expr(u, caddr(p));
		return;
	}

	// default

	if (issymbol(car(p)))
		emit_symbol(u, car(p), small_font);
	else
		emit_subexpr(u, car(p), small_font);

	emit_arglist(u, p, small_font);
}
