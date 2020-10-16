function
emit_main(p)
{
	var u = {type:EXPR, a:[]};

	emit_expr(u, p);

	if (u.a.length == 1)
		return u.a[0];

	emit_update(u);

	return u;
}
