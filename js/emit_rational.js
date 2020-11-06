function
emit_rational(u, p)
{
	var num, den, v;

	if (p.b == 1) {
		emit_roman_text(u, Math.abs(p.a).toFixed(0));
		return;
	}

	num = {type:LINE, a:[], level:u.level + 1};
	den = {type:LINE, a:[], level:u.level + 1};

	emit_roman_text(num, Math.abs(p.a).toFixed(0));
	emit_roman_text(den, p.b.toFixed(0));

	if (num.a.length == 1)
		num = num.a[0];
	else
		emit_update(num);

	if (den.a.length == 1)
		den = den.a[0];
	else
		emit_update(den);

	v = {type:FRACTION, num:num, den:den, level:u.level + 1};

	emit_update_fraction(v);

	u.a.push(v);
}
