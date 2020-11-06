function
emit_line(p, level)
{
	var u = {type:LINE, a:[], level:level};

	emit_expr(u, p);

	if (u.a.length == 1)
		return u.a[0];

	emit_update(u);

	return u;
}
