function
emit_term_nib(u, p, small_font)
{
	var n = 0, q, t;

	// count denominators

	t = p;

	p = cdr(p);
	while (iscons(p)) {
		q = car(p);
		if (car(q) == symbol(POWER) && isnegativenumber(caddr(q)))
			n++;
		p = cdr(p);
	}

	p = t;

	if (n > 0) {
		emit_fraction(u, p, n, small_font);
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
