// create a list from n things on the stack

void
list(int n)
{
	int i;
	push_symbol(NIL);
	for (i = 0; i < n; i++)
		cons();
}

void
cons(void)
{
	struct atom *p;
	p = alloc_atom();
	p->atomtype = CONS;
	p->u.cons.cdr = pop();
	p->u.cons.car = pop();
	push(p);
}

int
lengthf(struct atom *p)
{
	int n = 0;
	while (iscons(p)) {
		n++;
		p = cdr(p);
	}
	return n;
}

// returns 1 if expr p contains expr q, otherweise returns 0

int
findf(struct atom *p, struct atom *q)
{
	int i;

	if (equal(p, q))
		return 1;

	if (istensor(p)) {
		for (i = 0; i < p->u.tensor->nelem; i++)
			if (findf(p->u.tensor->elem[i], q))
				return 1;
		return 0;
	}

	while (iscons(p)) {
		if (findf(car(p), q))
			return 1;
		p = cdr(p);
	}

	return 0;
}

int
complexity(struct atom *p)
{
	int n = 1;

	while (iscons(p)) {
		n += complexity(car(p));
		p = cdr(p);
	}

	return n;
}

void
sort(int n)
{
	qsort(stack + tos - n, n, sizeof (struct atom *), sort_func);
}

int
sort_func(const void *p1, const void *p2)
{
	return cmp(*((struct atom **) p1), *((struct atom **) p2));
}

int
sign(int n)
{
	if (n < 0)
		return -1;
	if (n > 0)
		return 1;
	return 0;
}

int
find_denominator(struct atom *p)
{
	struct atom *q;
	p = cdr(p);
	while (iscons(p)) {
		q = car(p);
		if (car(q) == symbol(POWER) && isnegativenumber(caddr(q)))
			return 1;
		p = cdr(p);
	}
	return 0;
}

int
count_denominators(struct atom *p)
{
	int n = 0;
	p = cdr(p);
	while (iscons(p)) {
		if (isdenominator(car(p)))
			n++;
		p = cdr(p);
	}
	return n;
}

int
count_numerators(struct atom *p)
{
	int n = 0;
	p = cdr(p);
	while (iscons(p)) {
		if (isnumerator(car(p)))
			n++;
		p = cdr(p);
	}
	return n;
}

int
cmpfunc(void)
{
	struct atom *p1, *p2;
	p2 = pop();
	p1 = pop();
	return cmp(p1, p2);
}

int
lessp(struct atom *p1, struct atom *p2)
{
	if (cmp(p1, p2) < 0)
		return 1;
	else
		return 0;
}

