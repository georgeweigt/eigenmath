function
eval_sin(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	sin();
	expanding = t;
}
