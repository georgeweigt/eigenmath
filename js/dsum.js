function
dsum(p1, p2)
{
	var h = stack.length;

	p1 = cdr(p1);

	while (iscons(p1)) {
		push(car(p1));
		push(p2);
		derivative();
		p1 = cdr(p1);
	}

	add_terms(stack.length - h);
}
