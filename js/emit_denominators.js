function
emit_denominators(u, p)
{
	var n, p1;

	n = 0;

	p = cdr(p);
	p1 = car(p);

	if (isrational(p1)) {
		if (p1.b != 1) {
			emit_roman(u, p1.b.toFixed(0));
			n++;
		}
		p = cdr(p);
	}

	while (iscons(p)) {

		p1 = car(p);

		if (car(p1) != symbol(POWER) || !isnegativenumber(caddr(p1))) {
			p = cdr(p);
			continue; // not a denominator
		}

		if (n > 0)
			emit_thin_space(u);

		push(p1);
		reciprocate();
		p1 = pop();

		if (car(p1) == symbol(ADD))
			emit_subexpr(u, p1);
		else
			emit_expr(u, p1);

		n++;
		p = cdr(p);
	}

	if (n == 0)
		emit_roman(u, "1"); // there were no denominators
}
