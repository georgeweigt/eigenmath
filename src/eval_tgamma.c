void
eval_tgamma(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	tgammafunc();
}

void
tgammafunc(void)
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
			tgammafunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isnum(p1)) {
		push(p1);
		d = pop_double();
		d = tgamma(d);
		push_double(d);
		return;
	}

	push_symbol(TGAMMA);
	push(p1);
	list(2);
}
