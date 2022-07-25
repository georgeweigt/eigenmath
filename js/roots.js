function
roots()
{
	var h, i, j, n;
	var A, C, LIST, P, X;

	X = pop();
	P = pop();

	h = stack.length;

	coeffs(P, X); // put coeffs on stack

	LIST = symbol(NIL);

	while (stack.length - h > 1) {

		C = pop(); // leading coeff

		if (iszero(C))
			continue; // coeff of monomial is zero

		// divide through by C

		for (i = h; i < stack.length; i++) {
			push(stack[i]);
			push(C);
			divide();
			stack[i] = pop();
		}

		push_integer(1); // new leading coeff

		if (findroot(h) == 0)
			break; // no root found

		A = pop(); // root

		push(A);
		push(LIST);
		cons(); // prepend A to LIST
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

	sort(n); // sort roots

	// eliminate repeated roots

	for (i = 0; i < n - 1; i++)
		if (equal(stack[i], stack[i + 1])) {
			for (j = i + i; j < n - 1; j++)
				stack[j] = stack[j + 1];
			i--;
			n--;
		}

	if (n == 1) {
		stack.splice(h + 1);
		return;
	}

	A = alloc_vector(n);

	for (i = 0; i < n; i++)
		A.elem[i] = stack[h + i];

	stack.splice(h); // pop all

	push(A);
}
