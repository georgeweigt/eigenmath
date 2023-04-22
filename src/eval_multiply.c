void
eval_multiply(struct atom *p1)
{
	int h = tos;
	expanding--; // undo expanding++ in evalf
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalg();
		p1 = cdr(p1);
	}
	multiply_factors(tos - h);
	expanding++;
}

void
multiply(void)
{
	multiply_factors(2);
}

void
multiply_factors(int n)
{
	int h;
	struct atom *T;

	if (n < 2)
		return;

	h = tos - n;

	flatten_factors(h);

	T = multiply_tensor_factors(h);

	multiply_scalar_factors(h);

	if (istensor(T)) {
		push(T);
		inner();
	}
}

void
flatten_factors(int h)
{
	int i, n;
	struct atom *p1;
	n = tos;
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

struct atom *
multiply_tensor_factors(int h)
{
	int i, j;
	struct atom *p1, *T;
	T = symbol(NIL);
	for (i = h; i < tos; i++) {
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
		// remove the factor
		for (j = i + 1; j < tos; j++)
			stack[j - 1] = stack[j];
		i--;
		tos--;
	}
	return T;
}

void
multiply_scalar_factors(int h)
{
	int n;
	struct atom *COEF;

	COEF = one;

	COEF = combine_numerical_factors(h, COEF);

	if (iszero(COEF) || h == tos) {
		tos = h;
		push(COEF);
		return;
	}

	combine_factors(h);
	normalize_power_factors(h);

	// do again in case exp(1/2 i pi) changed to i

	combine_factors(h);
	normalize_power_factors(h);

	COEF = combine_numerical_factors(h, COEF);

	if (iszero(COEF) || h == tos) {
		tos = h;
		push(COEF);
		return;
	}

	COEF = reduce_radical_factors(h, COEF);

	if (isdouble(COEF) || !isplusone(COEF))
		push(COEF);

	if (expanding)
		expand_sum_factors(h); // success leaves one expr on stack

	n = tos - h;

	switch (n) {
	case 0:
		push_integer(1); // all factors canceled
		break;
	case 1:
		break;
	default:
		sort_factors(n);
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // prepend MULTIPLY to list
		break;
	}
}

struct atom *
combine_numerical_factors(int h, struct atom *COEF)
{
	int i, j;
	struct atom *p1;
	for (i = h; i < tos; i++) {
		p1 = stack[i];
		if (isnum(p1)) {
			multiply_numbers(COEF, p1);
			COEF = pop();
			// remove the factor
			for (j = i + 1; j < tos; j++)
				stack[j - 1] = stack[j];
			i--;
			tos--;
		}
	}
	return COEF;
}

// factors that have the same base are combined by adding exponents

void
combine_factors(int h)
{
	int i, j;
	sort_factors_provisional(h);
	for (i = h; i < tos - 1; i++) {
		if (combine_factors_nib(i, i + 1)) {
			// remove the factor
			for (j = i + 2; j < tos; j++)
				stack[j - 1] = stack[j];
			i--;
			tos--;
		}
	}
}

int
combine_factors_nib(int i, int j)
{
	struct atom *p1, *p2, *BASE1, *EXPO1, *BASE2, *EXPO2;

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

void
sort_factors_provisional(int h)
{
	qsort(stack + h, tos - h, sizeof (struct atom *), sort_factors_provisional_func);
}

int
sort_factors_provisional_func(const void *q1, const void *q2)
{
	return cmp_factors_provisional(*((struct atom **) q1), *((struct atom **) q2));
}

int
cmp_factors_provisional(struct atom *p1, struct atom *p2)
{
	if (car(p1) == symbol(POWER))
		p1 = cadr(p1); // p1 = base

	if (car(p2) == symbol(POWER))
		p2 = cadr(p2); // p2 = base

	return cmp(p1, p2);
}

void
normalize_power_factors(int h)
{
	int i, k;
	struct atom *p1;
	k = tos;
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

void
expand_sum_factors(int h)
{
	int i, j, n = tos - h;
	struct atom **s = stack + h, *p1, *p2;

	p2 = symbol(NIL); // silence compiler

	if (n < 2)
		return;

	// search for a sum factor

	for (i = 0; i < n; i++) {
		p2 = s[i];
		if (car(p2) == symbol(ADD))
			break;
	}

	if (i == n)
		return; // no sum factors

	// remove the sum factor

	for (j = i + 1; j < n; j++)
		s[j - 1] = s[j];

	n--;
	tos--;

	if (n > 1) {
		sort_factors(n);
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // prepend MULTIPLY to list
	}

	p1 = pop(); // p1 is the multiplier

	p2 = cdr(p2); // p2 is the sum factor

	while (iscons(p2)) {
		push(p1);
		push(car(p2));
		multiply();
		p2 = cdr(p2);
	}

	add_terms(tos - h);
}

void
sort_factors(int n)
{
	qsort(stack + tos - n, n, sizeof (struct atom *), sort_factors_func);
}

int
sort_factors_func(const void *q1, const void *q2)
{
	return cmp_factors(*((struct atom **) q1), *((struct atom **) q2));
}

int
cmp_factors(struct atom *p1, struct atom *p2)
{
	int a, b, c;
	struct atom *base1, *base2, *expo1, *expo2;

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

int
order_factor(struct atom *p)
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

void
multiply_numbers(struct atom *p1, struct atom *p2)
{
	double d1, d2;

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

void
multiply_rationals(struct atom *p1, struct atom *p2)
{
	int sign;
	uint32_t *a, *b, *c;

	if (iszero(p1) || iszero(p2)) {
		push_integer(0);
		return;
	}

	if (p1->sign == p2->sign)
		sign = MPLUS;
	else
		sign = MMINUS;

	if (isinteger(p1) && isinteger(p2)) {
		push_bignum(sign, mmul(p1->u.q.a, p2->u.q.a), mint(1));
		return;
	}

	a = mmul(p1->u.q.a, p2->u.q.a);
	b = mmul(p1->u.q.b, p2->u.q.b);
	c = mgcd(a, b);
	push_bignum(sign, mdiv(a, c), mdiv(b, c));

	mfree(a);
	mfree(b);
	mfree(c);
}

// for example, 2 / sqrt(2) -> sqrt(2)

struct atom *
reduce_radical_factors(int h, struct atom *COEF)
{
	if (!any_radical_factors(h))
		return COEF;

	if (isrational(COEF))
		return reduce_radical_rational(h, COEF);
	else
		return reduce_radical_double(h, COEF);
}

int
any_radical_factors(int h)
{
	int i, n;
	n = tos;
	for (i = h; i < n; i++)
		if (isradical(stack[i]))
			return 1;
	return 0;
}

struct atom *
reduce_radical_double(int h, struct atom *COEF)
{
	int i, j;
	double a, b, c;
	struct atom *p1;

	c = COEF->u.d;

	for (i = h; i < tos; i++) {

		p1 = stack[i];

		if (isradical(p1)) {

			push(cadr(p1)); // base
			a = pop_double();

			push(caddr(p1)); // exponent
			b = pop_double();

			c = c * pow(a, b); // a > 0 by isradical above

			// remove the factor

			for (j = i + 1; j < tos; j++)
				stack[j - 1] = stack[j];

			i--; // use same index again
			tos--;
		}
	}

	push_double(c);
	COEF = pop();

	return COEF;
}

struct atom *
reduce_radical_rational(int h, struct atom *COEF)
{
	int i, k;
	struct atom *p1, *p2, *NUMER, *DENOM, *BASE, *EXPO;

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

	for (i = h; i < tos; i++) {
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

void
multiply_expand(void)
{
	expanding++;
	multiply();
	expanding--;
}

void
multiply_noexpand(void)
{
	int t;
	t = expanding;
	expanding = 0;
	multiply();
	expanding = t;
}

void
multiply_factors_noexpand(int n)
{
	int t;
	t = expanding;
	expanding = 0;
	multiply_factors(n);
	expanding = t;
}

void
negate(void)
{
	push_integer(-1);
	multiply();
}

void
negate_noexpand(void)
{
	int t;
	t = expanding;
	expanding = 0;
	negate();
	expanding = t;
}

void
reciprocate(void)
{
	push_integer(-1);
	power();
}

void
divide(void)
{
	reciprocate();
	multiply();
}
