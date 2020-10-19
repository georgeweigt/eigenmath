function
emit_numeric_exponent(u, p, small_font) // p is rational or double, sign is not emitted
{
	var v = {type:SUPERSCRIPT, a:[], small_font:small_font};

	if (isrational(p))
		emit_rational(v, p, 1);
	else
		emit_double(v, p, 1);

	emit_update_superscript(v, small_font);

	u.a.push(v);
}
