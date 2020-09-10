function
append()
{
	var h, p1, p2;

	p2 = pop();
	p1 = pop();

	h = stack.length;

	if (iscons(p1)) {
		while (iscons(p1)) {
			push(car(p1));
			p1 = cdr(p1);
		}
	} else
		push(p1);

	if (iscons(p2)) {
		while (iscons(p2)) {
			push(car(p2));
			p2 = cdr(p2);
		}
	} else
		push(p2);

	list(stack.length - h);
}
