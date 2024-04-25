function
isequalq(p, a, b)
{
	var sign;
	if (isrational(p)) {
		if (a < 0) {
			sign = -1;
			a = -a;
		} else
			sign = 1;
		return p.sign == sign && bignum_equal(p.a, a) && bignum_equal(p.b, b);
	}
	if (isdouble(p))
		return p.d == a / b;
	return 0;
}
