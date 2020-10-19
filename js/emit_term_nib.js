function
emit_term_nib(u, p, small_font)
{
	var m = 0, n = 0, q, t;

	// count numerators and denominators

	t = p;

	p = cdr(p);
	while (iscons(p)) {
		q = car(p);
		if (car(q) == symbol(POWER) && isnegativenumber(caddr(q)))
			n++;
		else
			m++;
		p = cdr(p);
	}

	p = t;

	if (n > 0) {
		emit_fraction(u, p, m, n, small_font);
		return;
	}

	// no denominators

	p = cdr(p);

	if (isrational(car(p)) && isminusone(car(p)))
		p = cdr(p); // skip -1

	emit_factor(u, car(p), small_font);

	p = cdr(p);

	while (iscons(p)) {
		emit_thin_space(u, small_font);
		emit_factor(u, car(p), small_font);
		p = cdr(p);
	}
}
