function
emit_subexpr(u, p)
{
	var v = {type:PAREN, a:[]};
	emit_expr(v, p);
	emit_update(v);
	v.width += 2 * FONT_WIDTH; // for parens
	u.a.push(v);
}
