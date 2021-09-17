function
eval_expsin(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	expsin();
	expanding = t;
}
