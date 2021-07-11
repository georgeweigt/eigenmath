function
testeq(p1, p2)
{
	var p3;
	push(p1);
	push(p2);
	subtract();
	simplify();
	p3 = pop();
	return iszero(p3);
}
