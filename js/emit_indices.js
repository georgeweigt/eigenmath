function
emit_indices(u, p)
{
	emit_roman_text(u, "[");

	p = cdr(p);

	if (iscons(p)) {
		emit_expr(u, car(p));
		p = cdr(p);
		while (iscons(p)) {
			emit_roman_text(u, ",");
			emit_expr(u, car(p));
			p = cdr(p);
		}
	}

	emit_roman_text(u, "]");
}
