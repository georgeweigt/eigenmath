function
divisor(p)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		while (iscons(p)) {
			if (divisor_term(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return divisor_term(p);
}
