function
eval_inv(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	inv();
	expanding = t;
}
