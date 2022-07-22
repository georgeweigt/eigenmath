void
eval_floor(struct atom *p1)
{
	push(cadr(p1));
	eval();
	floorfunc();
}

void
floorfunc(void)
{
	uint32_t *a, *b;
	double d;
	struct atom *p1;

	p1 = pop();

	if (isinteger(p1)) {
		push(p1);
		return;
	}

	if (isrational(p1)) {
		a = mdiv(p1->u.q.a, p1->u.q.b);
		b = mint(1);
		if (isnegativenumber(p1)) {
			push_bignum(MMINUS, a, b);
			push_integer(-1);
			add();
		} else
			push_bignum(MPLUS, a, b);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = floor(d);
		push_double(d);
		return;
	}

	push_symbol(FLOOR);
	push(p1);
	list(2);
}
