function
eval_expsinh(p1)
{
	push(cadr(p1));
	evalf();
	expsinh();
}

function
expsinh()
{
	var p1;
	p1 = pop();
	push(p1);
	expfunc();
	push(p1);
	negate();
	expfunc();
	subtract();
	push_rational(1, 2);
	multiply();
}
