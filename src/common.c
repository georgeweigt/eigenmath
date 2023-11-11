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

// sort n things on the stack

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
cmp(struct atom *p1, struct atom *p2)
{
	int t;

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
		return strcmp(p1->u.str, p2->u.str);

	if (isstr(p1))
		return -1;

	if (isstr(p2))
		return 1;

	if (issymbol(p1) && issymbol(p2))
		return strcmp(printname(p1), printname(p2));

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
		t = cmp(car(p1), car(p2));
		if (t)
			return t;
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	if (iscons(p2))
		return -1; // lengthf(p1) < lengthf(p2)

	if (iscons(p1))
		return 1; // lengthf(p1) > lengthf(p2)

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
	int i, t;

	t = p1->u.tensor->ndim - p2->u.tensor->ndim;

	if (t)
		return t;

	for (i = 0; i < p1->u.tensor->ndim; i++) {
		t = p1->u.tensor->dim[i] - p2->u.tensor->dim[i];
		if (t)
			return t;
	}

	for (i = 0; i < p1->u.tensor->nelem; i++) {
		t = cmp(p1->u.tensor->elem[i], p2->u.tensor->elem[i]);
		if (t)
			return t;
	}

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
