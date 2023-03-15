void
cancel_factor(void)
{
	int h;
	struct atom *p1, *p2;

	p2 = pop();
	p1 = pop();

	if (car(p2) == symbol(ADD)) {
		h = tos;
		p2 = cdr(p2);
		while (iscons(p2)) {
			push(p1);
			push(car(p2));
			multiply();
			p2 = cdr(p2);
		}
		add_terms(tos - h);
		return;
	}

	push(p1);
	push(p2);
	multiply();
}
