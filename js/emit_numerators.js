function
emit_numerators(p, small_font)
{
	var q, u;

	u = {type:LINE, a:[]};

	p = cdr(p);
	q = car(p);

	while (iscons(p)) {

		q = car(p);

		if (car(q) == symbol(POWER) && isnegativenumber(caddr(q))) {
			p = cdr(p);
			continue; // printed in denominator
		}

		if (u.a.length > 0)
			emit_medium_space(u, small_font);

		if (isrational(q)) {
			if (Math.abs(q.a) != 1)
				emit_roman_text(u, Math.abs(q.a).toFixed(0), small_font);
		} else if (car(q) == symbol(ADD))
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
