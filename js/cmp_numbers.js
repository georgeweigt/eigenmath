function
cmp_numbers(p1, p2)
{
	var d1, d2;

	if (isrational(p1) && isrational(p2))
		return cmp_rationals(p1, p2);

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	if (d1 < d2)
		return -1;

	if (d1 > d2)
		return 1;

	return 0;
}

function
cmp_rationals(p1, p2)
{
	var a, b;

	if (isnegativenumber(p1) && !isnegativenumber(p2))
		return -1;

	if (!isnegativenumber(p1) && isnegativenumber(p2))
		return 1;

	if (isinteger(p1) && isinteger(p2))
		if (isnegativenumber(p1))
			return bignum_cmp(p2.a, p1.a);
		else
			return bignum_cmp(p1.a, p2.a);

	a = bignum_mul(p1.a, p2.b);
	b = bignum_mul(p1.b, p2.a);

	if (isnegativenumber(p1))
		return bignum_cmp(b, a);
	else
		return bignum_cmp(a, b);
}
