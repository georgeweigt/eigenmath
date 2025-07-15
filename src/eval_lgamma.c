void
eval_lgamma(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	lgammafunc();
}

void
lgammafunc(void)
{
	int i, n;
	double d;
	struct atom *p1, *p2;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			lgammafunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	floatfunc();
	p2 = pop();

	if (isnum(p2)) {
		push(p2);
		d = pop_double();
		d = lgamma(d);
		push_double(d);
		return;
	}

	push_symbol(LGAMMA);
	push(p1);
	list(2);
}
