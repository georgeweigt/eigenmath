function
multiply_factors(n) // n is number of factors on stack
{
	var h, T;

	if (n < 2)
		return;

	h = stack.length - n;

	flatten_factors(h);

	T = multiply_tensor_factors(h);

	multiply_scalar_factors(h);

	if (istensor(T)) {
		push(T);
		inner();
	}
}
