function
emit_subexpr(u, p, small_font)
{
	var v = {type:PAREN, a:[], small_font:small_font};

	emit_roman_text(v, "(", small_font);
	emit_expr(v, p, small_font);
	emit_roman_text(v, ")", small_font);

	emit_update(v);

	u.a.push(v);
}
