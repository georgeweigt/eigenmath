function
eval_adj(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	adj();
	expanding = t;
}
