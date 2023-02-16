function
eval_dim(p1)
{
	var k, p2;

	push(cadr(p1));
	evalf();
	p2 = pop();

	if (!istensor(p2)) {
		push_integer(1);
		return;
	}

	if (lengthf(p1) == 2)
		k = 1;
	else {
		push(caddr(p1));
		evalf();
		k = pop_integer();
	}

	if (k < 1 || k > p2.dim.length)
		stopf("dim 2nd arg: error");

	push_integer(p2.dim[k - 1]);
}
