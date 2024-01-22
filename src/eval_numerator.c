void
eval_numerator(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	numerator();
}

void
numerator(void)
{
	numden();
	swap();
	pop(); // discard denominator
}
