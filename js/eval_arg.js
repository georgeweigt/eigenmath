function
eval_arg(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	arg();
	expanding = t;
}
