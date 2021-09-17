function
eval_expcosh(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	expcosh();
	expanding = t;
}
