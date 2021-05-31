function
eval_dim(p1)
{
	var k, p2;

	push(cadr(p1));
	evalf();
	p2 = pop();

	if (lengthf(p1) < 3) {
		if (istensor(p2))
			push_integer(p2.elem.length);
		else
			push_integer(1);
		return;
	}

	if (!istensor(p2))
		stopf("dim 1st arg: tensor expected");

	push(caddr(p1));
	evalf();
	k = pop_integer();

	if (k < 1 || k > p2.dim.length)
		stopf("dim 2nd arg: range error");

	push_integer(p2.dim[k - 1]);
}
