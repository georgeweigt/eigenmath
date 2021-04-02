function
eval_polar(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	polar();
	expanding = t;
}
