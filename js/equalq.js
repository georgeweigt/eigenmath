function
equalq(p, a, b)
{
	if (isrational(p)) {
		if (isnegativenumber(p) && a >= 0)
			return 0;
		if (!isnegativenumber(p) && a < 0)
			return 0;
		a = Math.abs(a);
		return bignum_equal(p.a, a) && bignum_equal(p.b, b);
	}

	if (isdouble(p))
		return p.d == a / b;

	return 0;
}