int
cmp(struct atom *p1, struct atom *p2)
{
	int n;

	if (p1 == p2)
		return 0;

	if (p1 == symbol(NIL))
		return -1;

	if (p2 == symbol(NIL))
		return 1;

	if (isnum(p1) && isnum(p2))
		return cmp_numbers(p1, p2);

	if (isnum(p1))
		return -1;

	if (isnum(p2))
		return 1;

	if (isstr(p1) && isstr(p2))
		return sign(strcmp(p1->u.str, p2->u.str));

	if (isstr(p1))
		return -1;

	if (isstr(p2))
		return 1;

	if (issymbol(p1) && issymbol(p2))
		return sign(strcmp(printname(p1), printname(p2)));

	if (issymbol(p1))
		return -1;

	if (issymbol(p2))
		return 1;

	if (istensor(p1) && istensor(p2))
		return cmp_tensors(p1, p2);

	if (istensor(p1))
		return -1;

	if (istensor(p2))
		return 1;

	while (iscons(p1) && iscons(p2)) {
		n = cmp(car(p1), car(p2));
		if (n != 0)
			return n;
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	if (iscons(p2))
		return -1;

	if (iscons(p1))
		return 1;

	return 0;
}

int
cmp_numbers(struct atom *p1, struct atom *p2)
{
	double d1, d2;

	if (isrational(p1) && isrational(p2))
		return cmp_rationals(p1, p2);

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	if (d1 < d2)
		return -1;

	if (d1 > d2)
		return 1;

	return 0;
}

int
cmp_rationals(struct atom *a, struct atom *b)
{
	int t;
	uint32_t *ab, *ba;
	if (a->sign == MMINUS && b->sign == MPLUS)
		return -1;
	if (a->sign == MPLUS && b->sign == MMINUS)
		return 1;
	if (isinteger(a) && isinteger(b)) {
		if (a->sign == MMINUS)
			return mcmp(b->u.q.a, a->u.q.a);
		else
			return mcmp(a->u.q.a, b->u.q.a);
	}
	ab = mmul(a->u.q.a, b->u.q.b);
	ba = mmul(a->u.q.b, b->u.q.a);
	if (a->sign == MMINUS)
		t = mcmp(ba, ab);
	else
		t = mcmp(ab, ba);
	mfree(ab);
	mfree(ba);
	return t;
}

int
cmp_tensors(struct atom *p1, struct atom *p2)
{
	int i;

	if (p1->u.tensor->ndim < p2->u.tensor->ndim)
		return -1;

	if (p1->u.tensor->ndim > p2->u.tensor->ndim)
		return 1;

	for (i = 0; i < p1->u.tensor->ndim; i++) {
		if (p1->u.tensor->dim[i] < p2->u.tensor->dim[i])
			return -1;
		if (p1->u.tensor->dim[i] > p2->u.tensor->dim[i])
			return 1;
	}

	for (i = 0; i < p1->u.tensor->nelem; i++) {
		if (equal(p1->u.tensor->elem[i], p2->u.tensor->elem[i]))
			continue;
		if (lessp(p1->u.tensor->elem[i], p2->u.tensor->elem[i]))
			return -1;
		else
			return 1;
	}

	return 0;
}

int
cmp_args(struct atom *p1)
{
	struct atom *p2, *p3;

	push(cadr(p1));
	evalf();
	p2 = pop();

	push(caddr(p1));
	evalf();
	p3 = pop();

	if (!isnum(p2)) {
		push(p2);
		floatfunc();
		p2 = pop();
	}

	if (!isnum(p3)) {
		push(p3);
		floatfunc();
		p3 = pop();
	}

	if (!isnum(p2) || !isnum(p3))
		stopf("compare err");

	return cmp(p2, p3);
}

// faster than cmp

int
equal(struct atom *p1, struct atom *p2)
{
	int i, n;
	double d;

	if (p1 == p2)
		return 1;

	if (istensor(p1) && istensor(p2)) {
		if (p1->u.tensor->ndim != p2->u.tensor->ndim)
			return 0;
		n = p1->u.tensor->ndim;
		for (i = 0; i < n; i++)
			if (p1->u.tensor->dim[i] != p2->u.tensor->dim[i])
				return 0;
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++)
			if (!equal(p1->u.tensor->elem[i], p2->u.tensor->elem[i]))
				return 0;
		return 1;
	}

	if (iscons(p1) && iscons(p2)) {
		while (iscons(p1) && iscons(p2)) {
			if (!equal(car(p1), car(p2)))
				return 0;
			p1 = cdr(p1);
			p2 = cdr(p2);
		}
		if (p1 == symbol(NIL) && p2 == symbol(NIL))
			return 1;
		else
			return 0;
	}

	if (isrational(p1) && isrational(p2)) {
		if (p1->sign != p2->sign)
			return 0;
		if (!meq(p1->u.q.a, p2->u.q.a))
			return 0;
		if (!meq(p1->u.q.b, p2->u.q.b))
			return 0;
		return 1;
	}

	if (isrational(p1) && isdouble(p2)) {
		push(p1);
		d = pop_double();
		if (d == p2->u.d)
			return 1;
		else
			return 0;
	}

	if (isdouble(p1) && isrational(p2)) {
		push(p2);
		d = pop_double();
		if (p1->u.d == d)
			return 1;
		else
			return 0;
	}

	if (isdouble(p1) && isdouble(p2)) {
		if (p1->u.d == p2->u.d)
			return 1;
		else
			return 0;
	}

	if (iskeyword(p1) && iskeyword(p2)) {
		if (strcmp(p1->u.ksym.name, p2->u.ksym.name) == 0)
			return 1;
		else
			return 0;
	}

	if (isstr(p1) && isstr(p2)) {
		if (strcmp(p1->u.str, p2->u.str) == 0)
			return 1;
		else
			return 0;
	}

	return 0;
}
