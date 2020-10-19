function
emit_indices(u, p, small_font)
{
	var v = {type:BRACK, a:[], small_font:small_font};

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

	emit_update(v);

	if (small_font)
		v.width += 2 * SMALL_FONT_WIDTH;
	else
		v.width += 2 * FONT_WIDTH;

	u.a.push(v);
}
