void
eval_erfc(struct atom *p1)
{
	push(cadr(p1));
	eval();
	erfcfunc();
}

void
erfcfunc(void)
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
			erfcfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = erfc(d);
		push_double(d);
		return;
	}

	if (iszero(p1)) {
		push_integer(1);
		return;
	}

	push_symbol(ERFC);
	push(p1);
	list(2);
}
