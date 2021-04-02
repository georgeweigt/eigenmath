function
eval_expcos(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	expcos();
	expanding = t;
}
