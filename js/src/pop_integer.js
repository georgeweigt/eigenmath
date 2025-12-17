function
pop_integer()
{
	var a, d, n, p;
	p = pop();
	if (!isnum(p))
		stopf("number expected, argument is not a number");
	if (isdouble(p)) {
		d = Math.trunc(p.d);
		if (Math.abs(d) > 0x7fffffff)
			stopf("integer overflow");
		n = d;
	} else {
		if (isfraction(p))
			a = bignum_div(p.a, p.b);
		else
			a = p.a;
		if (!bignum_issmallnum(a))
			stopf("integer overflow");
		n = bignum_smallnum(a);
		if (n > 0 && p.sign == -1)
			n = -n;
	}
	return n;
}
