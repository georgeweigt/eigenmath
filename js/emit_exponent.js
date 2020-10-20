function
emit_exponent(u, p, small_font)
{
	var v;

	if (isnum(p) && !isnegativenumber(p)) {
		emit_numeric_exponent(u, p, small_font); // sign is not emitted
		return;
	}

	v = {type:SUPERSCRIPT, a:[], small_font:small_font};

	emit_expr(v, p, 1);

	emit_update_superscript(v, u.a[u.a.length - 1].height);

	u.a.push(v);
}
