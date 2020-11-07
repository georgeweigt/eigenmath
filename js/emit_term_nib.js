function
emit_term_nib(u, p)
{
	var n;

	n = count_denominators(p);

	if (n > 0) {
		emit_fraction(u, p);
		return;
	}

	// no denominators

	p = cdr(p);

	if (isminusone(car(p)) && !isdouble(car(p)))
		p = cdr(p); // sign already emitted

	emit_factor(u, car(p));

	p = cdr(p);

	while (iscons(p)) {
		emit_medium_space(u);
		emit_factor(u, car(p));
		p = cdr(p);
	}
}
