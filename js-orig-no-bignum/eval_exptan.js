function
eval_exptan(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	exptan();
	expanding = t;
}
