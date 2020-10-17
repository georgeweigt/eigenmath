function
emit_rational(u, p)
{
	var d, n, v;

	if (p.b == 1) {
		emit_roman(u, Math.abs(p.a).toFixed(0));
		return;
	}

	n = {a:[], small_font:1};
	d = {a:[], small_font:1};

	emit_roman(n, Math.abs(p.a).toFixed(0));
	emit_roman(d, p.b.toFixed(0));

	n = n.a[0];
	d = d.a[0];

	v = {type:FRACTION, num:n, den:d, small_font:1};

	emit_update_fraction(v);

	u.a.push(v);
}
