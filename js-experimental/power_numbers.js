// BASE and EXPO are numbers

function
power_numbers(BASE, EXPO)
{
	var a, b;

	if (iszero(EXPO)) {
		push_integer(1); // 0^0 = 1
		return;
	}

	if (iszero(BASE)) {
		if (isnegativenumber(EXPO))
			stopf("divide by zero in power_numbers");
		push_integer(0);
		return;
	}

	if (isplusone(BASE)) {
		push_integer(1);
		return;
	}

	if (isplusone(EXPO)) {
		push(BASE);
		return;
	}

	if (isrational(BASE) && isinteger(EXPO)) {
		a = bignum_pow(BASE.a, EXPO.a);
		b = bignum_pow(BASE.b, EXPO.a);
		if (isnegativenumber(BASE) && bignum_odd(EXPO.a))
			if (isnegativenumber(EXPO))
				push_rational_number(-1, b, a); // reciprocate
			else
				push_rational_number(-1, a, b);
		else
			if (isnegativenumber(EXPO))
				push_rational_number(1, b, a); // reciprocate
			else
				push_rational_number(1, a, b);
		return;
	}

	if (isdouble(BASE) && isinteger(EXPO)) {
		push(BASE);
		a = pop_double();
		push(EXPO);
		b = pop_double();
		push_double(Math.pow(a, b);
		return;
	}

	if (isminusone(BASE)) {
		power_minusone(EXPO);
		return;
	}

	if (isnegativenumber(BASE)) {
		power_minusone(EXPO);
		push(BASE);
		negate();
		BASE = pop();
		power_numbers_nib(BASE, EXPO);
		multiply();
		return;
	}

	power_numbers_nib(BASE, EXPO);
}

// BASE is nonnegative

function
power_numbers_nib(BASE, EXPO)
{
	var d;

	if (isrational(BASE) && isrational(EXPO)) {
		power_rationals(BASE, EXPO);
		return;
	}

	push(BASE);
	BASE = pop_double();

	push(EXPO);
	EXPO = pop_double();

	d = Math.pow(BASE, EXPO);

	if (!isFinite(d))
		stopf("floating point nan or infinity");

	push_double(d);
}

// BASE is nonnegative

function
power_rationals(BASE, EXPO)
{
	var h, i, n, p1, COEFF;

	h = stack.length;

	// put factors on stack

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);

	factor(); // factor detects POWER and applies EXPO to factors of BASE

	// normalize factors

	n = stack.length;

	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (car(p1) == symbol(POWER)) {
			BASE = cadr(p1);
			EXPO = caddr(p1);
			power_rationals_nib(BASE, EXPO);
			stack[i] = pop(); // 1 or 2 factors on stack, fill hole with tos
		}
	}

	// combine numbers (leaves radicals on stack)

	COEFF = one;

	n = stack.length;

	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (isnum(p1)) {
			push(COEFF);
			push(p1);
			multiply();
			COEFF = pop();
			stack.splice(i, 1);
			i--;
			n--;
		}
	}

	// finalize

	n = stack.length - h;

	if (n == 0 || !isplusone(COEFF) /* || isdouble(COEFF) */) { //FIXME
		push(COEFF);
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

// BASE is nonnegative integer, EXPO is signed rational

function
power_rationals_nib(BASE, EXPO)
{
	var a, b, n, q, r;

	// integer power?

	if (isinteger(EXPO)) {

		a = bignum_pow(BASE.a, EXPO.a);
		b = bignum_int(1);

		if (isnegativenumber(EXPO))
			push_rational_number(1, b, a); // reciprocate
		else
			push_rational_number(1, a, b);

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
			push_rational_number(1, b, a); // reciprocate
		else
			push_rational_number(1, a, b);
	}

	// process r

	if (BASE.a.length == 1 || (BASE.a.length == 2 && BASE.a[1] < 256)) {
		// BASE is 32 bits or less, hence a prime number, no root
		push_symbol(POWER);
		push(BASE);
		push_rational_number(EXPO.sign, r, bignum_copy(EXPO.b));
		list(3);
		return;
	}

	// BASE was too big to factor, try finding root

	n = bignum_root(BASE.a, EXPO.b);

	if (n == null) {
		// no root
		push_symbol(POWER);
		push(BASE);
		push_rational_number(EXPO.sign, r, bignum_copy(EXPO.b));
		list(3);
		return;
	}

	// raise root to rth power

	n = bignum_pow(n, r);

	if (isnegativenumber(EXPO))
		push_rational_number(1, bignum_int(1), n); // reciprocate
	else
		push_rational_number(1, n, bignum_int(1));
}
