function
emit_line(p, small_font)
{
	var u = {type:LINE, a:[]};
	emit_expr(u, p, small_font);
	if (u.a.length == 1)
		return u.a[0];
	emit_update(u);
	return u;
}
