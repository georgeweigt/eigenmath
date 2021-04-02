function
eval_dim(p1)
{
	var k, p2, t;

	t = expanding;
	expanding = 1;

	push(cadr(p1));
	evalf();
	p2 = pop();

	p1 = cddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		evalf();
		k = pop_integer();
	} else
		k = 1;

	if (!istensor(p2) || k < 1 || k > p2.dim.length)
		stopf("dim");

	push_integer(p2.dim[k - 1]);

	expanding = t;
}
