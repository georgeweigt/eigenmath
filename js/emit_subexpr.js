function
emit_subexpr(u, p, small_font)
{
	var v = {type:PAREN, a:[], small_font:small_font};
	emit_expr(v, p, small_font);
	emit_update(v);
	if (small_font)
		v.width += 2 * SMALL_FONT_WIDTH;
	else
		v.width += 2 * FONT_WIDTH;
	u.a.push(v);
}
