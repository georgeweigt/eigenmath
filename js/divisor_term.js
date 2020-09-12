function
divisor_term(p)
{
	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);
		while (iscons(p)) {
			if (divisor_factor(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return divisor_factor(p);
}
