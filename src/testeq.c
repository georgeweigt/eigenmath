void
eval_testeq(struct atom *p1)
{
	struct atom *p2, *p3;

	push(cadr(p1));
	eval();

	push(caddr(p1));
	eval();

	p2 = pop();
	p1 = pop();

	// null tensors are equal no matter the dimensions

	if (iszero(p1) && iszero(p2)) {
		push_integer(1);
		return;
	}

	// shortcut for trivial equality

	if (equal(p1, p2)) {
		push_integer(1);
		return;
	}

	// otherwise subtract and simplify

	if (!istensor(p1) && !istensor(p2)) {
		if (!iscons(p1) && !iscons(p2)) {
			push_integer(0); // p1 and p2 are numbers, symbols, or strings
			return;
		}
		push(p1);
		push(p2);
		subtract();
		simplify();
		p1 = pop();
		if (iszero(p1))
			push_integer(1);
		else
			push_integer(0);
		return;
	}

	if (istensor(p1) && istensor(p2)) {
		if (!compatible_dimensions(p1, p2)) {
			push_integer(0);
			return;
		}
		push(p1);
		push(p2);
		subtract();
		simplify();
		p1 = pop();
		if (iszero(p1))
			push_integer(1);
		else
			push_integer(0);
		return;
	}

	if (istensor(p2)) {
		// swap p1 and p2
		p3 = p1;
		p1 = p2;
		p2 = p3;
	}

	if (!iszero(p2)) {
		push_integer(0); // tensor not equal scalar
		return;
	}

	push(p1);
	simplify();
	p1 = pop();

	if (iszero(p1))
		push_integer(1);
	else
		push_integer(0);
}
