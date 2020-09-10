function
eval_testeq(p1)
{
	var i, n, p2, p3;

	push(cadr(p1));
	evalf();

	push(caddr(p1));
	evalf();

	p2 = pop();
	p1 = pop();

	// null tensors are equal no matter the dimensions

	if (iszero(p1) && iszero(p2)) {
		push_integer(1);
		return;
	}

	if (!istensor(p1) && !istensor(p2)) {
		if (testeq(p1, p2))
			push_integer(1);
		else
			push_integer(0);
		return;
	}

	if (!istensor(p1) && istensor(p2)) {
		p3 = p1;
		p1 = p2;
		p2 = p3;
	}

	if (istensor(p1) && !istensor(p2)) {
		if (!iszero(p2)) {
			push_integer(0); // tensor not equal scalar
			return;
		}
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			if (!testeq(p1.elem[i], zero)) {
				push_integer(0);
				return;
			}
		}
		push_integer(1);
		return;
	}

	// both p1 and p2 are tensors

	if (!compatible_dimensions(p1, p2)) {
		push_integer(0);
		return;
	}

	n = p1.elem.length;

	for (i = 0; i < n; i++) {
		if (!testeq(p1.elem[i], p2.elem[i])) {
			push_integer(0);
			return;
		}
	}

	push_integer(1);
}
