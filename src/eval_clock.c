void
eval_clock(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	clockfunc();
}

void
clockfunc(void)
{
	int i, n;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			clockfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	magfunc();

	push_integer(-1); // base

	push(p1);
	argfunc();
	push_symbol(PI);
	divide();

	power();

	multiply();
}
