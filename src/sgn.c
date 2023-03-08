void
eval_sgn(struct atom *p1)
{
	push(cadr(p1));
	eval();
	sgn();
}

void
sgn(void)
{
	int i, n;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			sgn();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (!isnum(p1)) {
		push_symbol(SGN);
		push(p1);
		list(2);
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	if (isnegativenumber(p1))
		push_integer(-1);
	else
		push_integer(1);
}
