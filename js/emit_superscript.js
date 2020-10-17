function
emit_superscript(u, p)
{
	var v = {type:SUPERSCRIPT, a:[], small_font:1};
	emit_expr(v, p);
	emit_update(v);
	if (u.small_font) {
		v.height = v.height + v.depth + SMALL_X_HEIGHT;
		v.depth = 0;
	} else {
		v.height = v.height + v.depth + X_HEIGHT;
		v.depth = 0;
	}
	u.a.push(v);
}
