function
emit_denominators(u, p, n) // n is number of denominators
{
	var q, v;

	v = {type:LINE, a:[], level:u.level};

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (q.b != 1) {
			emit_roman_text(v, q.b.toFixed(0));
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

		if (v.a.length > 0)
			emit_medium_space(v);

		if (isminusone(caddr(q))) {
			q = cadr(q);
			if (car(q) == symbol(ADD) && n > 1)
				emit_subexpr(v, q);
			else
				emit_expr(v, q);
		} else {
			emit_base(v, cadr(q));
			emit_numeric_exponent(v, caddr(q)); // sign is not emitted
		}

		p = cdr(p);
	}

	if (v.a.length == 1) {
		u.den = v.a[0];
		return;
	}

	emit_update(v);

	u.den = v;
}
