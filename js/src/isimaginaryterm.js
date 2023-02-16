function
isimaginaryterm(p)
{
	if (isimaginaryfactor(p))
		return 1;

	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);
		while (iscons(p)) {
			if (isimaginaryfactor(car(p)))
				return 1;
			p = cdr(p);
		}
	}

	return 0;
}
