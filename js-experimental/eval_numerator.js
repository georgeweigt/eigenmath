function
eval_numerator(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	numerator();
	expanding = t;
}
