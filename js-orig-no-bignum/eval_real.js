function
eval_real(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	real();
	expanding = t;
}
