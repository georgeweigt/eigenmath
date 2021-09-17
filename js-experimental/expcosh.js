function
expcosh()
{
	var p1 = pop();
	push(p1);
	exp();
	push(p1);
	negate();
	exp();
	add();
	push_rational(1, 2);
	multiply();
}
