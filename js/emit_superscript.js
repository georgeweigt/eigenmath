function
emit_superscript(u, p)
{
	var v = {type:SUPERSCRIPT, a:[], small_font:1};
	emit_expr(v, p);
	emit_update(v);
	u.a.push(v);
}
