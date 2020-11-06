function
emit_expr_nib(u, p)
{
	p = cdr(p);
	emit_term(u, car(p));
	p = cdr(p);
	while (iscons(p)) {
		if (isnegativeterm(car(p)))
			emit_infix_operator(u, "minus");
		else
			emit_infix_operator(u, "plus");
		emit_term(u, car(p));
		p = cdr(p);
	}
}
