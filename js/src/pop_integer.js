function
pop_integer()
{
	var n, p;
	p = pop();
	if (!isnum(p))
		stopf("argument is not a number");
	push(p);
	floorfunc();
	p = pop();
	if (!issmallinteger(p))
		stopf("integer overflow");
	if (isrational(p)) {
		n = bignum_smallnum(p.a);
		if (isnegativenumber(p))
			n = -n;
	} else
		n = p.d;
	return n;
}
