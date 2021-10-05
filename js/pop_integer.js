function
pop_integer()
{
	var n, p;

	p = pop();

	if (!issmallinteger(p))
		stopf("small integer expected");

	if (isrational(p)) {
		n = bignum_smallnum(p.a);
		if (isnegativenumber(p))
			n = -n;
	} else
		n = p.d;

	return n;
}
