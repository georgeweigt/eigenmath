function
pop_integer()
{
	var n, p;

	p = pop();

	if (isinteger(p)) {
		n = bignum_smallnum(p.a);
		if (n == null)
			stopf("bignum exceeds 4294967295");
		if (isnegativenumber(p))
			n = -n;
		return n;
	}

	if (isdouble(p) && Math.floor(p.d) == p.d)
		return p.d;

	stopf("integer expected");
}
