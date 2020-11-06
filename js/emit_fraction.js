function
emit_fraction(u, p, n) // n is number of denominators
{
	var v = {type:FRACTION, level:u.level};

	emit_numerators(v, p);
	emit_denominators(v, p, n);

	emit_update_fraction(v);

	u.a.push(v);
}
