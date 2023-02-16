function
isdenominator(p)
{
	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 1;

	if (isrational(p) && !bignum_equal(p.b, 1))
		return 1;

	return 0;
}
