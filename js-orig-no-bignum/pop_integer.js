function
pop_integer()
{
	var n, p1;

	p1 = pop();

	if (isinteger(p1)) {
		n = bignum_smallnum(p1.a);
		if (n == null)
			stopf("bignum exceeds 4294967295");
		return p1.sign * n;
	}

	if (isdouble(p1) && Math.floor(p1.d) == p1.d)
		return p1.d;

	stopf("integer expected");
}
