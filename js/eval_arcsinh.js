function
eval_arcsinh(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	arcsinh();
	expanding = t;
}
