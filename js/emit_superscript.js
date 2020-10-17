function
emit_superscript(u, p)
{
	var v = {type:SUPERSCRIPT, a:[]};
	emit_expr(v, p, 1);
	emit_update(v);
	v.height = v.height + v.depth + SMALL_X_HEIGHT;
	v.depth = -SMALL_X_HEIGHT;
	u.a.push(v);
}
