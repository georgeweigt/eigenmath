function
emit_fraction(u, p, n, small_font) // n is number of denominators
{
	var v = {type:FRACTION, small_font:small_font};
	v.num = emit_numerators(p, small_font);
	v.den = emit_denominators(p, n, small_font);
	emit_update_fraction(v);
	u.a.push(v);
}
