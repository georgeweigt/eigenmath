function
emit_numerators(u, p)
{
	var n, q;

	n = 0;

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (Math.abs(q.a) != 1) {
			emit_roman(u, Math.abs(q.a).toFixed(0));
			n++;
		}
		p = cdr(p);
	}

	while (iscons(p)) {

		q = car(p);

		if (car(q) == symbol(POWER) && isnegativenumber(caddr(q))) {
			p = cdr(p);
			continue; // printed in denominator
		}

		if (n > 0)
			emit_thin_space(u);

		emit_factor(u, q);
		n++;
		p = cdr(p);
	}

	if (n == 0)
		emit_roman(u, "1"); // there were no numerators

	emit_update(u);
}
