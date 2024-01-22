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
	numden();
	pop(); // discard numerator
}
