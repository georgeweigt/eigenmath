function
multiply_tensor_factors(h)
{
	var i, n, p1, T;
	T = symbol(NIL);
	n = stack.length;
	for (i = h; i < n; i++) {
		p1 = stack[i];
		if (!istensor(p1))
			continue;
		if (istensor(T)) {
			push(T);
			push(p1);
			hadamard();
			T = pop();
		} else
			T = p1;
		stack.splice(i, 1); // remove factor
		i--; // use same index again
		n--;
	}
	return T;
}
