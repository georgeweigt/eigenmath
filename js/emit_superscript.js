function
emit_superscript(u, p)
{
	var v = {type:SUPERSCRIPT, a:[]};
	emit_expr(v, p);
	emit_update(v);
	u.a.push(v);
}
