function
eval_denominator(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	denominator();
	expanding = t;
}
