function
eval_expcosh(p1)
{
	push(cadr(p1));
	evalf();
	expcosh();
}

function
expcosh()
{
	var p1;
	p1 = pop();
	push(p1);
	expfunc();
	push(p1);
	negate();
	expfunc();
	add();
	push_rational(1, 2);
	multiply();
}
