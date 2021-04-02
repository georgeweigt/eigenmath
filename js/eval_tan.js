function
eval_tan(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	tan();
	expanding = t;
}
