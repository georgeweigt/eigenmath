function
emit_rational(u, p, small_font)
{
	var num, den, v;

	if (p.b == 1) {
		emit_roman_text(u, Math.abs(p.a).toFixed(0), small_font);
		return;
	}

	num = {type:LINE, a:[]};
	den = {type:LINE, a:[]};

	emit_roman_text(num, Math.abs(p.a).toFixed(0), 1);
	emit_roman_text(den, p.b.toFixed(0), 1);

	if (num.a.length == 1)
		num = num.a[0];
	else
		emit_update(num);

	if (den.a.length == 1)
		den = den.a[0];
	else
		emit_update(den);

	v = {type:FRACTION, num:num, den:den, small_font:1};

	emit_update_fraction(v);

	u.a.push(v);
}
