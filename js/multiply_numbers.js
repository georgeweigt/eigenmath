function
multiply_numbers(p1, p2)
{
	var a, b;

	if (isrational(p1) && isrational(p2)) {
		multiply_rationals(p1, p2);
		return;
	}

	push(p1);
	a = pop_double();

	push(p2);
	b = pop_double();

	push_double(a * b);
}

function
multiply_rationals(p1, p2)
{
	var a, b, d, sign;

	if (isinteger(p1) && isinteger(p2)) {
		multiply_integers(p1, p2);
		return;
	}

	if (p1.sign == p2.sign)
		sign = 1;
	else
		sign = -1;

	a = bignum_mul(p1.a, p2.a);
	b = bignum_mul(p1.b, p2.b);

	d = bignum_gcd(a, b);

	a = bignum_div(a, d);
	b = bignum_div(b, d);

	push_bignum(sign, a, b);
}

function
multiply_integers(p1, p2)
{
	var a, b, sign;

	if (p1.sign == p2.sign)
		sign = 1;
	else
		sign = -1;

	a = bignum_mul(p1.a, p2.a);
	b = bignum_int(1);

	push_bignum(sign, a, b);
}
