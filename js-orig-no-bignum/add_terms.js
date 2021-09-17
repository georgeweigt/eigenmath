function
add_terms(n) // n is number of terms on stack
{
	var h, i, p1, T;

	if (n < 2)
		return;

	h = stack.length - n;

	flatten_terms(h);

	T = combine_tensors(h);

	combine_terms(h);

	n = stack.length - h;

	if (n == 0) {
		if (istensor(T))
			push(T);
		else
			push_integer(0);
		return;
	}

	if (n > 1) {
		list(n);
		push_symbol(ADD);
		swap();
		cons();
	}

	if (!istensor(T))
		return;

	p1 = pop();

	T = copy_tensor(T);

	n = T.elem.length;

	for (i = 0; i < n; i++) {
		push(T.elem[i]);
		push(p1);
		add();
		T.elem[i] = pop();
	}

	push(T);
}
