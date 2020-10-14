function
emit_numerators(p)
{
	var n, q;

	n = 0;

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (Math.abs(q.a) != 1) {
			emit_roman(Math.abs(q.a).toFixed(0));
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
			emit_thin_space();

		emit_factor(q);
		n++;
		p = cdr(p);
	}

	if (n == 0)
		emit_roman("1"); // there were no numerators
}
