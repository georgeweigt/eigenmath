function
testeq(p1, p2)
{
	var p3;
	push(p1);
	push(p2);
	subtract();
	p3 = pop();
	while (cross_expr(p3)) {
		push(p3);
		cancel_factor();
		p3 = pop();
	}
	return iszero(p3);
}
