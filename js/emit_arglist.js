function
emit_arglist(p)
{
	p = cdr(p);
	if (iscons(p)) {
		emit_expr(car(p));
		p = cdr(p);
		while (iscons(p)) {
			emit_roman(",");
			emit_thin_space();
			emit_expr(car(p));
			p = cdr(p);
		}
	}
}
