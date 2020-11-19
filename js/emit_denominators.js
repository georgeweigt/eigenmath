function
emit_denominators(u, p)
{
	var n, q, v;

	n = count_denominators(p);

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
		p = cdr(p);

		if (!isdenominator(q))
			continue;

		if (v.a.length > 0)
			emit_medium_space(v);

		if (isminusone(caddr(q))) {
			q = cadr(q);
			if (car(q) == symbol(ADD) && n == 1)
				emit_expr(v, q); // parens not needed
			else
				emit_factor(v, q);
		} else {
			emit_base(v, cadr(q));
			emit_numeric_exponent(v, caddr(q)); // sign is not emitted
		}
	}

	if (v.a.length == 1)
		v = v.a[0];
	else
		emit_update(v);

	u.den = v;
}
