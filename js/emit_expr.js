function
emit_expr(u, p)
{
	var q;
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		q = car(p);
		if (isnegativenumber(q) || (car(q) == symbol(MULTIPLY) && isnegativenumber(cadr(q))))
			emit_minus_sign(u);
		emit_term(u, q);
		p = cdr(p);
		while (iscons(p)) {
			q = car(p);
			if (isnegativenumber(q) || (car(q) == symbol(MULTIPLY) && isnegativenumber(cadr(q))))
				emit_infix_minus(u);
			else
				emit_infix_plus(u);
			emit_term(u, q);
			p = cdr(p);
		}
	} else {
		if (isnegativenumber(p) || (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p))))
			emit_minus_sign(u);
		emit_term(u, p);
	}
}
