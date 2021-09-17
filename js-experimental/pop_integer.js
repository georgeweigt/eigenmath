function
pop_integer()
{
	var p1 = pop();

	if (isinteger(p1)) {
		if (p1.a.length == 1)
			return p1.sign * p1.a[0];
		if (p1.a.length == 2 && p1.a[1] < 256)
			return p1.sign * BIGM * p1.a[1] + p1.a[0];
		stopf("bignum exceeds 2^32");
	}

	if (isdouble(p1) && Math.floor(p1.d) == p1.d)
		return p1.d;

	stopf("integer expected");
}
