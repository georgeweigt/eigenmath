void
eval_factorial(struct atom *p1)
{
	push(cadr(p1));
	eval();
	factorial();
}

void
factorial(void)
{
	int n;
	struct atom *p1;

	p1 = pop();

	if (!issmallinteger(p1)) {
		push_symbol(FACTORIAL);
		push(p1);
		list(2);
		return;
	}

	push(p1);
	n = pop_integer();

	if (n < 0) {
		push_symbol(FACTORIAL);
		push(p1);
		list(2);
		return;
	}

	bignum_factorial(n);

	if (isdouble(p1))
		bignum_float();
}
