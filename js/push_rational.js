function
push_rational(a, b)
{
	var sign;

	if (a < 0)
		sign = -1;
	else
		sign = 1;

	a = Math.abs(a);

	a = bignum_int(a);
	b = bignum_int(b);

	push_bignum(sign, a, b);
}
