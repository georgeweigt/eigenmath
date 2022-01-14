// BASE and EXPO are numbers

function
power_numbers(BASE, EXPO)
{
	var a, b, h, i, n, p1, p2;

	// n^0

	if (iszero(EXPO)) {
		push_integer(1);
		return;
	}

	// 0^n

	if (iszero(BASE)) {
		if (isnegativenumber(EXPO))
			stopf("divide by zero");
		push_integer(0);
		return;
	}

	// 1^n

	if (isplusone(BASE)) {
		push_integer(1);
		return;
	}

	// n^1

	if (isplusone(EXPO)) {
		push(BASE);
		return;
	}

	if (isdouble(BASE) || isdouble(EXPO)) {
		power_double(BASE, EXPO);
		return;
	}

	// integer exponent?

	if (isinteger(EXPO)) {
		a = bignum_pow(BASE.a, EXPO.a);
		b = bignum_pow(BASE.b, EXPO.a);
		if (isnegativenumber(BASE) && bignum_odd(EXPO.a))
			if (isnegativenumber(EXPO))
				push_bignum(-1, b, a); // reciprocate
			else
				push_bignum(-1, a, b);
		else
			if (isnegativenumber(EXPO))
				push_bignum(1, b, a); // reciprocate
			else
				push_bignum(1, a, b);
		return;
	}

	// exponent is a root

	h = stack.length;

	// put factors on stack

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);

	factor();

	// normalize factors

	n = stack.length - h; // fix n now, stack can grow

	for (i = 0; i < n; i++) {
		p1 = stack[h + i];
		if (car(p1) == symbol(POWER)) {
			BASE = cadr(p1);
			EXPO = caddr(p1);
			power_numbers_factor(BASE, EXPO);
			stack[h + i] = pop(); // fill hole
		}
	}

	// combine numbers (leaves radicals on stack)

	p1 = one;

	for (i = h; i < stack.length; i++) {
		p2 = stack[i];
		if (isnum(p2)) {
			push(p1);
			push(p2);
			multiply();
			p1 = pop();
			stack.splice(i, 1);
			i--;
		}
	}

	// finalize

	n = stack.length - h;

	if (n == 0 || !isplusone(p1)) {
		push(p1);
		n++;
	}

	if (n == 1)
		return;

	sort_factors(h);
	list(n);
	push_symbol(MULTIPLY);
	swap();
	cons();
}

// BASE is an integer factor

function
power_numbers_factor(BASE, EXPO)
{
	var a, b, n, q, r, p1;

	if (isminusone(BASE)) {
		power_minusone(EXPO);
		p1 = pop();
		if (car(p1) == symbol(MULTIPLY)) {
			push(cadr(p1));
			push(caddr(p1));
		} else
			push(p1);
		return;
	}

	if (isinteger(EXPO)) {

		a = bignum_pow(BASE.a, EXPO.a);
		b = bignum_int(1);

		if (isnegativenumber(EXPO))
			push_bignum(1, b, a); // reciprocate
		else
			push_bignum(1, a, b);

		return;
	}

	// EXPO.a          r
	// ------ == q + ------
	// EXPO.b        EXPO.b

	q = bignum_div(EXPO.a, EXPO.b);
	r = bignum_mod(EXPO.a, EXPO.b);

	// process q

	if (!bignum_iszero(q)) {

		a = bignum_pow(BASE.a, q);
		b = bignum_int(1);

		if (isnegativenumber(EXPO))
			push_bignum(1, b, a); // reciprocate
		else
			push_bignum(1, a, b);
	}

	// process r

	n = bignum_smallnum(BASE.a);

	if (n != null) {
		// BASE is 32 bits or less, hence BASE is a prime number, no root
		push_symbol(POWER);
		push(BASE);
		push_bignum(EXPO.sign, r, bignum_copy(EXPO.b));
		list(3);
		return;
	}

	// BASE was too big to factor, try finding root

	n = bignum_root(BASE.a, EXPO.b);

	if (n == null) {
		// no root
		push_symbol(POWER);
		push(BASE);
		push_bignum(EXPO.sign, r, bignum_copy(EXPO.b));
		list(3);
		return;
	}

	// raise n to rth power

	n = bignum_pow(n, r);

	if (isnegativenumber(EXPO))
		push_bignum(1, bignum_int(1), n); // reciprocate
	else
		push_bignum(1, n, bignum_int(1));
}

function
power_double(BASE, EXPO)
{
	var base, d, expo;

	push(BASE);
	base = pop_double();

	push(EXPO);
	expo = pop_double();

	if (base > 0 || expo == Math.floor(expo)) {
		d = Math.pow(base, expo);
		push_double(d);
		return;
	}

	// BASE is negative and EXPO is fractional

	power_minusone(EXPO);

	if (base == -1)
		return;

	d = Math.pow(-base, expo);
	push_double(d);

	multiply();
}
