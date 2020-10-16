function
emit_rational(u, p)
{
	if (p.b == 1)
		emit_roman(u, Math.abs(p.a).toFixed(0));
	else
		emit_fraction(u, p);
}
