function
eval_expsin(p1)
{
	push(cadr(p1));
	evalf();
	expsin();
}

function
expsin()
{
	var p1;
	p1 = pop();

	push(imaginaryunit);
	push(p1);
	multiply();
	expfunc();
	push(imaginaryunit);
	divide();
	push_rational(1, 2);
	multiply();

	push(imaginaryunit);
	negate();
	push(p1);
	multiply();
	expfunc();
	push(imaginaryunit);
	divide();
	push_rational(1, 2);
	multiply();

	subtract();
}
