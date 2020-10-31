function
eval_dim(p1)
{
	var k = 1, p2;

	push(cadr(p1));
	evalf();
	p2 = pop();

	p1 = cddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		evalf();
		k = pop_integer();
	}

	if (!istensor(p2) || k < 1 || k > p2.dim.length)
		stopf("dim: error");

	push_integer(p2.dim[k - 1]);
}
