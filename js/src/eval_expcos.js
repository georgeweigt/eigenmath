function
eval_expcos(p1)
{
	push(cadr(p1));
	evalf();
	expcos();
}

function
expcos()
{
	var p1;
	p1 = pop();

	push(imaginaryunit);
	push(p1);
	multiply();
	expfunc();
	push_rational(1, 2);
	multiply();

	push(imaginaryunit);
	negate();
	push(p1);
	multiply();
	expfunc();
	push_rational(1, 2);
	multiply();

	add();
}
