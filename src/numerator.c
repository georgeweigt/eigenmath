void
eval_numerator(struct atom *p1)
{
	push(cadr(p1));
	eval();
	numerator();
}

void
numerator(void)
{
	struct atom *p1;

	p1 = pop();

	if (isrational(p1)) {
		push_bignum(p1->sign, mcopy(p1->u.q.a), mint(1));
		return;
	}

	while (cross_expr(p1)) {
		push(p1);
		cancel_factor();
		p1 = pop();
	}

	push(p1);
}
