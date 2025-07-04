// first author: github.com/phbillet

void
eval_sgn(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	sgnfunc();
}

void
sgnfunc(void)
{
	int i, n;
	struct atom *p1, *p2;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			sgnfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	p2 = p1;

	if (!isnum(p2)) {
		push(p2);
		floatfunc();
		p2 = pop();
	}

	if (!isnum(p2)) {
		push_symbol(SGN);
		push(p1);
		list(2);
		return;
	}

	if (iszero(p2)) {
		push_integer(0);
		return;
	}

	if (isnegativenumber(p2))
		push_integer(-1);
	else
		push_integer(1);
}
