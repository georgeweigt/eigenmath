function
emit_term_nib(u, p, small_font)
{
	var n, q, t;

	// count denominators

	n = 0;
	q = cdr(p);
	while (iscons(q)) {
		t = car(q);
		if (car(t) == symbol(POWER) && isnegativenumber(caddr(t)))
			n++
		q = cdr(q);
	}

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
