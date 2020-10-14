function
emit_rational(p)
{
	if (p.b == 1)
		emit_roman(Math.abs(p.a).toFixed(0));
	else {
		emit_fraction_begin();
		emit_roman(Math.abs(p.a).toFixed(0));
		emit_fraction_separator();
		emit_roman(p.b.toFixed(0));
		emit_fraction_end();
	}
}
