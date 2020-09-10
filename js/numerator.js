function
numerator()
{
	var p1;

	p1 = pop();

	while (cross_expr(p1)) {
		push(p1);
		cancel_factor();
		p1 = pop();
	}

	push(p1);
}
