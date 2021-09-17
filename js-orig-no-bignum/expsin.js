function
expsin()
{
	var p1 = pop();

	push(imaginaryunit);
	push(p1);
	multiply();
	exp();
	push(imaginaryunit);
	divide();
	push_rational(1, 2);
	multiply();

	push(imaginaryunit);
	negate();
	push(p1);
	multiply();
	exp();
	push(imaginaryunit);
	divide();
	push_rational(1, 2);
	multiply();

	subtract();
}
