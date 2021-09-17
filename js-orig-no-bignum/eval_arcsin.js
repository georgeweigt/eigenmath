function
eval_arcsin(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	arcsin();
	expanding = t;
}
