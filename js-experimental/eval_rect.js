function
eval_rect(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	rect();
	expanding = t;
}
