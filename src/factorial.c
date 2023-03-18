void
eval_factorial(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	factorial();
}

void
factorial(void)
{
	int i, n;
	double m;
	struct atom *p1;

	p1 = pop();

	if (isposint(p1)) {
		push(p1);
		n = pop_integer();
		push_integer(1);
		for (i = 2; i <= n; i++) {
			push_integer(i);
			multiply();
		}
		return;
	}

	if (isdouble(p1) && p1->u.d >= 0 && floor(p1->u.d) == p1->u.d) {
		push(p1);
		n = pop_integer();
		m = 1.0;
		for (i = 2; i <= n; i++)
			m *= i;
		push_double(m);
		return;
	}

	push_symbol(FACTORIAL);
	push(p1);
	list(2);
}
