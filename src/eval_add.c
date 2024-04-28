void
eval_add(struct atom *p1)
{
	int h = tos;
	expanding--; // undo expanding++ in evalf
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		evalg();
		p1 = cdr(p1);
	}
	add_terms(tos - h);
	expanding++;
}

void
add(void)
{
	add_terms(2);
}

void
add_terms(int n)
{
	int h, i;
	struct atom *p1, *T;

	if (n < 2)
		return;

	h = tos - n;

	flatten_terms(h);

	T = combine_tensors(h);

	combine_terms(h);

	if (simplify_terms(h))
		combine_terms(h);

	n = tos - h;

	if (n == 0) {
		if (istensor(T))
			push(T);
		else
			push_integer(0);
		return;
	}

	if (n > 1) {
		list(n);
		push_symbol(ADD);
		swap();
		cons(); // prepend ADD to list
	}

	if (!istensor(T))
		return;

	p1 = pop();

	T = copy_tensor(T);

	n = T->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(T->u.tensor->elem[i]);
		push(p1);
		add();
		T->u.tensor->elem[i] = pop();
	}

	push(T);
}

void
flatten_terms(int h)
{
	int i, n;
	struct atom *p1;
	n = tos;
	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (car(p1) == symbol(ADD)) {
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
combine_tensors(int h)
{
	int i;
	struct atom *p1, *T;
	T = symbol(NIL);
	for (i = h; i < tos; i++) {
		p1 = stack[i];
		if (istensor(p1)) {
			if (istensor(T)) {
				push(T);
				push(p1);
				add_tensors();
				T = pop();
			} else
				T = p1;
			slice(i, 1);
			i--; // use same index again
		}
	}
	return T;
}

void
add_tensors(void)
{
	int i, n;
	struct atom *p1, *p2;

	p2 = pop();
	p1 = pop();

	if (!compatible_dimensions(p1, p2))
		stopf("incompatible tensor arithmetic");

	p1 = copy_tensor(p1);

	n = p1->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(p1->u.tensor->elem[i]);
		push(p2->u.tensor->elem[i]);
		add();
		p1->u.tensor->elem[i] = pop();
	}

	push(p1);
}

void
combine_terms(int h)
{
	int i;
	sort_terms(h);
	for (i = h; i < tos; i++) {
		if (iszero(stack[i])) {
			slice(i, 1); // remove
			i--; // use same index again
			continue;
		}
		if (i + 1 < tos && combine_terms_nib(i)) {
			slice(i + 1, 1); // remove
			i--; // use same index again
		}
	}
}

int
combine_terms_nib(int i)
{
	int denorm;
	struct atom *coeff1, *coeff2, *p1, *p2;

	p1 = stack[i];
	p2 = stack[i + 1];

	if (iszero(p2))
		return 1;

	if (isnum(p1) && isnum(p2)) {
		add_numbers(p1, p2);
		stack[i] = pop();
		return 1;
	}

	if (isnum(p1) || isnum(p2))
		return 0; // cannot add number and something else

	coeff1 = one;
	coeff2 = one;

	denorm = 0;

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		denorm = 1;
		if (isnum(car(p1))) {
			coeff1 = car(p1);
			p1 = cdr(p1);
			if (cdr(p1) == symbol(NIL)) {
				p1 = car(p1);
				denorm = 0;
			}
		}
	}

	if (car(p2) == symbol(MULTIPLY)) {
		p2 = cdr(p2);
		if (isnum(car(p2))) {
			coeff2 = car(p2);
			p2 = cdr(p2);
			if (cdr(p2) == symbol(NIL))
				p2 = car(p2);
		}
	}

	if (!equal(p1, p2))
		return 0;

	add_numbers(coeff1, coeff2);

	coeff1 = pop();

	if (iszero(coeff1)) {
		stack[i] = coeff1;
		return 1;
	}

	if (isplusone(coeff1) && !isdouble(coeff1)) {
		if (denorm) {
			push_symbol(MULTIPLY);
			push(p1); // p1 is a list, not an atom
			cons(); // prepend MULTIPLY
		} else
			push(p1);
	} else {
		if (denorm) {
			push_symbol(MULTIPLY);
			push(coeff1);
			push(p1); // p1 is a list, not an atom
			cons(); // prepend coeff1
			cons(); // prepend MULTIPLY
		} else {
			push_symbol(MULTIPLY);
			push(coeff1);
			push(p1);
			list(3);
		}
	}

	stack[i] = pop();

	return 1;
}

void
sort_terms(int h)
{
	qsort(stack + h, tos - h, sizeof (struct atom *), sort_terms_func);
}

int
sort_terms_func(const void *q1, const void *q2)
{
	return cmp_terms(*((struct atom **) q1), *((struct atom **) q2));
}

