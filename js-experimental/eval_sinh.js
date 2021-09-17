function
eval_sinh(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	sinh();
	expanding = t;
}
