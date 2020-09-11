function
any_radical_factors(h)
{
	var i, n;
	n = stack.length;
	for (i = h; i < n; i++)
		if (isradical(stack[i]))
			return 1;
	return 0;
}
