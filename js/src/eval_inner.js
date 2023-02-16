function
eval_inner(p1)
{
	var h = stack.length;

	// evaluate from right to left

	p1 = cdr(p1);

	while (iscons(p1)) {
		push(car(p1));
		p1 = cdr(p1);
	}

	if (h == stack.length)
		stopf("dot");

	evalf();

	while (stack.length - h > 1) {
		p1 = pop();
		evalf();
		push(p1);
		inner();
	}
}
