function
imag()
{
	var p1;
	rect();
	p1 = pop();
	push_rational(-1, 2);
	push(imaginaryunit);
	push(p1);
	push(p1);
	conj();
	subtract();
	multiply_factors(3);
}
