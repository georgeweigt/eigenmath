function
pop_integer()
{
	var n, p;

	p = pop();

	if (isinteger(p)) {
		n = bignum_smallnum(p.a);
		if (n == null)
			stopf("value exceeds 2^31 - 1");
		if (isnegativenumber(p))
			n = -n;
		return n;
	}

	if (isdouble(p) && Math.floor(p.d) == p.d)
		return p.d;

	stopf("integer expected");
}
