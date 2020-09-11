function
integral_search(F, h, table)
{
	var i, n, I, C;

	n = table.length;

	for (i = 0; i < n; i += 3) {

		scan1(table[i + 0]); // integrand
		I = pop();

		scan1(table[i + 2]); // condition
		C = pop();

		if (integral_search_nib(F, I, C, h))
			break;
	}

	stack.splice(h); // pop all

	if (i == n)
		return 0;

	scan1(table[i + 1]); // answer
	evalf();

	return 1;
}
