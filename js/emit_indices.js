function
emit_indices(u, p, small_font)
{
	var v = {type:BRACK, a:[], small_font:small_font};

	emit_roman_text(v, "[", small_font);

	p = cdr(p);

	if (iscons(p)) {
		emit_expr(u, car(p), small_font);
		p = cdr(p);
		while (iscons(p)) {
			emit_roman_text(u, ",", small_font);
			emit_expr(u, car(p), small_font);
			p = cdr(p);
		}
	}

	emit_roman_text(v, "]", small_font);

	emit_update(v);

	u.a.push(v);
}
