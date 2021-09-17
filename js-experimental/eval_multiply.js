function
eval_multiply(p1)
{
	var h = stack.length;

	p1 = cdr(p1);

	while (iscons(p1)) {
		push(car(p1));
		evalf();
		p1 = cdr(p1);
	}

	multiply_factors(stack.length - h);
}
