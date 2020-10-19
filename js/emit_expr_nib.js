function
emit_expr_nib(u, p, small_font)
{
	p = cdr(p);
	emit_term(u, car(p), small_font);
	p = cdr(p);
	while (iscons(p)) {
		if (isnegativeterm(car(p)))
			emit_infix_operator(u, "&minus;", small_font);
		else
			emit_infix_operator(u, "&plus;", small_font);
		emit_term(u, car(p), small_font);
		p = cdr(p);
	}
}
