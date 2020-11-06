function
emit_subexpr(u, p)
{
	var v = {type:PAREN, a:[], small_font:u.small_font};
	emit_expr(v, p);
	emit_update_subexpr(v);
	u.a.push(v);
}
