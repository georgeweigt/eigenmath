function
cancel_factor()
{
	var h, p1, p2;

	p2 = pop();
	p1 = pop();

	if (car(p2) == symbol(ADD)) {
		h = stack.length;
		p2 = cdr(p2);
		while (iscons(p2)) {
			push(p1);
			push(car(p2));
			multiply();
			p2 = cdr(p2);
		}
		add_terms(stack.length - h);
		return;
	}

	push(p1);
	push(p2);
	multiply();
}
