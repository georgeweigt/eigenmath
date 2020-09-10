function
isdoublez(p)
{
	if (car(p) == symbol(ADD)) {

		if (length(p) != 3)
			return 0;

		if (!isdouble(cadr(p))) // x
			return 0;

		p = caddr(p);
	}

	if (car(p) != symbol(MULTIPLY))
		return 0;

	if (length(p) != 3)
		return 0;

	if (!isdouble(cadr(p))) // y
		return 0;

	p = caddr(p);

	if (car(p) != symbol(POWER))
		return 0;

	if (!equaln(cadr(p), -1))
		return 0;

	if (!equalq(caddr(p), 1, 2))
		return 0;

	return 1;
}
