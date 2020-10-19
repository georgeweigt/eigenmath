function
emit_exponent(u, p, small_font)
{
	var v = {type:SUPERSCRIPT, a:[], small_font:small_font};

	emit_expr(v, p, 1);

	emit_update(v);

	if (small_font) {
		v.height = v.height + v.depth + SMALL_X_HEIGHT;
		v.depth = -SMALL_X_HEIGHT;
	} else {
		v.height = v.height + v.depth + X_HEIGHT;
		v.depth = -X_HEIGHT;
	}

	u.a.push(v);
}
