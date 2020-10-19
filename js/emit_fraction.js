function
emit_fraction(u, p, m, n, small_font) // m is number of numerators, n is number of denominators
{
	var v = {type:FRACTION, small_font:small_font};

	v.num = emit_numerators(p, m, small_font);
	v.den = emit_denominators(p, n, small_font);

	emit_update_fraction(v);

	u.a.push(v);
}
