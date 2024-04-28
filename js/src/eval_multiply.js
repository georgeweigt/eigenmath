function
eval_multiply(p1)
{
	var h = stack.length;
	expanding--; // undo expanding++ in evalf
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalf();
		p1 = cdr(p1);
	}
	multiply_factors(stack.length - h);
	expanding++;
}

function
multiply()
{
	multiply_factors(2);
}

function
multiply_factors(n)
{
	var h, T;

	if (n < 2)
		return;

	h = stack.length - n;

	flatten_factors(h);

	T = multiply_tensor_factors(h);

	multiply_scalar_factors(h);

	if (istensor(T)) {
		push(T);
		inner();
	}
}

function
flatten_factors(h)
{
	var i, n, p1;
	n = stack.length;
	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (car(p1) == symbol(MULTIPLY)) {
			stack[i] = cadr(p1);
			p1 = cddr(p1);
			while (iscons(p1)) {
				push(car(p1));
				p1 = cdr(p1);
			}
		}
	}
}

function
multiply_tensor_factors(h)
{
	var i, p1, T;
	T = symbol(NIL);
	for (i = h; i < stack.length; i++) {
		p1 = stack[i];
		if (!istensor(p1))
			continue;
		if (istensor(T)) {
			push(T);
			push(p1);
			hadamard();
			T = pop();
		} else
			T = p1;
		stack.splice(i, 1); // remove factor
		i--; // use same index again
	}
	return T;
}

function
multiply_scalar_factors(h)
{
	var n, COEF;

	COEF = combine_numerical_factors(h, one);

	if (iszero(COEF) || h == stack.length) {
		stack.length = h; // pop all
		push(COEF);
		return;
	}

	combine_factors(h);
	normalize_power_factors(h);

	// do again in case exp(1/2 i pi) changed to i

	combine_factors(h);
	normalize_power_factors(h);

	COEF = combine_numerical_factors(h, COEF);

	if (iszero(COEF) || h == stack.length) {
		stack.length = h; // pop all
		push(COEF);
		return;
	}

	COEF = reduce_radical_factors(h, COEF);

	if (isdouble(COEF) || !isplusone(COEF))
		push(COEF);

	if (expanding)
		expand_sum_factors(h); // success leaves one expr on stack

	n = stack.length - h;

	switch (n) {
	case 0:
		push_integer(1); // all factors canceled
		break;
	case 1:
		break;
	default:
		sort_factors(n); // previously sorted provisionally
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // prepend MULTIPLY to list
		break;
	}
}

function
combine_numerical_factors(h, COEF)
{
	var i, p1;
	for (i = h; i < stack.length; i++) {
		p1 = stack[i];
		if (isnum(p1)) {
			multiply_numbers(COEF, p1);
			COEF = pop();
			stack.splice(i, 1); // remove factor
			i--; // use same index again
		}
	}
	return COEF;
}

// factors that have the same base are combined by adding exponents

function
combine_factors(h)
{
	var i;
	sort_factors_provisional(h);
	for (i = h; i < stack.length - 1; i++) {
		if (combine_factors_nib(i, i + 1)) {
			stack.splice(i + 1, 1); // remove factor
			i--; // use same index again
		}
	}
}

function
combine_factors_nib(i, j)
{
	var p1, p2, BASE1, EXPO1, BASE2, EXPO2;

	p1 = stack[i];
	p2 = stack[j];

	if (car(p1) == symbol(POWER)) {
		BASE1 = cadr(p1);
		EXPO1 = caddr(p1);
	} else {
		BASE1 = p1;
		EXPO1 = one;
	}

	if (car(p2) == symbol(POWER)) {
		BASE2 = cadr(p2);
		EXPO2 = caddr(p2);
	} else {
		BASE2 = p2;
		EXPO2 = one;
	}

	if (!equal(BASE1, BASE2))
		return 0;

	if (isdouble(BASE2))
		BASE1 = BASE2; // if mixed rational and double, use double

	push_symbol(POWER);
	push(BASE1);
	push(EXPO1);
	push(EXPO2);
	add();
	list(3);

	stack[i] = pop();

	return 1;
}

function
sort_factors_provisional(h)
{
	var t = stack.splice(h).sort(cmp_factors_provisional);
	stack = stack.concat(t);
}

function
cmp_factors_provisional(p1, p2)
{
	if (car(p1) == symbol(POWER))
		p1 = cadr(p1); // p1 = base

	if (car(p2) == symbol(POWER))
		p2 = cadr(p2); // p2 = base

	return cmp(p1, p2);
}

function
normalize_power_factors(h)
{
	var i, k, p1;
	k = stack.length;
	for (i = h; i < k; i++) {
		p1 = stack[i];
		if (car(p1) == symbol(POWER)) {
			push(cadr(p1));
			push(caddr(p1));
			power();
			p1 = pop();
			if (car(p1) == symbol(MULTIPLY)) {
				stack[i] = cadr(p1);
				p1 = cddr(p1);
				while (iscons(p1)) {
					push(car(p1));
					p1 = cdr(p1);
				}
			} else
				stack[i] = p1;
		}
	}
}

function
expand_sum_factors(h)
{
	var i, n, p1, p2;

	if (stack.length - h < 2)
		return;

	// search for a sum factor

	for (i = h; i < stack.length; i++) {
		p2 = stack[i];
		if (car(p2) == symbol(ADD))
			break;
	}

	if (i == stack.length)
		return; // no sum factors

	// remove the sum factor

	stack.splice(i, 1);

	n = stack.length - h;

	if (n > 1) {
		sort_factors(n);
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // prepend MULTIPLY to list
	}

	p1 = pop(); // p1 is the multiplier

	p2 = cdr(p2); // p2 is the sum

	while (iscons(p2)) {
		push(p1);
		push(car(p2));
		multiply();
		p2 = cdr(p2);
	}

	add_terms(stack.length - h);
}

