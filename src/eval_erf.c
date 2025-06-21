// first author: github.com/phbillet

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
	struct atom *p1, *p2;

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

	push(p1);
	floatfunc();
	p2 = pop();

	if (isnum(p2)) {
		push(p2);
		d = pop_double();
		d = erf(d);
		push_double(d);
		return;
	}

	if (isnegativeterm(p1)) {
		push_symbol(ERF);
		push(p1);
		negate();
		list(2);
		negate();
	} else {
		push_symbol(ERF);
		push(p1);
		list(2);
	}
}
