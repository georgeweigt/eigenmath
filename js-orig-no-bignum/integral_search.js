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

	if (i == n)
		return 0;

	stack.splice(h); // pop all

	scan1(table[i + 1]); // answer
	evalf();

	return 1;
}

function
integral_search_nib(F, I, C, h)
{
	var i, j, n, p1;

	n = stack.length;

	for (i = h; i < n; i++) {

		set_symbol(symbol(SA), stack[i], symbol(NIL));

		for (j = h; j < n; j++) {

			set_symbol(symbol(SB), stack[j], symbol(NIL));

			push(C);			// condition ok?
			evalf();
			p1 = pop();
			if (iszero(p1))
				continue;		// no, go to next j

			push(F);			// F = I?
			push(I);
			evalf();
			subtract();
			p1 = pop();
			if (iszero(p1))
				return 1;		// yes
		}
	}

	return 0;					// no
}
