function
emit_rational(u, p, small_font)
{
	var num, den, v;

	if (p.b == 1) {
		emit_roman(u, Math.abs(p.a).toFixed(0), small_font);
		return;
	}

	emit_roman(u, Math.abs(p.a).toFixed(0), 1);
	num = u.a.pop();

	emit_roman(u, p.b.toFixed(0), 1);
	den = u.a.pop();

	v = {type:FRACTION, num:num, den:den, small_font:1};

	emit_update_fraction(v);

	u.a.push(v);
}
