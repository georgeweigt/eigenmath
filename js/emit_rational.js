function
emit_rational(u, p)
{
	var d, n, v;

	if (p.b == 1) {
		emit_roman(u, Math.abs(p.a).toFixed(0));
		return;
	}

	n = {a:[], script_size:1};
	d = {a:[], script_size:1};

	emit_roman(n, Math.abs(p.a).toFixed(0));
	emit_roman(d, p.b.toFixed(0));

	v = {type:FRACTION, num:n.a[0], den:d.a[0]};

	emit_update_fraction(v);

	u.a.push(v);
}
