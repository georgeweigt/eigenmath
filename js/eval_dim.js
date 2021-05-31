function
eval_dim(p1)
{
	var n, p2;

	push(cadr(p1));
	evalf();
	p2 = pop();

	n = lengthf(p1);

	if (n == 2) {
		if (istensor(p2))
			push_integer(p2.elem.length);
		else
			push_integer(1);
		return;
	}

	if (n != 3)
		stopf("dim: 2 args expected");

	if (!istensor(p2))
		stopf("dim 1st arg: tensor expected");

	push(caddr(p1));
	evalf();
	n = pop_integer();

	if (n < 1 || n > p2.dim.length)
		stopf("dim 2nd arg: range error");

	push_integer(p2.dim[n - 1]);
}