function
sort_factors(n)
{
	var t = stack.splice(stack.length - n).sort(cmp_factors);
	stack = stack.concat(t);
}

function
cmp_factors(p1, p2)
{
	var a, b, c;
	var base1, base2, expo1, expo2;

	a = order_factor(p1);
	b = order_factor(p2);

	if (a < b)
		return -1;

	if (a > b)
		return 1;

	if (car(p1) == symbol(POWER)) {
		base1 = cadr(p1);
		expo1 = caddr(p1);
	} else {
		base1 = p1;
		expo1 = one;
	}

	if (car(p2) == symbol(POWER)) {
		base2 = cadr(p2);
		expo2 = caddr(p2);
	} else {
		base2 = p2;
		expo2 = one;
	}

	c = cmp(base1, base2);

	if (c == 0)
		c = cmp(expo2, expo1); // swapped to reverse sort order

	return c;
}

//  1	number
//  2	number to power (root)
//  3	-1 to power (imaginary)
//  4	other factor (symbol, power, func, etc)
//  5	exponential
//  6	derivative

function
order_factor(p)
{
	if (isnum(p))
		return 1;

	if (p == symbol(EXP1))
		return 5;

	if (car(p) == symbol(DERIVATIVE) || car(p) == symbol(D_LOWER))
		return 6;

	if (car(p) == symbol(POWER)) {

		p = cadr(p); // p = base

		if (isminusone(p))
			return 3;

		if (isnum(p))
			return 2;

		if (p == symbol(EXP1))
			return 5;

		if (car(p) == symbol(DERIVATIVE) || car(p) == symbol(D_LOWER))
			return 6;
	}

	return 4;
}

function
multiply_numbers(p1, p2)
{
	var d1, d2;

	if (isrational(p1) && isrational(p2)) {
		multiply_rationals(p1, p2);
		return;
	}

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	push_double(d1 * d2);
}

function
multiply_rationals(p1, p2)
{
	var a, b, d, sign;

	if (iszero(p1) || iszero(p2)) {
		push_integer(0);
		return;
	}

	if (p1.sign == p2.sign)
		sign = 1;
	else
		sign = -1;

	if (isinteger(p1) && isinteger(p2)) {
		a = bignum_mul(p1.a, p2.a);
		b = bignum_int(1);
		push_bignum(sign, a, b);
		return;
	}

	a = bignum_mul(p1.a, p2.a);
	b = bignum_mul(p1.b, p2.b);

	d = bignum_gcd(a, b);

	a = bignum_div(a, d);
	b = bignum_div(b, d);

	push_bignum(sign, a, b);
}

// for example, 2 / sqrt(2) -> sqrt(2)

function
reduce_radical_factors(h, COEF)
{
	if (!any_radical_factors(h))
		return COEF;

	if (isrational(COEF))
		return reduce_radical_rational(h, COEF);
	else
		return reduce_radical_double(h, COEF);
}

function
any_radical_factors(h)
{
	var i;
	for (i = h; i < stack.length; i++)
		if (isradical(stack[i]))
			return 1;
	return 0;
}

function
reduce_radical_double(h, COEF)
{
	var a, b, c, i, p1;

	c = COEF.d;

	for (i = h; i < stack.length; i++) {

		p1 = stack[i];

		if (isradical(p1)) {

			push(cadr(p1)); // base
			a = pop_double();

			push(caddr(p1)); // exponent
			b = pop_double();

			c = c * Math.pow(a, b); // a > 0 by isradical above

			stack.splice(i, 1); // remove factor

			i--; // use same index again
		}
	}

	push_double(c);
	COEF = pop();

	return COEF;
}

function
reduce_radical_rational(h, COEF)
{
	var i, k, p1, p2, NUMER, DENOM, BASE, EXPO;

	if (isplusone(COEF) || isminusone(COEF))
		return COEF; // COEF has no factors, no cancellation is possible

	push(COEF);
	absfunc();
	p1 = pop();

	push(p1);
	numerator();
	NUMER = pop();

	push(p1);
	denominator();
	DENOM = pop();

	k = 0;

	for (i = h; i < stack.length; i++) {
		p1 = stack[i];
		if (!isradical(p1))
			continue;
		BASE = cadr(p1);
		EXPO = caddr(p1);
		if (isnegativenumber(EXPO)) {
			mod_integers(NUMER, BASE);
			p2 = pop();
			if (iszero(p2)) {
				push(NUMER);
				push(BASE);
				divide();
				NUMER = pop();
				push_symbol(POWER);
				push(BASE);
				push_integer(1);
				push(EXPO);
				add();
				list(3);
				stack[i] = pop();
				k++;
			}
		} else {
			mod_integers(DENOM, BASE);
			p2 = pop();
			if (iszero(p2)) {
				push(DENOM);
				push(BASE);
				divide();
				DENOM = pop();
				push_symbol(POWER);
				push(BASE);
				push_integer(-1);
				push(EXPO);
				add();
				list(3);
				stack[i] = pop();
				k++;
			}
		}
	}

	if (k) {
		push(NUMER);
		push(DENOM);
		divide();
		if (isnegativenumber(COEF))
			negate();
		COEF = pop();
	}

	return COEF;
}

function
multiply_expand()
{
	expanding++;
	multiply();
	expanding--;
}

function
multiply_noexpand()
{
	var t;
	t = expanding;
	expanding = 0;
	multiply();
	expanding = t;
}

function
negate()
{
	push_integer(-1);
	multiply();
}

function
reciprocate()
{
	push_integer(-1);
	power();
}

function
divide()
{
	reciprocate();
	multiply();
}
