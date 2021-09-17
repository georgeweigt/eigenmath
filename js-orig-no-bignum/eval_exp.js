function
eval_exp(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	exp();
	expanding = t;
}
