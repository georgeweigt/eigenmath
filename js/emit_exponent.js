function
emit_exponent(u, p) // p is rational or double
{
	var v = {type:SUPERSCRIPT, a:[], small_font:1};

	if (isrational(p))
		emit_rational(v, p, 1); // sign not emitted
	else
		emit_double(v, p, 1); // sign not emitted

	emit_update(v);

	u.a.push(v);
}
