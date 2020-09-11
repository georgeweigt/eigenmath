function
rewrite(p0) // p0 is arg subst list
{
	var count, h, i, n, p1, p2;

	p1 = pop();

	count = 0;

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			count += rewrite(p0);
			p1.elem[i] = pop();
		}
		push(p1);
		return count;
	}

	if (iscons(p1)) {

		h = stack.length;

		push(car(p1)); // don't rewrite function name
		p1 = cdr(p1);

		while (iscons(p1)) {
			push(car(p1));
			count += rewrite(p0);
			p1 = cdr(p1);
		}

		list(stack.length - h);

		return count;
	}

	// if not a symbol then done

	if (!issymbol(p1)) {
		push(p1);
		return 0; // no substitution
	}

	// check argument substitution list

	p2 = p0;
	while (iscons(p2)) {
		if (p1 == car(p2)) {
			push(cadr(p2));
			return 1; // substitution occurred
		}
		p2 = cddr(p2);
	}

	// get the symbol's binding, try again

	p2 = get_binding(p1);

	if (p1 == p2) {
		push(p1);
		return 0; // no substitution
	}

	push(p2);

	count = rewrite(p0);

	if (count == 0) {
		pop(); // undo
		push(p1);
	}

	return count;
}
