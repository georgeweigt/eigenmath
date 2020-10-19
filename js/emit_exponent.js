function
emit_exponent(u, p, small_font)
{
	var v = {type:SUPERSCRIPT, a:[], small_font:small_font};

	emit_expr(v, p, 1);

	emit_update_superscript(v, small_font);

	u.a.push(v);
}
