function
subst()
{
	var h, i, n, p1, p2, p3;

	p3 = pop(); // new expr
	p2 = pop(); // old expr

	if (p2 == symbol(NIL) || p3 == symbol(NIL))
		return;

	p1 = pop(); // expr

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			push(p2);
			push(p3);
			subst();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (iscons(p1)) {
		h = stack.length;
		while (iscons(p1)) {
			push(car(p1));
			push(p2);
			push(p3);
			subst();
			p1 = cdr(p1);
		}
		list(stack.length - h);
		return;
	}

	if (equal(p1, p2))
		push(p3);
	else
		push(p1);
}
