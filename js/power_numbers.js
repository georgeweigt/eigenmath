// BASE and EXPO are numbers

function
power_numbers(BASE, EXPO)
{
	var base, expo;

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

	if (isdouble(BASE) || isdouble(EXPO)) {
		power_double(BASE, EXPO);
		return;
	}



	if (iszero(BASE) && isnegativenumber(EXPO))
		stopf("divide by zero");

	if (isinteger(EXPO)) {
		power_simple(BASE, EXPO);
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
		power_numbers(BASE, EXPO);
		multiply();
		return;
	}

	if (isrational(BASE) && isrational(EXPO)) {
		power_rationals(BASE, EXPO);
		return;
	}

	push(BASE);
	base = pop_double();

	push(EXPO);
	expo = pop_double();

	push_double(Math.pow(base, expo));
}

function
power_rationals(BASE, EXPO)
{
	var d, h, i, n, base, expo, p1, COEFF;

	h = stack.length;

	// put factors on stack

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);

	factor();

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

	// float radicals if COEFF is double (can happen due to auto conversion of rational to double)

	n = stack.length;

	if (isdouble(COEFF) && n - h > 0) {
		d = COEFF.d;
		n = stack.length;
		for (i = h; i < n; i++) {
			p1 = stack[i];
			BASE = cadr(p1);
			EXPO = caddr(p1);
			base = BASE.a / BASE.b;
			expo = EXPO.a / EXPO.b;
			d *= Math.pow(base, expo);
		}
		stack.splice(h); // pop all
		push_double(d);
		COEFF = pop();
	}

	// finalize

	n = stack.length - h;

	if (n == 0 || !isplusone(COEFF) || isdouble(COEFF)) {
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

function
power_rationals_nib(BASE, EXPO) // BASE is prime number, EXPO is rational
{
	var n, q, r;

	// integer power?

	if (EXPO.b == 1) {

		n = Math.pow(BASE.a, Math.abs(EXPO.a));

		if (EXPO.a < 0)
			push_rational(1, n); // reciprocate
		else
			push_rational(n, 1);

		return;
	}

	q = Math.floor(Math.abs(EXPO.a) / EXPO.b);
	r = Math.abs(EXPO.a) % EXPO.b;

	// whole part

	if (q > 0) {

		n = Math.pow(BASE.a, q);

		if (EXPO.a < 0)
			push_rational(1, n); // reciprocate
		else
			push_rational(n, 1);
	}

	// remainder (BASE is prime hence no roots)

	if (EXPO.a < 0)
		r = -r;

	push_symbol(POWER);
	push(BASE);
	push_rational(r, EXPO.b);
	list(3);
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

	if (base == -1.0)
		return; // FIXME this is needed so we get i instead of 1 i

	d = Math.pow(-base, expo);
	push_double(d);

	multiply();
}
