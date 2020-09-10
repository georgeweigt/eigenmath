// p is arg subst list

function
rewrite(p)
{
	var h, i, m, n, p1, p2;

	p1 = pop();

	n = 0;

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		m = p1.elem.length;
		for (i = 0; i < m; i++) {
			push(p1.elem[i]);
			n += rewrite(p);
			p1.elem[i] = pop();
		}
		push(p1);
		return n;
	}

	if (iscons(p1)) {

		h = stack.length;

		push(car(p1)); // don't rewrite function name
		p1 = cdr(p1);

		while (iscons(p1)) {
			push(car(p1));
			n += rewrite(p);
			p1 = cdr(p1);
		}

		list(stack.length - h);

		return n;
	}

	// if not a symbol then done

	if (!issymbol(p1)) {
		push(p1);
		return 0; // no substitution
	}

	// check argument substitution list

	p2 = p;
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

	n = rewrite(p);

	if (n == 0) {
		pop(); // undo
		push(p1);
	}

	return n;
}
