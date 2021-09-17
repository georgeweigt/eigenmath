function
integral_classify(p)
{
	var t = 0;

	if (iscons(p)) {
		while (iscons(p)) {
			t |= integral_classify(car(p));
			p = cdr(p);
		}
		return t;
	}

	if (p == symbol(EXP1))
		return 1;

	if (p == symbol(LOG))
		return 2;

	if (p == symbol(SIN) || p == symbol(COS) || p == symbol(TAN))
		return 4;

	return 0;
}
