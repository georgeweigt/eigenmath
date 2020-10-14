function
emit_term(p)
{
	var q, t;

	if (car(p) == symbol(MULTIPLY)) {

		// any denominators?

		t = cdr(p);

		while (iscons(t)) {
			q = car(t);
			if (car(q) == symbol(POWER) && isnegativenumber(caddr(q)))
				break;
			t = cdr(t);
		}

		if (iscons(t)) {
			emit_fraction_begin();
			emit_numerators(p);
			emit_fraction_separator();
			emit_denominators(p);
			emit_fraction_end();
			return;
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
