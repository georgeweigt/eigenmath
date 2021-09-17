function
eval_factorial(p1)
{
	var t = expanding;
	expanding = 1;
	push(cadr(p1));
	evalf();
	factorial();
	expanding = t;
}
