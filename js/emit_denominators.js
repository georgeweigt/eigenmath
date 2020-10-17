function
emit_denominators(p, small_font)
{
	var n, q, u;

	u = {type:EXPR, a:[]};

	n = 0;

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (q.b != 1) {
			emit_roman(u, q.b.toFixed(0), small_font);
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

		if (n > 0)
			emit_thin_space(u, small_font);

		push(q);
		reciprocate();
		q = pop();

		if (car(q) == symbol(ADD))
			emit_subexpr(u, q, small_font);
		else
			emit_expr(u, q, small_font);

		n++;
		p = cdr(p);
	}

	if (n == 0)
		emit_roman(u, "1", small_font); // there were no denominators

	if (u.a.length == 1)
		return u.a[0];

	emit_update(u);

	return u;
}
