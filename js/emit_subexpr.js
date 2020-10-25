function
emit_subexpr(u, p, small_font)
{
	var v = {type:PAREN, a:[], small_font:small_font};
	emit_expr(v, p, small_font);
	emit_update(v);
	v.width += 2 * emit_delim_width(small_font);
	u.a.push(v);
}
