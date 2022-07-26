// divide by X - A

void
reduce(int h, struct atom *A)
{
	int i, t;

	t = tos - 1;

	for (i = t; i > h; i--) {
		push(A);
		push(stack[i]);
		multiply();
		push(stack[i - 1]);
		add();
		stack[i - 1] = pop();
	}

	if (!iszero(stack[h]))
		stop("roots: residual error");

	for (i = h; i < t; i++)
		stack[i] = stack[i + 1];

	pop(); // one less coeff on stack
}
