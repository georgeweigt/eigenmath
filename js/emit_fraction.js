function
emit_fraction(u, p, small_font)
{
	var v = {type:FRACTION};
	v.num = emit_numerators(p, small_font);
	v.den = emit_denominators(p, small_font);
	emit_update_fraction(v);
	u.a.push(v);
}
