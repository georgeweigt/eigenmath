void
eval_erf(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	erffunc();
}

void
erffunc(void)
{
	int i, n;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			erffunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = erf(d);
		push_double(d);
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	if (isnegativeterm(p1)) {
		push_symbol(ERF);
		push(p1);
		negate();
		list(2);
		negate();
		return;
	}

	push_symbol(ERF);
	push(p1);
	list(2);
}
