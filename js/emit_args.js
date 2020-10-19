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

	emit_update(v);

	if (small_font)
		v.width += 2 * SMALL_FONT_WIDTH;
	else
		v.width += 2 * FONT_WIDTH;

	u.a.push(v);
}
