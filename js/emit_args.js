function
emit_args(u, p)
{
	var v = {type:PAREN, a:[], small_font:u.small_font};

	p = cdr(p);

	if (iscons(p)) {
		emit_expr(v, car(p));
		p = cdr(p);
		while (iscons(p)) {
			emit_roman_text(v, ",");
			emit_expr(v, car(p));
			p = cdr(p);
		}
	}

	emit_update_subexpr(v);

	u.a.push(v);
}
