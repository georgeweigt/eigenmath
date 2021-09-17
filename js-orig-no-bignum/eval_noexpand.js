function
eval_noexpand(p1)
{
	var t;

	t = expanding;
	expanding = 0;

	push(cadr(p1));
	evalf();

	expanding = t;
}
