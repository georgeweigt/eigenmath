function
emit_numeric_exponent(u, p) // p is rational or double, sign is not emitted
{
	var v = {type:SUPERSCRIPT, a:[], small_font:1};

	if (isrational(p))
		emit_rational(v, p, 1);
	else
		emit_double(v, p, 1);

	emit_update(v);

	u.a.push(v);
}
