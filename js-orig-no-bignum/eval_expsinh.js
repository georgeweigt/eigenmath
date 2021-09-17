function
eval_expsinh(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	expsinh();
	expanding = t;
}
