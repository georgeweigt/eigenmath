function
eval_arccosh(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	arccosh();
	expanding = t;
}
