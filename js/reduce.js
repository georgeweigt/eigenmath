// divide by X - A

function
reduce(h, A)
{
	var i, t;

	t = stack.length - 1;

	for (i = t; i > h; i--) {
		push(A);
		push(stack[i]);
		multiply();
		push(stack[i - 1]);
		add();
		stack[i - 1] = pop();
	}

	if (!iszero(stack[h]))
		stopf("root finder");

	for (i = h; i < t; i++)
		stack[i] = stack[i + 1];

	pop(); // one less coeff on stack
}
