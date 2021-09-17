function
eval_simplify(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	simplify();
	expanding = t;
}
