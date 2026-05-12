void
eval_abs(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	absfunc();
}

void
absfunc(void)
{
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		if (p1->u.tensor->ndim > 1) {
			push_symbol(ABS);
			push(p1);
			list(2);
			return;
		}
		push(p1);
		push(p1);
		conjfunc();
		inner();
		sqrtfunc();
		return;
	}

	if (isnegativenumber(p1)) {
		push(p1);
		negate();
		return;
	}

	if (iscons(p1)) {
		push(p1);
		push(p1);
		conjfunc();
		multiply();
		sqrtfunc();
		return;
	}

	push(p1);
}
