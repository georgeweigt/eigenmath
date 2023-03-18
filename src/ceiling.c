void
eval_ceiling(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	ceilingfunc();
}

void
ceilingfunc(void)
{
	int i, n;
	uint32_t *a, *b;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			ceilingfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isinteger(p1)) {
		push(p1);
		return;
	}

	if (isrational(p1)) {
		a = mdiv(p1->u.q.a, p1->u.q.b);
		b = mint(1);
		if (isnegativenumber(p1))
			push_bignum(MMINUS, a, b);
		else {
			push_bignum(MPLUS, a, b);
			push_integer(1);
			add();
		}
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = ceil(d);
		push_double(d);
		return;
	}

	push_symbol(CEILING);
	push(p1);
	list(2);
}
