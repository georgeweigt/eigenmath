function
emit_term(p)
{
	var q, t;

	if (car(p) == symbol(MULTIPLY)) {

		// any denominators?

		q = cdr(p);

		while (iscons(q)) {
			t = car(q);
			if (car(t) == symbol(POWER) && isnegativenumber(caddr(t)))
				return emit_fraction(p);
			q = cdr(q);
		}

		// no denominators

		p = cdr(p);
		q = car(p);

		if (isrational(q) && isminusone(q))
			p = cdr(p); // skip -1

		emit_factor(car(p));
		p = cdr(p);

		while (iscons(p)) {
			emit_thin_space();
			emit_factor(car(p));
			p = cdr(p);
		}
	} else
		emit_factor(p);
}
