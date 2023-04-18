void
eval_noexpand(struct atom *p1)
{
	int t;

	t = expanding;
	expanding = 0;

	push(cadr(p1));
	evalf();

	expanding = t;
}
