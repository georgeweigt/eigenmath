void
eval_denominator(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	denominator();
}

void
denominator(void)
{
	struct atom *p0, *p1, *p2;

	p1 = pop();

	if (isrational(p1)) {
		push_bignum(MPLUS, mcopy(p1->u.q.b), mint(1));
		return;
	}

	p2 = one; // denominator

	while (divisor(p1)) {

		p0 = pop(); // p0 is a denominator

		push(p0); // cancel in orig expr
		push(p1);
		cancel_factor();
		p1 = pop();

		push(p0); // update denominator
		push(p2);
		cancel_factor();
		p2 = pop();
	}

	push(p2);
}
