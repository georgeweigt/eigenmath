function
eval_dim(p1)
{
	var n = 1, p2;

	push(cadr(p1));
	evalf();
	p2 = pop();

	p1 = cddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		evalf();
		n = pop_integer();
	}

	if (!istensor(p2)) {
		push_integer(1); // dim of scalar is 1
		return;
	}

	if (n < 1 || n > p2.ndim)
		stop("index error");

	push_integer(p2.dim[n - 1]);
}
