function
eval_circexp(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	circexp();
	expanding = t;
}
