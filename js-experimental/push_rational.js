function
push_rational(a, b)
{
	var sign;

	if (a < 0)
		sign = -1;
	else
		sign = 1;

	a = bignum_int(Math.abs(a));
	b = bignum_int(b);

	push_rational_number(sign, a, b);
}
