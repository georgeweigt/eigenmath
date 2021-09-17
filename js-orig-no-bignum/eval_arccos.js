function
eval_arccos(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	arccos();
	expanding = t;
}
