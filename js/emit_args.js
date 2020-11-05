function
emit_args(u, p, small_font)
{
	var v = {type:PAREN, a:[], small_font:small_font};

	p = cdr(p);

	if (iscons(p)) {
		emit_expr(v, car(p), small_font);
		p = cdr(p);
		while (iscons(p)) {
			emit_roman_text(v, ",", small_font);
			emit_expr(v, car(p), small_font);
			p = cdr(p);
		}
	}

	emit_update_subexpr(v);

	u.a.push(v);
}
