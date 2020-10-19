function
emit_numerators(p, m, small_font) // m is number of numerators
{
	var q, u;

	u = {type:LINE, a:[]};

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (Math.abs(q.a) != 1) {
			emit_roman_text(u, Math.abs(q.a).toFixed(0), small_font);
			m++;
		}
		p = cdr(p);
	}

	while (iscons(p)) {

		q = car(p);

		if (car(q) == symbol(POWER) && isnegativenumber(caddr(q))) {
			p = cdr(p);
			continue; // printed in denominator
		}

		if (u.a.length > 0)
			emit_thin_space(u, small_font);

		if (car(q) == symbol(ADD) && m > 1)
			emit_subexpr(u, q, small_font);
		else
			emit_expr(u, q, small_font);

		p = cdr(p);
	}

	if (u.a.length == 0)
		emit_roman_text(u, "1", small_font); // there were no numerators

	if (u.a.length == 1)
		return u.a[0];

	emit_update(u);

	return u;
}
