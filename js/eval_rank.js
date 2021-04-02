function
eval_rank(p1)
{
	var t = expanding;
	expanding = 1;

	push(cadr(p1));
	evalf();
	p1 = pop();

	if (istensor(p1))
		push_integer(p1.dim.length);
	else
		push_integer(0);

	expanding = t;
}
