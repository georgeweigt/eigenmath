function
emit_line(p, small)
{
	var u = {type:LINE, a:[], small_font:small};

	emit_expr(u, p);

	if (u.a.length == 1)
		return u.a[0];

	emit_update(u);

	return u;
}
