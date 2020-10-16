function
emit_term_nib(u, p)
{
	var q, t;

	// any denominators?

	q = cdr(p);

	while (iscons(q)) {
		t = car(q);
		if (car(t) == symbol(POWER) && isnegativenumber(caddr(t))) {
			u.a.push(emit_fraction(p));
			return;
		}
		q = cdr(q);
	}

	// no denominators

	p = cdr(p);

	if (isrational(car(p)) && isminusone(car(p)))
		p = cdr(p); // skip -1

	emit_factor(u, car(p));

	p = cdr(p);

	while (iscons(p)) {
		emit_thin_space(u);
		emit_factor(u, car(p));
		p = cdr(p);
	}
}
