function
emit_fraction(u, p)
{
	var v = {type:FRACTION, level:u.level};

	emit_numerators(v, p);
	emit_denominators(v, p);

	emit_update_fraction(v);

	u.a.push(v);
}
