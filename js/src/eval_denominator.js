function
eval_denominator(p1)
{
	push(cadr(p1));
	evalf();
	denominator();
}

function
denominator()
{
	numden();
	pop(); // discard numerator
}