int
cmp_terms(struct atom *p1, struct atom *p2)
{
	int a, b, c;

	// 1st level: imaginary terms on the right

	a = isimaginaryterm(p1);
	b = isimaginaryterm(p2);

	if (a == 0 && b == 1)
		return -1; // ok

	if (a == 1 && b == 0)
		return 1; // out of order

	// 2nd level: numericals on the right

	if (isnum(p1) && isnum(p2))
		return 0; // don't care about order, save time, don't compare

	if (isnum(p1))
		return 1; // out of order

	if (isnum(p2))
		return -1; // ok

	// 3rd level: sort by factors

	a = 0;
	b = 0;

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		a = 1; // p1 is a list of factors
		if (isnum(car(p1))) {
			// skip over coeff
			p1 = cdr(p1);
			if (cdr(p1) == symbol(NIL)) {
				p1 = car(p1);
				a = 0;
			}
		}
	}

	if (car(p2) == symbol(MULTIPLY)) {
		p2 = cdr(p2);
		b = 1; // p2 is a list of factors
		if (isnum(car(p2))) {
			// skip over coeff
			p2 = cdr(p2);
			if (cdr(p2) == symbol(NIL)) {
				p2 = car(p2);
				b = 0;
			}
		}
	}

	if (a == 0 && b == 0)
		return cmp_factors(p1, p2);

	if (a == 0 && b == 1) {
		c = cmp_factors(p1, car(p2));
		if (c == 0)
			c = -1; // lengthf(p1) < lengthf(p2)
		return c;
	}

	if (a == 1 && b == 0) {
		c = cmp_factors(car(p1), p2);
		if (c == 0)
			c = 1; // lengthf(p1) > lengthf(p2)
		return c;
	}

	while (iscons(p1) && iscons(p2)) {
		c = cmp_factors(car(p1), car(p2));
		if (c)
			return c;
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	if (iscons(p1))
		return 1; // lengthf(p1) > lengthf(p2)

	if (iscons(p2))
		return -1; // lengthf(p1) < lengthf(p2)

	return 0;
}

int
simplify_terms(int h)
{
	int i, n = 0;
	struct atom *p1, *p2;
	for (i = h; i < tos; i++) {
		p1 = stack[i];
		if (isradicalterm(p1)) {
			push(p1);
			evalf();
			p2 = pop();
			if (!equal(p1, p2)) {
				stack[i] = p2;
				n++;
			}
		}
	}
	return n;
}

int
isradicalterm(struct atom *p)
{
	return car(p) == symbol(MULTIPLY) && isnum(cadr(p)) && isradical(caddr(p));
}

int
isimaginaryterm(struct atom *p)
{
	if (isimaginaryfactor(p))
		return 1;
	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);
		while (iscons(p)) {
			if (isimaginaryfactor(car(p)))
				return 1;
			p = cdr(p);
		}
	}
	return 0;
}

int
isimaginaryfactor(struct atom *p)
{
	return car(p) == symbol(POWER) && isminusone(cadr(p));
}

void
add_numbers(struct atom *p1, struct atom *p2)
{
	double d1, d2;

	if (isrational(p1) && isrational(p2)) {
		add_rationals(p1, p2);
		return;
	}

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	push_double(d1 + d2);
}

void
add_rationals(struct atom *p1, struct atom *p2)
{
	int sign;
	uint32_t *a, *ab, *b, *ba, *c;

	if (iszero(p1)) {
		push(p2);
		return;
	}

	if (iszero(p2)) {
		push(p1);
		return;
	}

	if (isinteger(p1) && isinteger(p2)) {
		add_integers(p1, p2);
		return;
	}

	ab = mmul(p1->u.q.a, p2->u.q.b);
	ba = mmul(p1->u.q.b, p2->u.q.a);

	if (p1->sign == p2->sign) {
		a = madd(ab, ba);
		sign = p1->sign;
	} else {
		switch (mcmp(ab, ba)) {
		case 1:
			a = msub(ab, ba);
			sign = p1->sign;
			break;
		case 0:
			push_integer(0);
			mfree(ab);
			mfree(ba);
			return;
		case -1:
			a = msub(ba, ab);
			sign = p2->sign;
			break;
		default:
			// never gets here, fix compiler warning
			return;
		}
	}

	mfree(ab);
	mfree(ba);

	b = mmul(p1->u.q.b, p2->u.q.b);
	c = mgcd(a, b);

	push_bignum(sign, mdiv(a, c), mdiv(b, c));

	mfree(a);
	mfree(b);
	mfree(c);
}

void
add_integers(struct atom *p1, struct atom *p2)
{
	int sign = 0; // compiler nag
	uint32_t *c = NULL; // compiler nag
	if (p1->sign == p2->sign) {
		c = madd(p1->u.q.a, p2->u.q.a);
		sign = p1->sign;
	} else {
		switch (mcmp(p1->u.q.a, p2->u.q.a)) {
		case 1:
			c = msub(p1->u.q.a, p2->u.q.a);
			sign = p1->sign;
			break;
		case 0:
			push_integer(0);
			return;
		case -1:
			c = msub(p2->u.q.a, p1->u.q.a);
			sign = p2->sign;
			break;
		default:
			stopf("error");
		}
	}
	push_bignum(sign, c, mint(1));
}

void
subtract(void)
{
	negate();
	add();
}
