function
roots()
{
	var h, i, n;
	var A, C, LIST, P, X;

	X = pop();
	P = pop();

	h = stack.length;

	coeffs(P, X); // put coeffs on stack

	LIST = symbol(NIL);

	while (stack.length - h > 1) {

		C = pop(); // leading coeff

		if (iszero(C))
			continue;

		// divide through by C

		for (i = h; i < stack.length; i++) {
			push(stack[i]);
			push(C);
			divide();
			stack[i] = pop();
		}

		push_integer(1); // leading coeff

		if (findroot(h) == 0)
			break;

		A = pop(); // root

		push(A);
		push(LIST);
		cons(); // prepend A to list LIST
		LIST = pop();

		reduce(h, A); // divide by X - A
	}

	stack.splice(h); // pop all

	n = lengthf(LIST);

	if (n == 0)
		stopf("no roots found");

	if (n == 1) {
		push(car(LIST));
		return;
	}

	for (i = 0; i < n; i++) {
		push(car(LIST));
		LIST = cdr(LIST);
	}

	sort(n);

	A = alloc_vector(n);

	for (i = 0; i < n; i++)
		A.elem[i] = stack[h + i];

	stack.splice(h); // pop all

	push(A);
}
