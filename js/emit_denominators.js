function
emit_denominators(p, n, small_font) // n is number of denominators
{
	var q, u;

	u = {type:LINE, a:[]};

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (q.b != 1) {
			emit_roman_text(u, q.b.toFixed(0), small_font);
			n++;
		}
		p = cdr(p);
	}

	while (iscons(p)) {

		q = car(p);

		if (car(q) != symbol(POWER) || !isnegativenumber(caddr(q))) {
			p = cdr(p);
			continue; // not a denominator
		}

		if (u.a.length > 0)
			emit_thin_space(u, small_font);

		if (isminusone(caddr(q))) {
			q = cadr(q);
			if (car(q) == symbol(ADD) && n > 1)
				emit_subexpr(u, q, small_font);
			else
				emit_expr(u, q, small_font);
		} else {
			emit_base(u, cadr(q), small_font);
			emit_numeric_exponent(u, caddr(q), small_font); // sign is not emitted
		}

		p = cdr(p);
	}

	if (u.a.length == 1)
		return u.a[0];

	emit_update(u);

	return u;
}
