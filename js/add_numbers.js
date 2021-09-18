function
add_numbers(p1, p2)
{
	var a, b;

	if (isrational(p1) && isrational(p2)) {
		add_rationals(p1, p2);
		return;
	}

	push(p1);
	a = pop_double();

	push(p2);
	b = pop_double();

	push_double(a + b);
}

function
add_rationals(p1, p2)
{
	var a, ab, b, ba, d, sign;

	if (isinteger(p1) && isinteger(p2)) {
		add_integers(p1, p2);
		return;
	}

	ab = bignum_mul(p1.a, p2.b);
	ba = bignum_mul(p1.b, p2.a);

	if (p1.sign == p2.sign) {
		a = bignum_add(ab, ba);
		sign = p1.sign;
	} else {
		switch (bignum_cmp(ab, ba)) {
		case 1:
			a = bignum_sub(ab, ba);
			sign = p1.sign;
			break;
		case 0:
			push_integer(0);
			return;
		case -1:
			a = bignum_sub(ba, ab);
			sign = p2.sign;
			break;
		}
	}

	b = bignum_mul(p1.b, p2.b);

	d = bignum_gcd(a, b);

	a = bignum_div(a, d);
	b = bignum_div(b, d);

	push_bignum(sign, a, b);
}

function
add_integers(p1, p2)
{
	var a, b, sign;

	if (p1.sign == p2.sign) {
		a = bignum_add(p1.a, p2.a);
		sign = p1.sign;
	} else {
		switch (bignum_cmp(p1.a, p2.a)) {
		case 1:
			a = bignum_sub(p1.a, p2.a);
			sign = p1.sign;
			break;
		case 0:
			push_integer(0);
			return;
		case -1:
			a = bignum_sub(p2.a, p1.a);
			sign = p2.sign;
			break;
		}
	}

	b = bignum_int(1);

	push_bignum(sign, a, b);
}
