function
emit_rational(p)
{
	if (p.b == 1)
		emit_roman(Math.abs(p.a).toFixed(0));
	else
		emit_fraction(p);
}
