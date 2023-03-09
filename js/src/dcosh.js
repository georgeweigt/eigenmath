function
dcosh(p1, p2)
{
	push(cadr(p1));
	push(p2);
	derivative();
	push(cadr(p1));
	sinhfunc();
	multiply();
}
