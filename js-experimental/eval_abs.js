function
eval_abs(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	abs();
	expanding = t;
}
