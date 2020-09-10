function
expsinh()
{
	var p1 = pop();
	push(p1);
	exp();
	push(p1);
	negate();
	exp();
	subtract();
	push_rational(1, 2);
	multiply();
}
