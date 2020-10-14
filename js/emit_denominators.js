function
emit_denominators(p)
{
	var n, q;

	n = 0;

	p = cdr(p);
	q = car(p);

	if (isrational(q)) {
		if (q.b != 1) {
			emit_roman(q.b.toFixed(0));
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
			emit_thin_space();

		emit_base(cadr(q));

		if (!isminusone(caddr(q))) {
			emit_superscript_begin();
			emit_number(caddr(q)); // sign is not emitted
			emit_superscript_end();
		}

		n++;
		p = cdr(p);
	}

	if (n == 0)
		emit_roman("1"); // there were no denominators
}
