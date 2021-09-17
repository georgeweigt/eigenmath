function
eval_cosh(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	cosh();
	expanding = t;
}
