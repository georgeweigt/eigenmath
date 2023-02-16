function
isnumerator(p)
{
	if (car(p) == symbol(POWER) && isnegativenumber(caddr(p)))
		return 0;

	if (isrational(p) && bignum_equal(p.a, 1))
		return 0;

	return 1;
}
