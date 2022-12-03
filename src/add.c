void
eval_add(struct atom *p1)
{
	int h = tos;
	expanding--; // undo expanding++ in eval
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		eval();
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
	int i, h;
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

	push(T);
	copy_tensor();
	T = pop();

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
	int i, j;
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
			for (j = i + 1; j < tos; j++)
				stack[j - 1] = stack[j];
			tos--;
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
		stop("incompatible tensor arithmetic");

	push(p1);
	copy_tensor();
	p1 = pop();

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
	int i, j;
	sort_terms(tos - h);
	for (i = h; i < tos - 1; i++) {
		if (combine_terms_nib(i, i + 1)) {
			if (iszero(stack[i])) {
				for (j = i + 2; j < tos; j++)
					stack[j - 2] = stack[j]; // remove 2 terms
				tos -= 2;
			} else {
				for (j = i + 2; j < tos; j++)
					stack[j - 1] = stack[j]; // remove 1 term
				tos -= 1;
			}
			i--; // use same index again
		}
	}
	if (h < tos && iszero(stack[tos - 1]))
		tos--;
}

int
combine_terms_nib(int i, int j)
{
	int denorm;
	struct atom *p1, *p2, *p3, *p4;

	p1 = stack[i];
	p2 = stack[j];

	if (iszero(p2))
		return 1;

	if (iszero(p1)) {
		stack[i] = p2;
		return 1;
	}

	if (isnum(p1) && isnum(p2)) {
		add_numbers(p1, p2);
		stack[i] = pop();
		return 1;
	}

	if (isnum(p1) || isnum(p2))
		return 0; // cannot add number and something else

	p3 = p1;
	p4 = p2;

	p1 = one;
	p2 = one;

	denorm = 0;

	if (car(p3) == symbol(MULTIPLY)) {
		p3 = cdr(p3);
		denorm = 1;
		if (isnum(car(p3))) {
			p1 = car(p3); // coeff
			p3 = cdr(p3);
			if (cdr(p3) == symbol(NIL)) {
				p3 = car(p3);
				denorm = 0;
			}
		}
	}

	if (car(p4) == symbol(MULTIPLY)) {
		p4 = cdr(p4);
		if (isnum(car(p4))) {
			p2 = car(p4); // coeff
			p4 = cdr(p4);
			if (cdr(p4) == symbol(NIL)) {
				p4 = car(p4);
			}
		}
	}

	if (!equal(p3, p4))
		return 0;

	add_numbers(p1, p2); // add p1 and p2

	p4 = pop(); // new coeff

	if (iszero(p4)) {
		stack[i] = p4;
		return 1;
	}

	if (isplusone(p4) && !isdouble(p4)) {
		if (denorm) {
			push_symbol(MULTIPLY);
			push(p3); // p3 is a list, not an atom
			cons(); // prepend MULTIPLY to p3
		} else
			push(p3);
	} else {
		if (denorm) {
			push_symbol(MULTIPLY);
			push(p4);
			push(p3); // p3 is a list, not an atom
			cons(); // prepend p4 to p3
			cons(); // prepend MULTIPLY
		} else {
			push_symbol(MULTIPLY);
			push(p4);
			push(p3);
			list(3);
		}
	}

	stack[i] = pop();

	return 1;
}

void
sort_terms(int n)
{
	qsort(stack + tos - n, n, sizeof (struct atom *), sort_terms_func);
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

	a = is_imaginary_term(p1);
	b = is_imaginary_term(p2);

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
			eval();
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
is_imaginary_term(struct atom *p)
{
	if (car(p) == symbol(POWER) && isminusone(cadr(p)))
		return 1;
	if (iscons(p)) {
		p = cdr(p);
		while (iscons(p)) {
			if (caar(p) == symbol(POWER) && isminusone(cadar(p)))
				return 1;
			p = cdr(p);
		}
	}
	return 0;
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
	int sign;
	uint32_t *a, *b, *c;

	a = p1->u.q.a;
	b = p2->u.q.a;

	if (p1->sign == p2->sign) {
		c = madd(a, b);
		sign = p1->sign;
	} else {
		switch (mcmp(a, b)) {
		case 1:
			c = msub(a, b);
			sign = p1->sign;
			break;
		case 0:
			push_integer(0);
			return;
		case -1:
			c = msub(b, a);
			sign = p2->sign;
			break;
		default:
			// never gets here, fix compiler warning
			return;
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
